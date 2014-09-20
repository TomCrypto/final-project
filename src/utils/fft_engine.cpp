#include <easylogging.h>
#include <stdexcept>

#include "utils/fft_engine.h"

static fftwf_complex* buf_alloc(const glm::ivec2& dims)
{
    return fftwf_alloc_complex(dims.x * dims.y);
}

static void buf_free(fftwf_complex* buf)
{
    fftwf_free(buf);
}

static fftwf_plan_s* plan_alloc(const glm::ivec2& dims, fftwf_complex* buf)
{
    return fftwf_plan_dft_2d(dims.x, dims.y, buf, buf,
                             FFTW_FORWARD, FFTW_MEASURE);
}

static void plan_free(fftwf_plan_s* plan)
{
    fftwf_destroy_plan(plan);
}

/* Returns whether n is 5-powersmooth. Used to select efficient
 * discrete Fourier transform dimensions. */
static bool is_smooth(int n)
{
    for (int f = 2; f <= 5; ++f)
    {
        while (n % f == 0) n /= f;
        if (n == 1) return true;
    }

    return false;
}

static glm::ivec2 make_smooth(const glm::ivec2& dims)
{
    int x = dims.x, y = dims.y;
    while (!is_smooth(x)) ++x;
    while (!is_smooth(y)) ++y;
    return glm::ivec2(x, y);
}

fft_engine::fft_engine(const glm::ivec2& max_dims)
    : m_fft_buf(buf_alloc(max_dims), buf_free),
      m_max_dims(make_smooth(max_dims))
{
    LOG(TRACE) << "Requested maximum size "
               << max_dims.x << " by " << max_dims.y
               << ", going with "
               << m_max_dims.x << " by " << m_max_dims.y
               << ".";
}

// FOR POWER SPECTRUM
static void pack_image(const image& src, fftwf_complex* dst)
{
    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        const glm::vec4* ptr = src[y];

        for (int x = 0; x < w; ++x)
        {
            dst[y * w + x][0] = ptr->x * (float)pow(-1, x + y);
            dst[y * h + x][1] = 0;

            ++ptr;
        }
    }
}

// FOR POWER SPECTRUM
static void unpack_image(const fftwf_complex* src, image& dst)
{
    int w = dst.width();
    int h = dst.height();

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* ptr = dst[y];

        for (int x = 0; x < w; ++x)
        {
            float re = src[y * w + x][0];
            float im = src[y * w + x][1];
            ptr->x = (re * re + im * im) / (w * h);
            ptr->y = 0;
            ptr->z = 0;

            ++ptr;
        }
    }
}

image fft_engine::psf(const image& input, const glm::ivec2& _dims)
{
    if ((input.width() >= m_max_dims.x) || (input.height() >= m_max_dims.y)) {
        LOG(ERROR) << "Requested FFT larger than max dimensions.";
        throw std::logic_error("");
    }
    
    if ((_dims.x < input.width()) || (_dims.y < input.height())) {
        LOG(ERROR) << "FFT dimensions smaller than image dimensions.";
        throw std::logic_error("");
    }

    glm::ivec2 dims = make_smooth(_dims);
    
    if (m_plans.find(dims) == m_plans.end()) {
        m_plans.insert(std::make_pair(dims,
            std::unique_ptr<fftwf_plan_s, plan_free_fn>(
                plan_alloc(dims, m_fft_buf.get()),
                plan_free)));
    }

    auto tmp = input.enlarge(dims.x, dims.y);

    pack_image(tmp, m_fft_buf.get());
    
    fftwf_execute(m_plans.at(dims).get());

    unpack_image(m_fft_buf.get(), tmp);
    
    int dx = (dims.x - input.width()) / 2;
    int dy = (dims.y - input.height()) / 2;
    
    return tmp.subregion(dx, dy, input.width(), input.height());
}
