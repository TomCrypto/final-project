#include "gui/framebuffer.h"

#include <cstdio>
#include <cmath>

#include <glm/glm.hpp>

#include "utils/image.hpp"

fbuffer::fbuffer(size_t width, size_t height)
    : m_width(width), m_height(height),
      m_shader("default.vert", "default.frag"),
      m_log_shader("tonemap/log_lum.vert", "tonemap/log_lum.frag")
{
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glGenTextures(1, &m_tmp);
    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

   //Here, we'll use :
   //glGenerateMipmapEXT(GL_TEXTURE_2D);
   //-------------------------
   glGenFramebuffersEXT(1, &m_fbo);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
   //Attach 2D texture to this FBO
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_tex, 0);
   //-------------------------
   glGenRenderbuffersEXT(1, &m_depth);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
   //-------------------------
   //Attach depth buffer to FBO
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depth);
   //-------------------------
   //Does the GPU support current FBO configuration?
   GLenum status;
   status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   switch(status)
   {
      case GL_FRAMEBUFFER_COMPLETE_EXT:
      printf("good\n");
      break;
   default:
       printf("ERROR\n");
   }

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void fbuffer::resize(size_t width, size_t height)
{
    m_width = width;
    m_height = height;

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height);
}

void fbuffer::bind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_tex, 0);
}

void fbuffer::clear(bool depth)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

static int get_mip_level(int w, int h)
{
    int m = (w < h) ? h : w;

    return (int)ceil(log(m) / log(2));
}

void fbuffer::render(float exposure)
{
    int mip_level = get_mip_level(m_width, m_height);

    glDisable(GL_DEPTH_TEST);

    // HERE, tmp contains garbage, tex contains the rendered image
    // We want to have tmp contain the color + log luminance info
    // thus, attach tmp to our framebuffer, and attach tex as texture

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_tmp, 0);

    //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

    m_log_shader.bind();

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBegin(GL_QUADS);

    glVertex3f(-1, -1, 0);
    glVertex3f(1, -1, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(-1, 1, 0);

    glEnd();

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);

    // And mipmap m_tmp which now contains the log-average luminance information

    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glGenerateMipmapEXT(GL_TEXTURE_2D);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    m_shader.bind();

    //glBindTexture(GL_TEXTURE_2D, m_tmp);

    // At this point, tmp contains what we wanted, so unbind it from the framebuffer
    // and attach tex to the BACKBUFFER, attach tmp as a texture, and run the main shader

    //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_tex, 0);

    m_shader.set("mip_level", (float)mip_level);
    m_shader.set("exposure", exposure);
    m_shader.set("pixel_count", (float)(m_width * m_height));

    glBegin(GL_QUADS);

    glVertex3f(-1, -1, 0);
    glVertex3f(1, -1, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(-1, 1, 0);

    glEnd();

    m_shader.unbind();

    return;

    //=====

    // later on:

    // 1. copy current framebuffer contents (m_tex) into (m_tmp)

    // 2. unbind current framebuffer, restoring backbuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // 3. tonemap into backbuffer using m_tex and m_tmp
}

fbuffer::~fbuffer()
{
    glDeleteTextures(1, &m_tex);
    glDeleteTextures(1, &m_tmp);
    glDeleteRenderbuffersEXT(1, &m_depth);
    //Bind 0, which means render to back buffer, as a result, fb is unbound  // IMPORTANT IMPORTANT <<<
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &m_fbo);
}
