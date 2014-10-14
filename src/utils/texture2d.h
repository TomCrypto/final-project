#ifndef UTILS_TEXTURE2D_H
#define UTILS_TEXTURE2D_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <utility>
#include <vector>
#include <string>

#include "utils/image.h"

namespace gl
{
    // Simple texture2D class, mainly designed to store actual texture
    // data and/or framebuffer data (i.e. it is not designed to hold
    // depth data or special textures like cubemaps)
    // It can support both 32-bit (8-bit per channel) or 128-bit float
    // RGBA data.
    class texture2D
    {
    public:
        texture2D(const std::string& path, GLenum format);
        texture2D(const image& img, GLenum format);
        texture2D(const glm::ivec2& dims, GLenum format);
        texture2D& operator=(const texture2D& other);
        texture2D(const texture2D& other);
        texture2D();
        ~texture2D();

        // Resizes this texture (after this operation
        // the contents of the texture are indeterminate)
        void resize(const glm::ivec2& dims);

        // Binds this texture to a texture unit
        void bind(int unit, int min_filter = GL_LINEAR,
                            int mag_filter = GL_LINEAR,
                            int wrap_s = GL_REPEAT,
                            int wrap_t = GL_REPEAT) const;

        // Returns the texture ID of the texture
        GLuint operator()() const;

    private:
        glm::ivec2 m_dims;
        GLuint m_tex;
        GLenum m_fmt;
    };
}

#endif
