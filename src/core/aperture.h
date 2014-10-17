#ifndef CORE_APERTURE_H
#define CORE_APERTURE_H

#include <glm/glm.hpp>
#include <fftw3.h>

#include <vector>
#include <string>
#include <map>

#include "utils/fft_engine.h"
#include "utils/texture2d.h"
#include "utils/shader.h"
#include "utils/image.h"

#include "gui/framebuffer.h"
#include "core/camera.h"
#include "core/light.h"

enum transmission_function
{
    PENTAGON        = 0,
    FINGERPRINTS    = 1,
    GRAZED          = 2,
    GRAZED_CUT      = 3,
    HUMAN_EYE       = 4,
    CUSTOM          = 5
};

class aperture
{
public:
    aperture(fft_engine& fft);

    void load_aperture(const transmission_function& tf,
                       float f_number);

    void render_flare(const std::vector<light>& lights,
                      const gl::texture2D& occlusion,
                      const camera& camera,
                      float intensity);

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

    gl::shader m_flare_shader;
    gl::shader m_ghost_shader;
    gl::texture2D m_spectrum;
    fft_engine& m_fft;
    int m_flare_hash;
    float m_f_number;

    image get_cfft(const image& aperture);
    image get_flare(const image& cfft, int radius);
    std::pair<int, float> compensate(
        const camera& camera, const light& light);
};

#endif
