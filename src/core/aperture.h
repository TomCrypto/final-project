#ifndef CORE_APERTURE_H
#define CORE_APERTURE_H

#include <glm/glm.hpp>
#include <fftw3.h>

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "utils/fft_engine.h"
#include "utils/image.h"

#include "gui/framebuffer.h"
#include "core/camera.h"
#include "core/light.h"

enum transmission_function
{
    PENTAGON        = 0,
    CIRCLE          = 1,
};

// allows one to generate random apertures of a given size as well as their
// chromatic Fourier transform

class aperture
{
public:
    aperture(fft_engine& fft);

    void load_aperture(const transmission_function& tf,
                       float scale);

    // renders into/using the current framebuffer
    void render(const std::vector<light>& lights,
                const gl::texture2D& occlusion,
                const camera& camera,
                float w0, float i0);

private:
    aperture& operator=(const aperture& other);
    aperture(const aperture& other);

    std::map<int, std::unique_ptr<gl::texture2D>> m_flares;

    gl::shader m_shader;
    fft_engine& m_fft;

    image get_cfft(const image& aperture);
    image get_flare(const image& cfft, int radius);
};

#endif
