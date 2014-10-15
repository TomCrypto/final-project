#include <easylogging.h>

#include "utils/texture2d.h"

#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cctype>
#include <locale>

static unsigned char saturate(float x)
{
    if (x < 0) return 0;
    if (x > 1) return 255;
    return (int)(x * 255);
}

static GLuint alloc_texture(const glm::ivec2& dims,
                            const void *raw_ptr,
                            GLenum internal,
                            GLenum format)
{
    GLuint tex;

    glGenTextures(1, &tex);
    if (!tex) {
        LOG(ERROR) << "Failed to generate texture.";
        LOG(TRACE) << "glGenTextures failed.";
        throw std::runtime_error("");
    }

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, internal, dims.x, dims.y,
                         0, GL_RGBA, format, raw_ptr);

    return tex;
}

static std::vector<uint8_t> image_to_bytes(const image& img)
{
    auto w = img.dims().x;
    auto h = img.dims().y;

    auto buf = std::vector<unsigned char>();
    buf.resize(w * h * 4); // 4 bytes/pixel

    for (int y = 0; y < h; ++y) {
        const glm::vec4* ptr = img[y];

        for (int x = 0; x < w; ++x) {
            buf[4 * (y * w + x) + 0] = saturate(ptr->x);
            buf[4 * (y * w + x) + 1] = saturate(ptr->y);
            buf[4 * (y * w + x) + 2] = saturate(ptr->z);
            buf[4 * (y * w + x) + 3] = saturate(ptr->w);

            ++ptr;
        }
    }

    return buf;
}

namespace gl
{
    texture2D::texture2D(const std::string& path, GLenum format)
        : m_fmt(format), m_tex(0)
    {
        image img(path);
        m_dims = img.dims();

        if (m_fmt == GL_UNSIGNED_BYTE) {
            m_tex = alloc_texture(m_dims, &image_to_bytes(img)[0],
                                  GL_RGBA, GL_UNSIGNED_BYTE);
        } else if (m_fmt == GL_FLOAT) {
            m_tex = alloc_texture(m_dims, img.data(),
                                  GL_RGBA32F, GL_FLOAT);
        } else {
            LOG(ERROR) << "Unsupported texture format.";
            LOG(TRACE) << "Supported formats are:";
            LOG(TRACE) << "* GL_UNSIGNED_BYTE";
            LOG(TRACE) << "* GL_FLOAT";
            throw std::logic_error("");
        }
    }

    texture2D::texture2D(const image& img, GLenum format)
        : m_fmt(format)
    {
        m_dims = img.dims();

        if (m_fmt == GL_UNSIGNED_BYTE) {
            m_tex = alloc_texture(m_dims, &image_to_bytes(img)[0],
                                  GL_RGBA, GL_UNSIGNED_BYTE);
        } else if (m_fmt == GL_FLOAT) {
            m_tex = alloc_texture(m_dims, img.data(),
                                  GL_RGBA32F, GL_FLOAT);
        } else {
            LOG(ERROR) << "Unsupported texture format.";
            LOG(TRACE) << "Supported formats are:";
            LOG(TRACE) << "* GL_UNSIGNED_BYTE";
            LOG(TRACE) << "* GL_FLOAT";
            throw std::logic_error("");
        }
    }

    texture2D::texture2D(const glm::ivec2& dims, GLenum format)
        : m_dims(dims), m_fmt(format)
    {
        if (m_fmt == GL_UNSIGNED_BYTE) {
            m_tex = alloc_texture(m_dims, nullptr,
                                  GL_RGBA, GL_UNSIGNED_BYTE);
        } else if (m_fmt == GL_FLOAT) {
            m_tex = alloc_texture(m_dims, nullptr,
                                  GL_RGBA32F, GL_FLOAT);
        } else {
            LOG(ERROR) << "Unsupported texture format.";
            LOG(TRACE) << "Supported formats are:";
            LOG(TRACE) << "* GL_UNSIGNED_BYTE";
            LOG(TRACE) << "* GL_FLOAT";
            throw std::logic_error("");
        }
    }

    texture2D::~texture2D()
    {
        if (m_tex) {
            glDeleteTextures(1, &m_tex);
        }
    }

    void texture2D::resize(const glm::ivec2& dims)
    {
        m_dims = dims;

        if (m_fmt == GL_UNSIGNED_BYTE) {
            m_tex = alloc_texture(m_dims, nullptr,
                                  GL_RGBA, GL_UNSIGNED_BYTE);
        } else if (m_fmt == GL_FLOAT) {
            m_tex = alloc_texture(m_dims, nullptr,
                                  GL_RGBA32F, GL_FLOAT);
        }
    }

    void texture2D::bind(int unit, int min_filter,
                                   int mag_filter,
                                   int wrap_s,
                                   int wrap_t) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    }

    GLuint texture2D::operator()() const
    {
        return m_tex;
    }

    glm::ivec2 texture2D::dims() const
    {
        return m_dims;
    }
}
