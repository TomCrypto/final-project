#include <easylogging.h>

#include "gui/framebuffer.h"

#include <cmath>

framebuffer::framebuffer(const glm::ivec2& dims)
    : m_dims(dims),
      m_tex(dims, GL_FLOAT),
      m_cpy(dims, GL_FLOAT),
      m_shader("generic/fs_quad.vert", "tonemap/reinhard.frag"),
      m_cpy_shader("generic/fs_quad.vert", "tonemap/copy.frag")
{
    if (!GLEW_ARB_framebuffer_object && !GLEW_EXT_framebuffer_object) {
        LOG(ERROR) << "Framebuffer requires ARB or EXT framebuffer_object"
                   << " but neither is available! (is this OpenGL 2.1?)";
        throw std::runtime_error("");
    }

    if (GLEW_ARB_framebuffer_object) {
        glGenFramebuffers(1, &m_fbo);
        glGenRenderbuffers(1, &m_depth);
    } else if (GLEW_EXT_framebuffer_object) {
        glGenFramebuffersEXT(1, &m_fbo);
        glGenRenderbuffersEXT(1, &m_depth);
    }

    resize(m_dims);
    bind();

    // Attach depth buffer to FBO

    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, m_depth);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, m_depth);
    }

    //Does the GPU support current FBO configuration?

    if (GLEW_ARB_framebuffer_object) {
        switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            case GL_FRAMEBUFFER_COMPLETE:
                LOG(INFO) << "Framebuffer successfully initialized.";
                LOG(TRACE) << "Resolution " << dims.x <<
                              " by " << dims.y << " pixels.";
                break;
            default:
                LOG(ERROR) << "Failed to create the framebuffer!";
                throw std::runtime_error("");
        }
    } else if (GLEW_EXT_framebuffer_object) {
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
    }

    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}

void framebuffer::resize(const glm::ivec2& dims)
{
    m_dims = dims;

    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_tex(), 0);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, m_tex(), 0);
    }

    m_tex.resize(dims);
    m_cpy.resize(dims);

    if (GLEW_ARB_framebuffer_object) {
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                              m_dims.x, m_dims.y);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                                 m_dims.x, m_dims.y);
    }
}

void framebuffer::bind()
{
    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_tex(), 0);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, m_tex(), 0);
    }
}

void framebuffer::bind_as(const gl::texture2D& other)
{
    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, other(), 0);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, other(), 0);
    }
}

void framebuffer::clear(bool depth)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void framebuffer::render(float exposure)
{
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, m_dims.x, m_dims.y);

    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    }

    m_shader.bind();
    m_tex.bind(0);

    m_shader.set("render", 0);
    m_shader.set("exposure", exposure);

    m_shader.fullscreen_quad();

    m_shader.unbind();
}

const gl::texture2D& framebuffer::frame_copy()
{
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, m_dims.x, m_dims.y);

    bind_as(m_cpy);

    m_cpy_shader.bind();
    m_tex.bind(0);
    m_cpy_shader.set("render", 0);

    m_cpy_shader.fullscreen_quad();
    m_cpy_shader.unbind();

    bind();
    glEnable(GL_DEPTH_TEST);

    return m_cpy;
}

framebuffer::~framebuffer()
{
    if (GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteRenderbuffers(1, &m_depth);
        glDeleteFramebuffers(1, &m_fbo);
    } else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteRenderbuffersEXT(1, &m_depth);
        glDeleteFramebuffersEXT(1, &m_fbo);
    }
}
