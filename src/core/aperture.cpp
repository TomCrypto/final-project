#include <easylogging.h>

#include "core/aperture.h"

#include <cmath>

static image get_spectrum_image();

aperture::aperture(fft_engine& fft) : m_fft(fft),
    m_flare_shader("aperture.vert", "aperture.frag"),
    m_ghost_shader("ghost.vert", "ghost.frag"),
    m_spectrum(get_spectrum_image(), GL_FLOAT)
{

}

// Five different disk radii for every light type
static const int radii[] = { 2, 4, 10 /*, 28, 75 */ };

// This seems to be the sweet spot of frequency
// sampling resolution versus spectral leakage
static const int quality = 1024;

static image trim(const image& img)
{
    int max_dist = 0;

    for (int y = 0; y < img.dims().y; ++y) {
        const glm::vec4* ptr = img[y];

        for (int x = 0; x < img.dims().x; ++x) {
            int dist = std::max(std::abs(x - img.dims().x / 2),
                                std::abs(y - img.dims().y / 2));

            if ((ptr->x > 5 * 1e-7f)
             || (ptr->y > 5 * 1e-7f)
             || (ptr->z > 5 * 1e-7f)) {
                max_dist = std::max(max_dist, dist);
            }

            ++ptr;
        }
    }

    if (max_dist < 20)
        return img;

    return img.subregion(img.dims().x / 2 - max_dist,
                         img.dims().y / 2 - max_dist,
                         2 * max_dist, 2 * max_dist);
}

void aperture::load_aperture(const transmission_function& tf,
                             float f_number)
{
    m_flares.clear();
    std::string path;

    switch (tf)
    {
        case PENTAGON:
            path = "pentagon.png";
            break;
        case FINGERPRINTS:
            path = "fingerprints.png";
            break;
        case GRAZED:
            path = "grazed.png";
            break;
        case GRAZED_CUT:
            path = "grazed_cut.png";
            break;
        case HUMAN_EYE:
            path = "human_eye.png";
            break;
        case CUSTOM:
            path = "custom.png";
            break;
        default:
            LOG(ERROR) << "Bad aperture passed!";
            throw std::runtime_error("");
    }

    LOG(INFO) << "Now loading aperture '" << path << "'.";

    auto aperture = image("apertures/" + path)
                  .resize(glm::ivec2(quality));

    auto blades = utils::draw_circle((int)((quality / 2) / f_number), false);
    blades = blades.enlarge(glm::ivec2(quality));
    aperture.mul(blades);

    LOG(TRACE) << "Computing point spread function.";

    auto point_spread_function = m_fft.psf(aperture, aperture.dims());
    point_spread_function.reproduce(channels::R);
    point_spread_function.normalize(false);

    LOG(TRACE) << "Computing chromatic Fourier transform.";

    auto chromatic_transform = get_cfft(point_spread_function);
    chromatic_transform.normalize(false);

    LOG(TRACE) << "Convolving chromatic Fourier transform.";

    for (auto r : radii) {
        auto convolved = get_flare(chromatic_transform, r);
        m_flares[r] = std::unique_ptr<gl::texture2D>(
            new gl::texture2D(convolved, GL_FLOAT)
        );
    }

    LOG(INFO) << "Aperture successfully loaded.";

    // Try and compute a unique ID number for the
    // aperture + scale combination we processed
    m_flare_hash = (int)tf + (int)(1000 * f_number);

    m_f_number = f_number;
}

static glm::vec3 curve[80] = {
    glm::vec3(0.0014,0.0000,0.0065), glm::vec3(0.0022,0.0001,0.0105),
    glm::vec3(0.0042,0.0001,0.0201), glm::vec3(0.0076,0.0002,0.0362),
    glm::vec3(0.0143,0.0004,0.0679), glm::vec3(0.0232,0.0006,0.1102),
    glm::vec3(0.0435,0.0012,0.2074), glm::vec3(0.0776,0.0022,0.3713),
    glm::vec3(0.1344,0.0040,0.6456), glm::vec3(0.2148,0.0073,1.0391),
    glm::vec3(0.2839,0.0116,1.3856), glm::vec3(0.3285,0.0168,1.6230),
    glm::vec3(0.3483,0.0230,1.7471), glm::vec3(0.3481,0.0298,1.7826),
    glm::vec3(0.3362,0.0380,1.7721), glm::vec3(0.3187,0.0480,1.7441),
    glm::vec3(0.2908,0.0600,1.6692), glm::vec3(0.2511,0.0739,1.5281),
    glm::vec3(0.1954,0.0910,1.2876), glm::vec3(0.1421,0.1126,1.0419),
    glm::vec3(0.0956,0.1390,0.8130), glm::vec3(0.0580,0.1693,0.6162),
    glm::vec3(0.0320,0.2080,0.4652), glm::vec3(0.0147,0.2586,0.3533),
    glm::vec3(0.0049,0.3230,0.2720), glm::vec3(0.0024,0.4073,0.2123),
    glm::vec3(0.0093,0.5030,0.1582), glm::vec3(0.0291,0.6082,0.1117),
    glm::vec3(0.0633,0.7100,0.0782), glm::vec3(0.1096,0.7932,0.0573),
    glm::vec3(0.1655,0.8620,0.0422), glm::vec3(0.2257,0.9149,0.0298),
    glm::vec3(0.2904,0.9540,0.0203), glm::vec3(0.3597,0.9803,0.0134),
    glm::vec3(0.4334,0.9950,0.0087), glm::vec3(0.5121,1.0000,0.0057),
    glm::vec3(0.5945,0.9950,0.0039), glm::vec3(0.6784,0.9786,0.0027),
    glm::vec3(0.7621,0.9520,0.0021), glm::vec3(0.8425,0.9154,0.0018),
    glm::vec3(0.9163,0.8700,0.0017), glm::vec3(0.9786,0.8163,0.0014),
    glm::vec3(1.0263,0.7570,0.0011), glm::vec3(1.0567,0.6949,0.0010),
    glm::vec3(1.0622,0.6310,0.0008), glm::vec3(1.0456,0.5668,0.0006),
    glm::vec3(1.0026,0.5030,0.0003), glm::vec3(0.9384,0.4412,0.0002),
    glm::vec3(0.8544,0.3810,0.0002), glm::vec3(0.7514,0.3210,0.0001),
    glm::vec3(0.6424,0.2650,0.0000), glm::vec3(0.5419,0.2170,0.0000),
    glm::vec3(0.4479,0.1750,0.0000), glm::vec3(0.3608,0.1382,0.0000),
    glm::vec3(0.2835,0.1070,0.0000), glm::vec3(0.2187,0.0816,0.0000),
    glm::vec3(0.1649,0.0610,0.0000), glm::vec3(0.1212,0.0446,0.0000),
    glm::vec3(0.0874,0.0320,0.0000), glm::vec3(0.0636,0.0232,0.0000),
    glm::vec3(0.0468,0.0170,0.0000), glm::vec3(0.0329,0.0119,0.0000),
    glm::vec3(0.0227,0.0082,0.0000), glm::vec3(0.0158,0.0057,0.0000),
    glm::vec3(0.0114,0.0041,0.0000), glm::vec3(0.0081,0.0029,0.0000),
    glm::vec3(0.0058,0.0021,0.0000), glm::vec3(0.0041,0.0015,0.0000),
    glm::vec3(0.0029,0.0010,0.0000), glm::vec3(0.0020,0.0007,0.0000),
    glm::vec3(0.0014,0.0005,0.0000), glm::vec3(0.0010,0.0004,0.0000),
    glm::vec3(0.0007,0.0002,0.0000), glm::vec3(0.0005,0.0002,0.0000),
    glm::vec3(0.0003,0.0001,0.0000), glm::vec3(0.0002,0.0001,0.0000),
    glm::vec3(0.0002,0.0001,0.0000), glm::vec3(0.0001,0.0000,0.0000),
    glm::vec3(0.0001,0.0000,0.0000), glm::vec3(0.0001,0.0000,0.0000)
};

/* Converts a visible wavelength to an RGB color. */
static glm::vec3 wavelength_rgb(float lambda)
{
    if (lambda <= 380)
        return curve[0];

    if (lambda >= 775)
        return curve[79];

    int pos = (int)((lambda - 380) / 5);
    float t = (lambda - 380) / 5 - pos;

    auto xyz = glm::mix(curve[pos], curve[pos + 1], t);

    glm::mat3 xyz_to_rgb(+2.3706743, -0.9000405, -0.4706338,
                         -0.5138850, +1.4253036, +0.0885814,
                         +0.0052982, -0.0146949, +1.0093968);

    glm::vec3 rgb = xyz_to_rgb * xyz;

    return glm::vec3(glm::clamp(rgb.x, 0.0f, 1.0f),
                     glm::clamp(rgb.y, 0.0f, 1.0f),
                     glm::clamp(rgb.z, 0.0f, 1.0f));
}

image get_spectrum_image()
{
    image img(glm::ivec2(80, 1));

    for (size_t t = 0; t < 80; ++t) {
        img[0][t] = glm::vec4(wavelength_rgb(380 + 5 * (float)t), 1.0f);
    }

    return img;
}

image aperture::get_cfft(const image& psf)
{
    image out(psf.dims());

    const int samples = 40; // 10nm step (300nm to 700nm over 40 samples)

    for (int t = 0; t < samples; ++t)
    {
        float lambda = 700 - 300 * (float)t / samples;
        float scale = lambda / 700;

        int newX = (int)(out.width() * scale);
        int newY = (int)(out.height() * scale);

        auto ps_resized = psf.resize(glm::ivec2(newX, newY),
                                     FILTER_BILINEAR);
        auto color = wavelength_rgb(lambda);
        ps_resized.colorize(color);
        ps_resized = ps_resized.enlarge(psf.dims());

        out.add(ps_resized);
    }

    return out;
}

image aperture::get_flare(const image& cfft, int radius)
{
    return trim(m_fft.convolve_disk(cfft, radius));
}

/* What this function does is select the proper lens flare to pick according
 * to the type of the light (e.g. LIGHT_SMALL = small convolved disk)
 * and then calculates by how much to scale that texture so that the disk
 * is drawn exactly on top of the actual light sphere on screen.
*/
std::pair<int, float> aperture::compensate(
    const camera& camera, const light& light)
{
    int selected_radius = radii[(int)light.type];

    // compute best flare texture + compensation factor

    float max_radius = 0.0;
    const int samples = 16;

    glm::vec4 ref = camera.proj() * camera.view() * light.position;
    ref /= ref.w;

    for (int y = 0; y < samples; ++y) {
        float theta = y / float(samples) * glm::pi<float>();

        for (int x = 0; x < samples; ++x) {
            float phi = x / float(samples) * glm::pi<float>() * 2;

            glm::vec4 world_pos = light.position + glm::vec4(
                glm::sin(theta) * glm::cos(phi),
                glm::cos(theta),
                glm::sin(theta) * glm::sin(phi),
                0.0) * (light.radius * 0.99f);

            glm::vec4 projected = camera.proj() * camera.view() * world_pos;
            projected /= projected.w;

            float distance = glm::length((glm::vec2)projected -
                                         (glm::vec2)ref) / 2.0f;

            max_radius += distance / (samples * samples);
        }
    }

    // now that we've settled on a radius, check how large we need it to be
    return std::make_pair(
        selected_radius,
        (2.0f * selected_radius / max_radius / (float)m_flares[selected_radius].get()->dims().x)
    );
}

void aperture::render_flare(const std::vector<light>& lights,
                            const gl::texture2D& occlusion,
                            const camera& camera,
                            float intensity)
{
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glViewport(0, 0, camera.dims().x, camera.dims().y);

    m_flare_shader.bind();

    int last_flare = -1;

    m_flare_shader.set("occlusion", occlusion, 1, GL_NEAREST, GL_NEAREST);
    m_flare_shader.set("max_lights", 8);
    m_flare_shader.set("intensity", intensity);
    m_flare_shader.set("f_number", m_f_number);

    for (size_t t = 0; t < lights.size(); ++t) {
        auto comp = compensate(camera, lights[t]);
        float s = comp.second; // size compensation

        if (comp.first != last_flare) {
            m_flare_shader.set("flare", *m_flares[comp.first].get(), 0,
                GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
            last_flare = comp.first;
        }

        const float w0 = 2.0f;

        auto cam_to_light = (glm::vec3)lights[t].position
                          - camera.pos() * lights[t].position.w;

        // Project light on sensor
        bool forward_facing = glm::dot(
            glm::normalize(cam_to_light),
            glm::normalize(camera.dir())) > 0;

        glm::vec4 projected = camera.proj() * camera.view() * lights[t].position;
        projected /= projected.w;

        float aspect = camera.aspect_ratio();

        if (forward_facing) {
            glBegin(GL_QUADS);
            glTexCoord3f(0.5f - 0.5f * s, 0.5f - 0.5f * s, (float)t);
            glVertex2f(-w0 + projected.x, -w0 * aspect + projected.y);
            glTexCoord3f(0.5f + 0.5f * s, 0.5f - 0.5f * s, (float)t);
            glVertex2f(+w0 + projected.x, -w0 * aspect + projected.y);
            glTexCoord3f(0.5f + 0.5f * s, 0.5f + 0.5f * s, (float)t);
            glVertex2f(+w0 + projected.x, +w0 * aspect + projected.y);
            glTexCoord3f(0.5f - 0.5f * s, 0.5f + 0.5f * s, (float)t);
            glVertex2f(-w0 + projected.x, +w0 * aspect + projected.y);
            glEnd();
        }
    }

    m_flare_shader.unbind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

static float uniform()
{
    return (float)rand() / RAND_MAX;
}

void aperture::render_ghosts(const std::vector<light>& lights,
                             const gl::texture2D& occlusion,
                             const camera& camera,
                             float intensity,
                             int ghost_count,
                             float ghost_size,
                             float ghost_brightness)
{
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glViewport(0, 0, camera.dims().x, camera.dims().y);

    m_ghost_shader.bind();

    m_ghost_shader.set("occlusion", occlusion, 0, GL_NEAREST, GL_NEAREST);
    m_ghost_shader.set("spectrum", m_spectrum, 1);
    m_ghost_shader.set("max_lights", 8);
    m_ghost_shader.set("intensity", intensity);
    m_ghost_shader.set("f_number", m_f_number);
    m_ghost_shader.set("ghost_brightness", ghost_brightness);

    glBegin(GL_QUADS);

    for (size_t t = 0; t < lights.size(); ++t) {
        if (!lights[t].ghosts)
            continue;

        auto cam_to_light = (glm::vec3)lights[t].position
                          - camera.pos() * lights[t].position.w;

        bool forward_facing = glm::dot(
            glm::normalize(cam_to_light),
            glm::normalize(camera.dir())) > 0;

        glm::vec4 projected = camera.proj() * camera.view() * lights[t].position;
        projected /= projected.w;

        float aspect = camera.aspect_ratio();

        if (forward_facing) {
            for (int i = 0; i < ghost_count; ++i) {
                srand(101 * i + m_flare_hash);

                float p = exp(1 / std::pow(uniform(), 0.35f)) - exp(1.0f);
                float sz = ghost_size * 3 * (0.2f + 0.8f * pow(uniform(), 5.0f));

                float aspect = camera.aspect_ratio();

                auto pos = glm::mix((glm::vec2)projected, glm::vec2(0), p);
                if (p < 1) sz *= glm::sqrt(p); // ghosts smaller near flare

                glTexCoord4f(0.0f, 0.0f, (float)t, (float)(101 * i + m_flare_hash));
                glVertex2f(-sz + pos.x, -sz * aspect + pos.y);
				glTexCoord4f(1.0f, 0.0f, (float)t, (float)(101 * i + m_flare_hash));
                glVertex2f(+sz + pos.x, -sz * aspect + pos.y);
				glTexCoord4f(1.0f, 1.0f, (float)t, (float)(101 * i + m_flare_hash));
                glVertex2f(+sz + pos.x, +sz * aspect + pos.y);
				glTexCoord4f(0.0f, 1.0f, (float)t, (float)(101 * i + m_flare_hash));
                glVertex2f(-sz + pos.x, +sz * aspect + pos.y);
            }
        }
    }

    glEnd();

    m_ghost_shader.unbind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
