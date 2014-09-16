// contains various C++ wrappers of OpenGL objects
// like shaders and such

#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <GL/glew.h>
#include <string>

#include "utils/image.hpp"

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
    private:
        GLuint m_vert;
        GLuint m_frag;
        GLuint m_prog;

        bool has_compiled(GLuint shader) const;
        bool has_linked(GLuint program) const;

        std::string get_compile_log(GLuint shader) const;
        std::string get_link_log(GLuint program) const;
    };

    class texture
    {
    public:
        texture(const std::string& path, GLenum format);
        texture(int width, int height, GLenum format);
        texture& operator=(const texture& other);
        texture(const texture& other);
        ~texture();

        void bind(int unit) const;

    private:
        GLuint m_tex;
        GLenum m_fmt;
        int w;
        int h;

        void init_texture();
    };
}

#endif
