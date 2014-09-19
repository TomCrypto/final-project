#include <easylogging.h>

#include "gui/framebuffer.h"

#include <glm/glm.hpp>
#include <cstdio>
#include <cmath>

#include "utils/image.hpp"

framebuffer::framebuffer(const glm::ivec2& dims)
    : m_dims(dims),
      m_shader("generic/fs_quad.vert", "tonemap/reinhard.frag"),
      m_log_shader("generic/fs_quad.vert", "tonemap/log_lum.frag")
{
    glGenTextures(1, &m_tex);
    glGenTextures(1, &m_tmp);
    glGenFramebuffersEXT(1, &m_fbo);
    glGenRenderbuffersEXT(1, &m_depth);

    resize(m_dims);

    // Attach depth buffer to FBO
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, m_depth);

    //Does the GPU support current FBO configuration?
    
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            LOG(INFO) << "Framebuffer successfully initialized.";
            LOG(TRACE) << "Resolution " << dims.x <<
                          " by " << dims.y << " pixels.";
            break;
        default:
            LOG(ERROR) << "Failed to create the framebuffer!";
            throw std::runtime_error("");
    }

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void framebuffer::resize(const glm::ivec2& dims)
{
    m_dims = dims;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dims.x, m_dims.y, 0,
                 GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dims.x, m_dims.y, 0,
                 GL_RGBA, GL_FLOAT, nullptr);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                             m_dims.x, m_dims.y);
}

void framebuffer::bind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, m_tex, 0);
}

void framebuffer::clear(bool depth)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

static int get_mip_level(int w, int h)
{
    int m = (w < h) ? h : w;

    return (int)ceil(log(m) / log(2));
}

void framebuffer::render(float exposure)
{
    // Get the 1x1 mip level for the current framebuffer resolutions, this
    // will be equal to the log2 of the largest dimension (width or height)

    int mip_level = get_mip_level(m_dims.x, m_dims.y);

    // Set up the necessary OpenGL state for doing fullscreen quad rendering

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, m_dims.x, m_dims.y);

    // First convert the rendered image (in m_tex) to a log-luminance texture
    // in m_tmp, so bind m_tmp to the framebuffer and run a fullscreen shader

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, m_tmp, 0);

    m_log_shader.bind();

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_log_shader.fullscreen_quad();

    // Now that m_tmp contains the log-luminance texture, tonemap it into the
    // backbuffer, by unbinding the framebuffer and binding m_tmp as texture
    // (remember to mipmap m_tmp, to access the 1x1 average log-luminance)

    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glGenerateMipmapEXT(GL_TEXTURE_2D);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    m_shader.bind();

    glBindTexture(GL_TEXTURE_2D, m_tmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_shader.set("mip_level", (float)mip_level);
    m_shader.set("exposure", exposure);
    m_shader.set("pixel_count", (float)(m_dims.x * m_dims.y));

    m_shader.fullscreen_quad();

    m_shader.unbind();
}

framebuffer::~framebuffer()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteRenderbuffersEXT(1, &m_depth);
    glDeleteFramebuffersEXT(1, &m_fbo);
    glDeleteTextures(1, &m_tmp);
    glDeleteTextures(1, &m_tex);
}
