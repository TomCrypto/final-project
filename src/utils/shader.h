/* Purpose:
 *
 *  - wraps an OpenGL shader program (fragment + vertex shader)
 *
 * This class wraps a shader created with glCreateShader and allows to bind it
 * to the OpenGL pipeline and upload uniforms/textures to it. This class keeps
 * a variable cache for performance and also warns if nonexistent uniforms are
 * uploaded to the shader (once).
 *
 * It also helpfully prints out the GLSL build log in case of build error.
*/

#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <utility>
#include <vector>
#include <string>

#include "utils/texture2d.h"

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
        void set(const std::string& var, const gl::texture2D& tex,
                    int texture_unit,
                    int min_filter = GL_LINEAR, int mag_filter = GL_LINEAR,
                    int wrap_s = GL_REPEAT, int wrap_t = GL_REPEAT);

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
}

#endif
