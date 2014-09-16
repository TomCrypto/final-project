// Converts an aperture texture to a lens flare

// It works as follows: the FlareGenerator class is created, and loads
// the necessary shaders and everything, then the caller can call the
// Generate() method which takes an aperture texture of given
// dimensions, and returns the corresponding lens flare

#ifndef FFT_H
#define FFT_H

#include <functional>
#include <cstdio>

#include <FreeImage.h>
#include <GL/glew.h>
#include <fftw3.h>

#include "utils/image.hpp"

class fft_planner
{
public:
    /* Instantiates an FFT planner of dimensions width x height. */
    fft_planner(int srcW, int srcH, int cnvW, int cnvH);
    ~fft_planner();

    /* Returns the power spectrum of a texture. */
    image power_spectrum(const image& src, const channels& which = channels::RGB);
    image convolve(const image& src, const image& cnv, const channels& which);

private:
    fftwf_complex* powspectrum_buf;
    fftwf_complex* convolve_lt_buf;
    fftwf_complex* convolve_rt_buf;
    fftwf_complex* convolve_bk_buf;
    int srcW, srcH, cnvW, cnvH;
    fftwf_plan powspectrum_plan;
    fftwf_plan convolve_lt_plan;
    fftwf_plan convolve_rt_plan;
    fftwf_plan convolve_bk_plan;

    void power_spectrum_channel(const image& src, image& dst,
                                const std::function<float(const glm::vec4&)>& src_op,
                                const std::function<void(glm::vec4&, float, float)>& dst_op);
};

#endif
