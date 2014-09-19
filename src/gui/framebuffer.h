#ifndef GUI_FRAMEBUFFER_H
#define GUI_FRAMEBUFFER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

#include "utils/gl_utils.h"

// manages an HDR framebuffer with tonemapping capabilities

class framebuffer
{
public:
    //<< Initialize and free the framebuffer
    framebuffer(const glm::ivec2& dims);
    ~framebuffer();

    //<< Setup the framebuffer for rendering
    void bind();

    //<< Clears the framebuffer (depth optional)
    void clear(bool depth);

    //<< Resize the framebuffer
    void resize(const glm::ivec2& dims);

    //<< Render framebuffer into the backbuffer
    void render(float exposure);

private:
    framebuffer& operator=(const framebuffer& other);
    framebuffer(const framebuffer& other);

    GLuint m_tex;   //<< main render texture
    GLuint m_tmp;   //<< temporary texture for readback
    GLuint m_depth; //<< depth texture for rendering

    GLuint m_fbo;   //<< FBO for rendering

    gl::shader m_shader;
    gl::shader m_log_shader;

    glm::ivec2 m_dims;
};

#endif
