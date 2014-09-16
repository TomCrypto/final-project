#include "utils/gl_utils.h"

#include <fstream>
#include <sstream>

namespace gl
{
    static std::string load_file(const std::string& path)
    {
        auto stream = std::ifstream(path);
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

        if (!has_compiled(m_vert))
            throw std::runtime_error("Failed to compile vertex shader '"
                                   + vert_name + "':\n\n" + vert_log());

        glCompileShader(m_frag);

        if (!has_compiled(m_frag))
            throw std::runtime_error("Failed to compile fragment shader '"
                                   + vert_name + "':\n\n" + frag_log());

        m_prog = glCreateProgram();
        glAttachShader(m_prog, m_vert);
        glAttachShader(m_prog, m_frag);
        glLinkProgram(m_prog);

        if (!has_linked(m_prog))
            throw std::runtime_error("Failed to link shader program:\n\n"
                                   + link_log());
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

    bool shader::has_compiled(GLuint shader) const
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        return status == GL_TRUE;
    }

    bool shader::has_linked(GLuint program) const
    {
        GLint status;
        glGetShaderiv(program, GL_LINK_STATUS, &status);
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

            for (size_t y = 0; y < h; ++y)
            {
                glm::vec4* ptr = img[y];

                for (size_t x = 0; x < w; ++x)
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
        }
        else throw std::logic_error("Bad texture format");
    }

    texture::texture(size_t width, size_t height, GLenum format)
        : w(width), h(height), m_fmt(format)
    {
        if ((m_fmt != GL_UNSIGNED_BYTE) && (m_fmt != GL_FLOAT))
            throw std::logic_error("Bad texture format");

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