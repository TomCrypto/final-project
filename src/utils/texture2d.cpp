#include <easylogging.h>

#include "utils/texture2d.h"

#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <locale>

static unsigned char saturate(float x)
{
    if (x < 0) return 0;
    if (x > 1) return 255;
    return (int)(x * 255);
}

namespace gl
{
    texture2D::texture2D()
        : m_fmt(0), m_tex(0)
    {

    }

    texture2D& texture2D::operator=(const texture2D& other)
    {
        m_fmt = other.m_fmt;
        m_tex = other.m_tex;
        m_dims = other.m_dims;

        return *this;
    }

    texture2D::texture2D(const texture2D& other)
    {
        *this = other;
    }

    texture2D::texture2D(const std::string& path, GLenum format)
        : m_fmt(format)
    {
        image img(path);
        m_dims = glm::ivec2(img.width(),
                            img.height());

		LOG(INFO) << img.width() << " " << img.height();

        glGenTextures(1, &m_tex);

        if (m_tex == 0) {
            LOG(ERROR) << "Failed to create texture object.";
            throw std::runtime_error("");
        }

        glBindTexture(GL_TEXTURE_2D, m_tex);

        if (m_fmt == GL_UNSIGNED_BYTE) {
            int w = m_dims.x, h = m_dims.y;

            auto buf = std::vector<unsigned char>();
            buf.resize(w * h * 4); // 4 bytes/pixel

            for (int y = 0; y < h; ++y) {
                const glm::vec4* ptr = img[y];

                for (int x = 0; x < w; ++x) {
                    buf[4 * (y * w + x) + 0] = saturate(ptr->x);
                    buf[4 * (y * w + x) + 1] = saturate(ptr->y);
                    buf[4 * (y * w + x) + 2] = saturate(ptr->z);
                    buf[4 * (y * w + x) + 3] = 0;

                    ++ptr;
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dims.x, m_dims.y,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, &buf[0]);
        } else if (m_fmt == GL_FLOAT) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_dims.x, m_dims.y,
                         0, GL_RGBA, GL_FLOAT, img.data());
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
        m_dims = glm::ivec2(img.width(),
                            img.height());

        glGenTextures(1, &m_tex);

        if (m_tex == 0) {
            LOG(ERROR) << "Failed to create texture object.";
            throw std::runtime_error("");
        }

        glBindTexture(GL_TEXTURE_2D, m_tex);

        if (m_fmt == GL_UNSIGNED_BYTE) {
            int w = m_dims.x, h = m_dims.y;

            auto buf = std::vector<unsigned char>();
            buf.resize(w * h * 4); // 4 bytes/pixel

            for (int y = 0; y < h; ++y) {
                const glm::vec4* ptr = img[y];

                for (int x = 0; x < w; ++x) {
                    buf[4 * (y * w + x) + 0] = 255;//saturate(ptr->x);
                    buf[4 * (y * w + x) + 1] = 0;//saturate(ptr->y);
                    buf[4 * (y * w + x) + 2] = 255;//saturate(ptr->z);
                    buf[4 * (y * w + x) + 3] = 0;

                    ++ptr;
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dims.x, m_dims.y,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, &buf[0]);
        } else if (m_fmt == GL_FLOAT) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_dims.x, m_dims.y,
                         0, GL_RGBA, GL_FLOAT, img.data());
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
        if ((m_fmt != GL_UNSIGNED_BYTE) && (m_fmt != GL_FLOAT)) {
            LOG(ERROR) << "Unsupported texture format.";
            LOG(TRACE) << "Supported formats are:";
            LOG(TRACE) << "* GL_UNSIGNED_BYTE";
            LOG(TRACE) << "* GL_FLOAT";
            throw std::logic_error("");
        }

        glGenTextures(1, &m_tex);

        if (m_tex == 0) {
            LOG(ERROR) << "Failed to create texture object.";
            throw std::runtime_error("");
        }

        glBindTexture(GL_TEXTURE_2D, m_tex);

        GLenum internal = m_fmt == GL_FLOAT ? GL_RGBA32F : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, internal, m_dims.x, m_dims.y, 0,
                     GL_RGBA, m_fmt, nullptr);
    }

    texture2D::~texture2D()
    {
        if (m_tex != 0) {
            glDeleteTextures(1, &m_tex);
        }
    }

    void texture2D::resize(const glm::ivec2& dims)
    {
        m_dims = dims;

        GLenum internal = m_fmt == GL_FLOAT ? GL_RGBA32F : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, m_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, internal, m_dims.x, m_dims.y, 0,
                     GL_RGBA, m_fmt, nullptr);
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
}
