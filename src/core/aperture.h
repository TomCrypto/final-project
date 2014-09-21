#ifndef CORE_APERTURE_H
#define CORE_APERTURE_H

#include <glm/glm.hpp>
#include <fftw3.h>
#include <vector>
#include <random>

#include "utils/fft_engine.h"
#include "utils/image.hpp"

// allows one to generate random apertures of a given size as well as their
// chromatic Fourier transform

class aperture
{
public:
    aperture();

    // generates a random aperture with noise
    // TODO: make noise configurable as parameters?
    image gen_aperture(const glm::ivec2& dims);

    // gets the chromatic Fourier transform of an aperture
    // the final output is resized to the given dimensions
    image get_cfft(const image& aperture, const glm::ivec2& dims);

    image get_flare(const image& cfft, float radius);

private:
    aperture& operator=(const aperture& other);
    aperture(const aperture& other);

    std::vector<image> m_apertures;
    std::vector<image> m_noise;

    std::random_device m_rd;
    std::mt19937 m_rng;
    fft_engine m_fft;
};

#endif
