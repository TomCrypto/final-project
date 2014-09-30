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
                             FFTW_FORWARD, FFTW_ESTIMATE);
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

static void pack_image(const image& src, fftwf_complex* dst,
                       const channels& which, bool center)
{
    unsigned v = (unsigned)which;
    if (!(v && !(v & (v - 1)))) {
        LOG(ERROR) << "Bad channel";
        throw std::runtime_error("");
    }

    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        const glm::vec4* ptr = src[y];

        for (int x = 0; x < w; ++x)
        {
            float norm = center ? (float)pow(-1, x + y) : 1;

            if (channels::R & which)
                dst[y * w + x][0] = ptr->x * norm;
            else if (channels::G & which)
                dst[y * w + x][0] = ptr->y * norm;
            else if (channels::B & which)
                dst[y * w + x][0] = ptr->z * norm;

            dst[y * h + x][1] = 0;

            ++ptr;
        }
    }
}

static void unpack_image(const fftwf_complex* src, image& dst,
                         const channels& which,
                         const std::function<float(float,float)>& op)
{
    unsigned v = (unsigned)which;
    if (!(v && !(v & (v - 1)))) {
        LOG(ERROR) << "Bad channel";
        throw std::runtime_error("");
    }

    int w = dst.width();
    int h = dst.height();

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* ptr = dst[y];

        for (int x = 0; x < w; ++x)
        {
            float re = src[y * w + x][0];
            float im = src[y * w + x][1];

            if (channels::R & which)
                ptr->x = op(re, im) / (w * h);
            else if (channels::G & which)
                ptr->y = op(re, im) / (w * h);
            else if (channels::B & which)
                ptr->z = op(re, im) / (w * h);

            ++ptr;
        }
    }
}

static void pointwise_multiply(fftwf_complex* a, const fftwf_complex* b,
                               const glm::ivec2& dims)
{
    for (int y = 0; y < dims.y; ++y)
        for (int x = 0; x < dims.x; ++x)
        {
            float r1 = a[y * dims.x + x][0];
            float c1 = a[y * dims.x + x][1];
            float r2 = b[y * dims.x + x][0];
            float c2 = b[y * dims.x + x][1];

            float r = r1 * r2 - c1 * c2;
            float c = r1 * c2 + c1 * r2;

            a[y * dims.x + x][0] = r * (float)pow(-1, x + y);
            a[y * dims.x + x][1] = c * (float)pow(-1, x + y);
        }
}

fft_engine::fft_engine(const glm::ivec2& max_dims)
    : m_fft_buf(buf_alloc(max_dims), buf_free),
      m_tmp_buf(buf_alloc(max_dims), buf_free),
      m_max_dims(make_smooth(max_dims))
{
    LOG(TRACE) << "Requested maximum Fourier transform dimensions "
               << max_dims.x << " by " << max_dims.y
               << ", using "
               << m_max_dims.x << " by " << m_max_dims.y
               << ".";
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

    auto tmp = image(dims);

    pack_image(input.enlarge(dims), m_fft_buf.get(),
               channels::R, true);

    fftwf_execute(m_plans.at(dims).get());

    unpack_image(m_fft_buf.get(), tmp, channels::R,
                 [](float re, float im){return re * re + im * im;});

    int dx = (dims.x - input.width()) / 2;
    int dy = (dims.y - input.height()) / 2;

    return tmp.subregion(dx, dy, input.width(), input.height());
}

image fft_engine::convolve_disk(const image& _input, int radius)
{
    // work out convolution dimensions (at least dim(input) + 2 radius)
    const glm::ivec2& dims = make_smooth(
        glm::ivec2(_input.width(), _input.height())
      + glm::ivec2(2 * radius, 2 * radius));

    image disk = utils::draw_circle(radius, true).enlarge(dims);
    image input = _input.enlarge(dims);
    image convolved(dims);

    if (m_plans.find(dims) == m_plans.end()) {
        m_plans.insert(std::make_pair(dims,
            std::unique_ptr<fftwf_plan_s, plan_free_fn>(
                plan_alloc(dims, m_fft_buf.get()),
                plan_free)));
    }

    pack_image(disk, m_fft_buf.get(), channels::R, false);
    fftwf_execute(m_plans.at(dims).get());
    memcpy(m_tmp_buf.get(), m_fft_buf.get(),
           m_max_dims.x * m_max_dims.y * sizeof(fftwf_complex));

    for (int t = 0; t < 3; ++t) {
        channels which;
        if (t == 0) which = channels::R;
        if (t == 1) which = channels::G;
        if (t == 2) which = channels::B;

        pack_image(input, m_fft_buf.get(), which, false);

        // Convolution Theorem: FFT(A) . FFT(B) = FFT(A * B)
        // Where . denotes pointwise multiplication
        // And * denotes convolution

        fftwf_execute(m_plans.at(dims).get());
        pointwise_multiply(m_fft_buf.get(), m_tmp_buf.get(), dims);
        fftwf_execute(m_plans.at(dims).get());

        unpack_image(m_fft_buf.get(), convolved, which,
        [](float re, float im){ return sqrt(re * re + im * im);});
    }

    convolved.normalize(true);
    return convolved;
}
