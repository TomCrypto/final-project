// contains various C++ wrappers of OpenGL objects
// like shaders and such

#ifndef UTILS_GL_UTILS_H
#define UTILS_GL_UTILS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <utility>
#include <vector>
#include <string>

#include "utils/image.h"

namespace gl
{
    //<< Shader class incorporating a vertex + fragment shader
    class shader
    {
    public:
        //<< Names are relative to res/shaders
        shader& operator=(const shader& other);
        shader(const std::string& vert_name,
               const std::string& frag_name);
        shader(const shader& other);
        ~shader();

        //<< Bind or unbind a shader program
        void bind() const;
        void unbind() const;

        //<< Gets the different build logs
        std::string vert_log() const;
        std::string frag_log() const;
        std::string link_log() const;

        //<< Access and modify shader variables
        GLint operator[](const std::string& variable);

        void set(const std::string& var, const float& value);
        void set(const std::string& var, const glm::vec2& value);
        void set(const std::string& var, const glm::vec3& value);
        void set(const std::string& var, const glm::vec4& value);
        void set(const std::string& var, const int& value);
        void set(const std::string& var, const glm::ivec2& value);
        void set(const std::string& var, const glm::ivec3& value);
        void set(const std::string& var, const glm::ivec4& value);
        void set(const std::string& var, const glm::mat3& value);
        void set(const std::string& var, const glm::mat4& value);

        //<< Run this shader on a fullscreen quad (bind first)
        void fullscreen_quad();

    private:
        std::string m_vert_name;
        std::string m_frag_name;
        GLuint m_vert;
        GLuint m_frag;
        GLuint m_prog;

        std::vector<std::pair<std::string, GLint>> m_vars;
        bool is_missing(const std::string& var) const;
        void mark_missing(const std::string& var);
        std::vector<std::string> m_missing_vars;

        bool has_compiled(GLuint shader) const;
        bool has_linked(GLuint program) const;

        std::string get_compile_log(GLuint shader) const;
        std::string get_link_log(GLuint program) const;
    };

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
        texture2D& operator=(const texture2D& other);
        texture2D(const texture2D& other);

        glm::ivec2 m_dims;
        GLuint m_tex;
        GLenum m_fmt;
    };
}

#endif
