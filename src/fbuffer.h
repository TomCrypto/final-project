#ifndef FBUFFER_H
#define FBUFFER_H

#include <GL/glew.h>
#include <string>

class fbuffer
{
public:
    // todo
    fbuffer(size_t width, size_t height);

private:
    // todo
    GLuint m_tex, m_depth;
    GLuint m_fbo;
};

#endif