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
    FINGERPRINTS    = 1,
    GRAZED          = 2,
    OCTAGON         = 3,
    SIGGRAPH        = 4,
};

class aperture
{
public:
    aperture(fft_engine& fft);

    void load_aperture(const transmission_function& tf,
                       float scale);

    void render_flare(const std::vector<light>& lights,
                      const gl::texture2D& occlusion,
                      const camera& camera,
                      float intensity,
                      float f_number);

    void render_ghosts(const std::vector<light>& lights,
                       const gl::texture2D& occlusion,
                       const camera& camera,
                       float intensity,
                       int ghost_count,
                       float ghost_size,
                       float ghost_brightness);

private:
    aperture& operator=(const aperture& other);
    aperture(const aperture& other);

    std::map<int, std::unique_ptr<gl::texture2D>> m_flares;

    gl::shader m_shader;
    gl::shader m_ghost_shader;
    fft_engine& m_fft;
    int m_flare_hash;

    image get_cfft(const image& aperture);
    image get_flare(const image& cfft, int radius);
    std::pair<int, float> compensate(
        const camera& camera, const light& light);
};

#endif
