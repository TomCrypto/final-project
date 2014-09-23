#include <easylogging.h>

#include "gui/framebuffer.h"

#include <cmath>

framebuffer::framebuffer(const glm::ivec2& dims)
    : m_dims(dims),
      m_tex(dims, GL_FLOAT),
      m_tmp(dims, GL_FLOAT),
      m_shader("generic/fs_quad.vert", "tonemap/reinhard.frag"),
      m_log_shader("generic/fs_quad.vert", "tonemap/log_lum.frag")
{
    glGenFramebuffersEXT(1, &m_fbo);
    glGenRenderbuffersEXT(1, &m_depth);

    resize(m_dims);

    // Attach depth buffer to FBO
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                 GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, m_depth);

    //Does the GPU support current FBO configuration?
    
    switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT))
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

    m_tex.resize(dims);
    m_tmp.resize(dims);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                             m_dims.x, m_dims.y);
}

void framebuffer::bind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, m_tex(), 0);
}

void framebuffer::clear(bool depth)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

static int get_mip_level(const glm::ivec2& dims)
{
    auto m = std::max(dims.x, dims.y);
    return (int)ceil(log(m) / log(2));
}

void framebuffer::render(float exposure)
{
    // Get the 1x1 mip level for the current framebuffer resolutions, this
    // will be equal to the log2 of the largest dimension (width or height)

    int mip_level = get_mip_level(m_dims);

    // Set up the necessary OpenGL state for doing fullscreen quad rendering

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, m_dims.x, m_dims.y);

    // First convert the rendered image (in m_tex) to a log-luminance texture
    // in m_tmp, so bind m_tmp to the framebuffer and run a fullscreen shader

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, m_tmp(), 0);

    m_log_shader.bind();
    m_tex.bind(0);

    m_log_shader.fullscreen_quad();

    // Now that m_tmp contains the log-luminance texture, tonemap it into the
    // backbuffer, by unbinding the framebuffer and binding m_tmp as texture
    // (remember to mipmap m_tmp, to access the 1x1 average log-luminance)

    m_tmp.bind(0);
    glGenerateMipmapEXT(GL_TEXTURE_2D);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    m_shader.bind();
    m_tmp.bind(0);

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
}
