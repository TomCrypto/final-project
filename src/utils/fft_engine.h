/* Purpose:
 *
 *  - provides Fourier transform and image convolution services
 *
 * This class is essentially a massive cache of FFTW plans, to make it as easy
 * to use as possible. As the contents of this class takes a lot of memory, it
 * was essential to make it memory-leak-free, hence it uses smart pointers and
 * an unordered_map to guarantee memory is freed as soon as possible.
 *
 * It is also noncopyable, to force passing it by reference.
*/

#ifndef UTILS_FFT_ENGINE_H
#define UTILS_FFT_ENGINE_H

#include <glm/glm.hpp>
#include <fftw3.h>

#include <unordered_map>
#include <memory>

#include "utils/image.h"

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
    /* ivec2 hash function */
    struct ivec2_hash_helper
    {
        size_t operator()(const glm::ivec2& k) const
        {
            size_t p = 13;
            p ^= 17 * std::hash<int>()(k.x);
            p ^= 23 * std::hash<int>()(k.y);
            return p;
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
                       ivec2_hash_helper,
                       ivec2_hash_helper> m_plans;
    std::unique_ptr<fftwf_complex, buf_free_fn> m_fft_buf;
    std::unique_ptr<fftwf_complex, buf_free_fn> m_tmp_buf;
};

#endif
