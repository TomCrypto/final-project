#ifndef FBUFFER_H
#define FBUFFER_H

#include <GL/glew.h>
#include <string>

#include "utils/gl_utils.h"

class fbuffer
{
public:
    //<< Initialize and free the framebuffer
    fbuffer(int width, int height);
    ~fbuffer();

    //<< Setup the framebuffer for rendering
    void bind();

    //<< Clears the framebuffer (depth optional)
    void clear(bool depth);

    //<< Resize the framebuffer
    void resize(int width, int height);

    //<< Render framebuffer into the backbuffer
    void render(float exposure);

private:
    GLuint m_tex;   //<< main render texture
    GLuint m_tmp;   //<< temporary texture for readback
    GLuint m_depth; //<< depth texture for rendering

    GLuint m_fbo;   //<< FBO for rendering

    gl::shader m_shader;
    gl::shader m_log_shader;

    int m_width;
    int m_height;
};

#endif
