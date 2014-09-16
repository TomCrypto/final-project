#include "scratch/fft.hpp"

#include <stdexcept>
#include <cassert>

/* Returns whether n is b-powersmooth. Used to select efficient discrete Fourier
 * transform dimensions for the convolution. */
static bool is_smooth(int n, int b)
{
    for (int f = 2; f <= b; ++f)
    {
        while (n % f == 0) n /= f;
        if (n == 1) return true;
    }

    return false;
}

fft_planner::fft_planner(int srcW, int srcH, int cnvW, int cnvH)
{
    assert((srcW > 0) && (srcH > 0) && (cnvW > 0) && (cnvH > 0));

    printf("Requested fft_planner for power spectrum %d x %d, convolve %d x %d\n",
           srcW, srcH, srcW + cnvW, srcH + cnvH);

    while (!is_smooth(srcW + cnvW, 5)) ++cnvW;
    while (!is_smooth(srcH + cnvH, 5)) ++cnvH;

    printf("Selecting fft_planner for power spectrum %d x %d, convolve %d x %d\n",
           srcW, srcH, srcW + cnvW, srcH + cnvH);

    this->powspectrum_buf = fftwf_alloc_complex(srcW * srcH);
    this->convolve_lt_buf = fftwf_alloc_complex((srcW + cnvW) * (srcH + cnvH));
    this->convolve_rt_buf = fftwf_alloc_complex((srcW + cnvW) * (srcH + cnvH));
    this->convolve_bk_buf = fftwf_alloc_complex((srcW + cnvW) * (srcH + cnvH));

    if (!this->powspectrum_buf || !this->convolve_lt_buf)
        throw std::runtime_error("Failed to allocate memory");

    this->powspectrum_plan = fftwf_plan_dft_2d(srcW, srcH,
        this->powspectrum_buf, this->powspectrum_buf, FFTW_FORWARD,
        FFTW_MEASURE);

    this->convolve_lt_plan = fftwf_plan_dft_2d(srcW + cnvW, srcH + cnvH,
        this->convolve_lt_buf, this->convolve_lt_buf, FFTW_FORWARD,
        FFTW_MEASURE);

    this->convolve_rt_plan = fftwf_plan_dft_2d(srcW + cnvW, srcH + cnvH,
        this->convolve_rt_buf, this->convolve_rt_buf, FFTW_FORWARD,
        FFTW_MEASURE);

    this->convolve_bk_plan = fftwf_plan_dft_2d(srcW + cnvW, srcH + cnvH,
        this->convolve_bk_buf, this->convolve_bk_buf, FFTW_BACKWARD,
        FFTW_MEASURE);

    if (!this->powspectrum_plan)
        throw std::runtime_error("Failed to compile FFTWlambda = 460.000000, color = (0.67, 0.00, 1.00 plan");

    if (!this->convolve_lt_plan)
        throw std::runtime_error("Failed to compile FFTW plan");

    this->srcW = srcW;
    this->srcH = srcH;
    this->cnvW = cnvW;
    this->cnvH = cnvH;
}

fft_planner::~fft_planner()
{
    fftwf_destroy_plan(this->powspectrum_plan);
    fftwf_destroy_plan(this->convolve_lt_plan);
    fftwf_destroy_plan(this->convolve_rt_plan);
    fftwf_destroy_plan(this->convolve_bk_plan);
    fftwf_free(this->powspectrum_buf);
    fftwf_free(this->convolve_lt_buf);
    fftwf_free(this->convolve_rt_buf);
    fftwf_free(this->convolve_bk_buf);
}

void fft_planner::power_spectrum_channel(const image& src, image& dst,
                                         const std::function<float(const glm::vec4&)>& src_op,
                                         const std::function<void(glm::vec4&, float, float)>& dst_op)
{
    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        const glm::vec4* ptr = src[y];

        for (int x = 0; x < w; ++x)
        {
            this->powspectrum_buf[y * srcW + x][0] = src_op(*ptr) * pow(-1, x + y);
            this->powspectrum_buf[y * srcW + x][1] = 0;

            ++ptr;
        }
    }

    fftwf_execute(this->powspectrum_plan);

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* ptr = dst[y];

        for (int x = 0; x < w; ++x)
        {
            float re = this->powspectrum_buf[y * srcW + x][0];
            float im = this->powspectrum_buf[y * srcW + x][1];
            dst_op(*ptr, re, im);

            ++ptr;
        }
    }
}

image fft_planner::power_spectrum(const image& src, const channels& which)
{
    image output(src.width(), src.height());
    int w = src.width(), h = src.height();

    if ((channels::R & which) != 0)
        power_spectrum_channel(src, output,
            [](const glm::vec4& a){return a.x;},
            [w, h](glm::vec4& a, float re, float im){a.x = (re * re + im * im) / (w * h);});
    if ((channels::G & which) != 0)
        power_spectrum_channel(src, output,
            [](const glm::vec4& a){return a.y;},
            [w, h](glm::vec4& a, float re, float im){a.y = (re * re + im * im) / (w * h);});
    if ((channels::B & which) != 0)
        power_spectrum_channel(src, output,
            [](const glm::vec4& a){return a.z;},
            [w, h](glm::vec4& a, float re, float im){a.z = (re * re + im * im) / (w * h);});

    return output;
}

const float epsilon = 1e-9f;

image fft_planner::convolve(const image& src, const image& cnv, const channels& which)
{
    // assert size here...

    //int w = src.width();
    //int h = src.width();

    image out(srcW + cnvW, srcH + cnvH);

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_lt_buf[y * (srcW + cnvW) + x][1] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][1] = 0;
        }

    if ((channels::R & which) != 0)
    {
        for (int y = 0; y < srcH; ++y)
        {
            const glm::vec4* srcPtr = src[y];

            for (int x = 0; x < srcW; ++x)
            {
                this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = srcPtr->x - epsilon;
                if (this->convolve_lt_buf[y * (srcW + cnvW) + x][0] < 0)
                    this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;

                ++srcPtr;
            }
        }

        int cnv_w = cnv.width(), cnv_h = cnv.height();

        for (int y = 0; y < cnv_h; ++y)
        {
            const glm::vec4* cnvPtr = cnv[y];

            for (int x = 0; x < cnv_h; ++x)
            {
                this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = cnvPtr->x;

                ++cnvPtr;
            }
        }
    }

    fftwf_execute(this->convolve_lt_plan);
    fftwf_execute(this->convolve_rt_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float r1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][0];
            float c1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][1];
            float r2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][0];
            float c2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][1];

            float r = r1 * r2 - c1 * c2;
            float c = r1 * c2 + c1 * r2;

            this->convolve_bk_buf[y * (srcW + cnvW) + x][0] = r;
            this->convolve_bk_buf[y * (srcW + cnvW) + x][1] = c;
        }

    fftwf_execute(this->convolve_bk_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
    {
        glm::vec4* ptr = out[y];

        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float re = this->convolve_bk_buf[y * (srcW + cnvW) + x][0] / ((srcW + cnvW) * (srcH + cnvH));
            float im = this->convolve_bk_buf[y * (srcW + cnvW) + x][1] / ((srcW + cnvW) * (srcH + cnvH)); // ??

            //ptr->x = re;
            ptr->x = sqrt(re * re + im * im);

            ++ptr;
        }
    }

    // TODO: REMOVE DUPLICATION AFTERWARDS

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_lt_buf[y * (srcW + cnvW) + x][1] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][1] = 0;
        }

    if ((channels::G & which) != 0)
    {
        for (int y = 0; y < srcH; ++y)
        {
            const glm::vec4* srcPtr = src[y];

            for (int x = 0; x < srcW; ++x)
            {
                this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = srcPtr->y - epsilon;
                if (this->convolve_lt_buf[y * (srcW + cnvW) + x][0] < 0)
                    this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;

                ++srcPtr;
            }
        }

        int cnv_w = cnv.width(), cnv_h = cnv.height();

        for (int y = 0; y < cnv_h; ++y)
        {
            const glm::vec4* cnvPtr = cnv[y];

            for (int x = 0; x < cnv_h; ++x)
            {
                this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = cnvPtr->y;

                ++cnvPtr;
            }
        }
    }

    fftwf_execute(this->convolve_lt_plan);
    fftwf_execute(this->convolve_rt_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float r1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][0];
            float c1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][1];
            float r2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][0];
            float c2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][1];

            float r = r1 * r2 - c1 * c2;
            float c = r1 * c2 + c1 * r2;

            this->convolve_bk_buf[y * (srcW + cnvW) + x][0] = r;
            this->convolve_bk_buf[y * (srcW + cnvW) + x][1] = c;
        }

    fftwf_execute(this->convolve_bk_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
    {
        glm::vec4* ptr = out[y];

        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float re = this->convolve_bk_buf[y * (srcW + cnvW) + x][0] / ((srcW + cnvW) * (srcH + cnvH)); // ??
            float im = this->convolve_bk_buf[y * (srcW + cnvW) + x][1] / ((srcW + cnvW) * (srcH + cnvH)); // ??

            //ptr->y = re;
            ptr->y = sqrt(re * re + im * im);

            ++ptr;
        }
    }

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_lt_buf[y * (srcW + cnvW) + x][1] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = 0;
            this->convolve_rt_buf[y * (srcW + cnvW) + x][1] = 0;
        }

    if ((channels::B & which) != 0)
    {
        for (int y = 0; y < srcH; ++y)
        {
            const glm::vec4* srcPtr = src[y];

            for (int x = 0; x < srcW; ++x)
            {
                this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = srcPtr->z - epsilon;
                if (this->convolve_lt_buf[y * (srcW + cnvW) + x][0] < 0)
                    this->convolve_lt_buf[y * (srcW + cnvW) + x][0] = 0;

                ++srcPtr;
            }
        }

        int cnv_w = cnv.width(), cnv_h = cnv.height();

        for (int y = 0; y < cnv_h; ++y)
        {
            const glm::vec4* cnvPtr = cnv[y];

            for (int x = 0; x < cnv_h; ++x)
            {
                this->convolve_rt_buf[y * (srcW + cnvW) + x][0] = cnvPtr->z;

                ++cnvPtr;
            }
        }
    }

    fftwf_execute(this->convolve_lt_plan);
    fftwf_execute(this->convolve_rt_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float r1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][0];
            float c1 = this->convolve_lt_buf[y * (srcW + cnvW) + x][1];
            float r2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][0];
            float c2 = this->convolve_rt_buf[y * (srcW + cnvW) + x][1];

            float r = r1 * r2 - c1 * c2;
            float c = r1 * c2 + c1 * r2;

            this->convolve_bk_buf[y * (srcW + cnvW) + x][0] = r;
            this->convolve_bk_buf[y * (srcW + cnvW) + x][1] = c;
        }

    fftwf_execute(this->convolve_bk_plan);

    for (int y = 0; y < srcH + cnvH; ++y)
    {
        glm::vec4* ptr = out[y];

        for (int x = 0; x < srcW + cnvW; ++x)
        {
            float re = this->convolve_bk_buf[y * (srcW + cnvW) + x][0] / ((srcW + cnvW) * (srcH + cnvH));
            float im = this->convolve_bk_buf[y * (srcW + cnvW) + x][1] / ((srcW + cnvW) * (srcH + cnvH)); // ??

            //ptr->z = re;
            ptr->z = sqrt(re * re + im * im);

            ++ptr;
        }
    }

    // END DUPLICATION

    return out;
}
