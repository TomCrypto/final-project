#include <easylogging.h>

#include "utils/shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <locale>

// These string-trimming functions from:
//      http://stackoverflow.com/questions/216823

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
                s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

static std::string load_file(const std::string& path)
{
    std::ifstream stream(path, std::ios::in);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

namespace gl
{
    shader& shader::operator=(const shader& other)
    {
        glDeleteProgram(m_prog);
        glDeleteShader(m_frag);
        glDeleteShader(m_vert);

        m_missing_vars = other.m_missing_vars;
        m_vert_name = other.m_vert_name;
        m_frag_name = other.m_frag_name;
        m_vars = other.m_vars;
        m_vert = other.m_vert;
        m_frag = other.m_frag;
        m_prog = other.m_prog;
        return *this;
    }

    shader::shader(const shader& other)
    {
        *this = other;
    }

    shader::shader(const std::string& vert_name,
                    const std::string& frag_name)
        : m_vert_name(vert_name), m_frag_name(frag_name)
    {
        const std::string &base_path = "shaders/";

        m_vert = glCreateShader(GL_VERTEX_SHADER);
        m_frag = glCreateShader(GL_FRAGMENT_SHADER);

        if (!m_vert) {
            LOG(ERROR) << "Failed to create vertex shader.";
            LOG(TRACE) << "glCreateShader failed.";
            throw std::runtime_error("");
        }

        if (!m_frag) {
            LOG(ERROR) << "Failed to create fragment shader.";
            LOG(TRACE) << "glCreateShader failed.";
            throw std::runtime_error("");
        }

        auto vert_src = load_file(base_path + vert_name);
        const char *vert_src_ptr = vert_src.c_str();
        glShaderSource(m_vert, 1, &vert_src_ptr, 0);

        auto frag_src = load_file(base_path + frag_name);
        const char *frag_src_ptr = frag_src.c_str();
        glShaderSource(m_frag, 1, &frag_src_ptr, 0);

        glCompileShader(m_vert);

        if (!has_compiled(m_vert)) {
            LOG(ERROR) << "Failed to compile vertex shader '"
                        << vert_name << "':";
            LOG(TRACE) << vert_log();
            throw std::runtime_error("");
        }

        glCompileShader(m_frag);

        if (!has_compiled(m_frag)) {
            LOG(ERROR) << "Failed to compile fragment shader '"
                        << frag_name << "':";
            LOG(TRACE) << frag_log();
            throw std::runtime_error("");
        }

        m_prog = glCreateProgram();

        if (!m_prog) {
            LOG(ERROR) << "Failed to create shader program.";
            LOG(TRACE) << "glCreateProgram failed.";
            throw std::runtime_error("");
        }

        glAttachShader(m_prog, m_vert);
        glAttachShader(m_prog, m_frag);
        glLinkProgram(m_prog);

        if (!has_linked(m_prog)) {
            LOG(ERROR) << "Failed to link shader program <"
                        << vert_name << ", " << frag_name
                        << ">:";
            LOG(TRACE) << link_log();
            throw std::runtime_error("");
        }
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

    bool shader::is_missing(const std::string& var) const
    {
        auto it = std::lower_bound(m_missing_vars.begin(),
                                    m_missing_vars.end(),
                                    var);

        return (it != m_missing_vars.end()) && (*it == var);
    }

    void shader::mark_missing(const std::string& var)
    {
        auto it = std::lower_bound(m_missing_vars.begin(),
                                    m_missing_vars.end(),
                                    var);

        m_missing_vars.insert(it, var);
    }

    GLint shader::operator[](const std::string& variable)
    {
        if (is_missing(variable))
            return -1;

        auto it = std::lower_bound(m_vars.begin(), m_vars.end(),
                                    std::make_pair(variable, 0));

        if ((it == m_vars.end()) || (it->first != variable))
        {
            GLint loc = glGetUniformLocation(m_prog, variable.c_str());
            if (loc == -1) {
                LOG(WARNING) << "Uniform '" << variable << "' not found in <"
                                << m_vert_name << ", " << m_frag_name << ">.";
                mark_missing(variable); /* Will not be reported again. */
            } else m_vars.insert(it, std::make_pair(variable, loc));
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

    void shader::set(const std::string& var, const glm::ivec2& value) {
        glUniform2i((*this)[var], value.x, value.y);
    }

    void shader::set(const std::string& var, const glm::ivec3& value) {
        glUniform3i((*this)[var], value.x, value.y, value.z);
    }

    void shader::set(const std::string& var, const glm::ivec4& value) {
        glUniform4i((*this)[var], value.x, value.y, value.z, value.w);
    }

    void shader::set(const std::string& var, const glm::mat3& value) {
        glUniformMatrix3fv((*this)[var], 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader::set(const std::string& var, const glm::mat4& value) {
        glUniformMatrix4fv((*this)[var], 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader::set(const std::string& var, const gl::texture2D& tex,
                        int texture_unit,
                        int min_filter, int mag_filter,
                        int wrap_s, int wrap_t) {
        tex.bind(texture_unit, min_filter, mag_filter, wrap_s, wrap_t);
        set(var, texture_unit);
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
        std::string lg = "";
        int log_length;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 0)
        {
            char buffer[65536];
            int tmp  = 0;

            glGetShaderInfoLog(shader, log_length, &tmp, buffer);
            lg += buffer;
            lg += "\n";
        }

        return trim(lg);
    }

    std::string shader::get_link_log(GLuint program) const
    {
        std::string lg = "";
        int log_length;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 0)
        {
            char buffer[65536];
            int tmp  = 0;

            glGetProgramInfoLog(program, log_length, &tmp, buffer);
            lg += buffer;
            lg += "\n";
        }

        return trim(lg);
    }
}
