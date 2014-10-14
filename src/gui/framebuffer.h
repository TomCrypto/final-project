/* Purpose:
 *
 *  - manages an OpenGL floating-point framebuffer
 *  - has high dynamic range capabilities using GL_FLOAT
 *  - also allows temporarily rendering to other textures
 *  - automatically handles tonemapping into the backbuffer
 *
 * This framebuffer is designed such that the rest of the program doesn't even
 * know that they are rendering into an HDR texture - it behaves as a standard
 * backbuffer.
 *
 * The only class which makes use of more advanced features of the framebuffer
 * is the occlusion class, which needs a copy of the current frame data, using
 * frame_copy(), and also needs to render into its own texture with bind_as().
*/

#ifndef GUI_FRAMEBUFFER_H
#define GUI_FRAMEBUFFER_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>

#include "utils/shader.h"
#include "utils/texture2d.h"

class framebuffer
{
public:
    //<< Initialize and free the framebuffer
    framebuffer(const glm::ivec2& dims);
    ~framebuffer();

    //<< Setup the framebuffer for rendering
    void bind();

    //<< Bind some other texture to render into
    void bind_as(const gl::texture2D& other);

    //<< Get a copy of the current framebuffer
    const gl::texture2D& frame_copy();

    //<< Clears the framebuffer (depth optional)
    void clear(bool depth);

    //<< Resize the framebuffer
    void resize(const glm::ivec2& dims);

    //<< Render framebuffer into the backbuffer
    void render(float exposure);

private:
    framebuffer& operator=(const framebuffer& other);
    framebuffer(const framebuffer& other);

    gl::texture2D m_tex; //<< main render texture
    gl::texture2D m_cpy; //<< render texture copy
    GLuint m_depth; //<< depth texture for rendering

    GLuint m_fbo;   //<< FBO for rendering

    gl::shader m_shader;
    gl::shader m_cpy_shader;

    glm::ivec2 m_dims;
};

#endif
