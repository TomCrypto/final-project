#include "utils/gl_utils.h"

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace gl
{
    static std::string load_file(const std::string& path)
    {
        std::ifstream stream(path, std::ios::in);
        std::stringstream buffer;
        buffer << stream.rdbuf();
        return buffer.str();
    }

    shader& shader::operator=(const shader& other)
    {
        m_vert = other.m_vert;
        m_frag = other.m_frag;
        m_prog = other.m_prog;
        return *this;
    }

    shader::shader(const std::string& vert_name,
                   const std::string& frag_name)
    {
        const std::string &base_path = "shaders/";

        m_vert = glCreateShader(GL_VERTEX_SHADER);
        m_frag = glCreateShader(GL_FRAGMENT_SHADER);

        auto vert_src = load_file(base_path + vert_name);
        const char *vert_src_ptr = vert_src.c_str();
        glShaderSource(m_vert, 1, &vert_src_ptr, 0);

        auto frag_src = load_file(base_path + frag_name);
        const char *frag_src_ptr = frag_src.c_str();
        glShaderSource(m_frag, 1, &frag_src_ptr, 0);

        glCompileShader(m_vert);

        if (!has_compiled(m_vert)) {
            LOG(ERROR) << "Failed to compile vertex shader '" << vert_name << "':";
            LOG(TRACE) << vert_log();
            throw 0;
        }

        glCompileShader(m_frag);

        if (!has_compiled(m_frag)) {
            LOG(ERROR) << "Failed to compile fragment shader '" << frag_name << "':";
            LOG(TRACE) << frag_log();
            throw 0;
        }

        m_prog = glCreateProgram();
        glAttachShader(m_prog, m_vert);
        glAttachShader(m_prog, m_frag);
        glLinkProgram(m_prog);

        if (!has_linked(m_prog)) {
            LOG(ERROR) << "Failed to link shader program:";
            LOG(TRACE) << link_log();
            throw 0;
        }
    }

    shader::shader(const shader& other)
    {
        m_vert = other.m_vert;
        m_frag = other.m_frag;
        m_prog = other.m_prog;
    }

    shader::~shader()
    {
        glDeleteProgram(m_prog);
        glDeleteShader(m_frag);
        glDeleteShader(m_vert);
    }

    std::string shader::vert_log() const
    {
        return get_compile_log(m_vert);
    }

    std::string shader::frag_log() const
    {
        return get_compile_log(m_frag);
    }

    std::string shader::link_log() const
    {
        return get_link_log(m_prog);
    }

    void shader::bind() const
    {
        glUseProgram(m_prog);
    }

    void shader::unbind() const
    {
        glUseProgram(0);
    }

    GLint shader::operator[](const std::string& variable)
    {
        auto it = std::lower_bound(m_vars.begin(), m_vars.end(),
                                   std::make_pair(variable, 0));

        if ((it == m_vars.end()) || (it->first != variable))
        {
            GLint loc = glGetUniformLocation(m_prog, variable.c_str());
            m_vars.insert(it, std::make_pair(variable, loc));
            return loc;
        }
        else return it->second;
    }

    void shader::set(const std::string& var, const float& value) {
        glUniform1f((*this)[var], value);
    }

    void shader::set(const std::string& var, const glm::vec2& value) {
        glUniform2f((*this)[var], value.x, value.y);
    }

    void shader::set(const std::string& var, const glm::vec3& value) {
        glUniform3f((*this)[var], value.x, value.y, value.z);
    }

    void shader::set(const std::string& var, const glm::vec4& value) {
        glUniform4f((*this)[var], value.x, value.y, value.z, value.w);
    }

    void shader::set(const std::string& var, const int& value) {
        glUniform1i((*this)[var], value);
    }

    void shader::set(const std::string& var, const int& x, const int& y) {
        glUniform2i((*this)[var], x, y);
    }

    void shader::set(const std::string& var, const glm::mat3& value) {
        glUniformMatrix3fv((*this)[var], 1, GL_TRUE, glm::value_ptr(value));
    }

    void shader::set(const std::string& var, const glm::mat4& value) {
        glUniformMatrix4fv((*this)[var], 1, GL_TRUE, glm::value_ptr(value));
    }

    void shader::fullscreen_quad()
    {
        glBegin(GL_QUADS);
        glVertex2f(-1, -1);
        glVertex2f(+1, -1);
        glVertex2f(+1, +1);
        glVertex2f(-1, +1);
        glEnd();
    }

    bool shader::has_compiled(GLuint shader) const
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        return status == GL_TRUE;
    }

    bool shader::has_linked(GLuint program) const
    {
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        return status == GL_TRUE;
    }

    std::string shader::get_compile_log(GLuint shader) const
    {
        int infologLength = 0;
        std::string lg = "";

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0)
        {
            char *line = (char *)malloc(infologLength);
            int charsWritten  = 0;

            glGetShaderInfoLog(shader, infologLength, &charsWritten, line);
            lg += line;
            lg += "\n";
            free(line);
        }

        return lg;
    }

    std::string shader::get_link_log(GLuint program) const
    {
        int infologLength = 0;
        std::string lg = "";

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0)
        {
            char *line = (char *)malloc(infologLength);
            int charsWritten  = 0;

            glGetProgramInfoLog(program, infologLength, &charsWritten, line);
            lg += line;
            lg += "\n";
            free(line);
        }

        return lg;
    }

    static unsigned char clamp(float x)
    {
        if (x < 0) return 0;
        if (x > 1) return 255;
        return (int)(x * 255);
    }

    texture::texture(const std::string& path, GLenum format)
        : m_fmt(format)
    {
        image img(path);
        w = img.width();
        h = img.height();
        init_texture();

        if (m_fmt == GL_UNSIGNED_BYTE)
        {
            unsigned char* buf = (unsigned char *)malloc(w * h * 4);

            for (int y = 0; y < h; ++y)
            {
                glm::vec4* ptr = img[y];

                for (int x = 0; x < w; ++x)
                {
                    buf[y * (4 * w) + 4 * x + 0] = clamp(ptr->x);
                    buf[y * (4 * w) + 4 * x + 1] = clamp(ptr->y);
                    buf[y * (4 * w) + 4 * x + 2] = clamp(ptr->z);
                    buf[y * (4 * w) + 4 * x + 3] = 0;

                    ++ptr;
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, buf);

            free(buf);
        }
        else if (m_fmt == GL_FLOAT)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, img.data());
        } else {
            LOG(ERROR) << "Bad texture format";
            throw 0;
        }
    }

    texture::texture(int width, int height, GLenum format)
        : w(width), h(height), m_fmt(format)
    {
        if ((m_fmt != GL_UNSIGNED_BYTE) && (m_fmt != GL_FLOAT)) {
            LOG(ERROR) << "Bad texture format";
            throw 0;
        }

        init_texture();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, m_fmt, nullptr);
    }

    texture& texture::operator=(const texture& other)
    {
        m_tex = other.m_tex;
        m_fmt = other.m_fmt;
        w = other.w;
        h = other.h;

        return *this;
    }

    texture::texture(const texture& other)
    {
        m_tex = other.m_tex;
        m_fmt = other.m_fmt;
        w = other.w;
        h = other.h;
    }

    texture::~texture()
    {
        glDeleteTextures(1, &m_tex);
    }

    void texture::bind(int unit) const
    {
        glBindTexture(GL_TEXTURE_2D, m_tex);
    }

    void texture::init_texture()
    {
        glGenTextures(1, &m_tex);
        glBindTexture(GL_TEXTURE_2D, m_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}
