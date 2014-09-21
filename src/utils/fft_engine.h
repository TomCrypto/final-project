#ifndef UTILS_FFT_ENGINE_H
#define UTILS_FFT_ENGINE_H

#include <glm/glm.hpp>
#include <fftw3.h>
#include <memory>
#include <unordered_map>

#include "utils/image.hpp"

class fft_engine
{
public:
    fft_engine(const glm::ivec2& max_dims);

    // Point spread function (power spectrum)
    // The transform may be done at a higher resolution than the image
    // dimensions for performance, the image will be zero-padded and
    // the output cropped to remain the same size
    // ONLY THE RED CHANNEL IS CONSIDERED
    image psf(const image& input, const glm::ivec2& dims);

    // Convolves the given image with a disk of given radius
    // (in units of the input's resolution)
    // the output image will have dimensions dim(input) + 2(radius, radius)
    image convolve_disk(const image& input, int radius);

private:
    struct ivec2_order_helper
    {
        size_t operator()(const glm::ivec2& k) const
        {
            return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
        }

        bool operator()(const glm::ivec2& a, const glm::ivec2& b) const
        {
                return a.x == b.x && a.y == b.y;
        }
    };

    typedef void(* buf_free_fn)(fftwf_complex*);
    typedef void(* plan_free_fn)(fftwf_plan_s*);

    fft_engine& operator=(fft_engine& other);
    fft_engine(fft_engine& other);
    const glm::ivec2 m_max_dims;

    std::unordered_map<glm::ivec2,
                       std::unique_ptr<fftwf_plan_s, plan_free_fn>,
                       ivec2_order_helper,
                       ivec2_order_helper> m_plans;
    std::unique_ptr<fftwf_complex, buf_free_fn> m_fft_buf;
    std::unique_ptr<fftwf_complex, buf_free_fn> m_tmp_buf;
};

#endif
