#include "fbuffer.h"

#include <cstdio>

fbuffer::fbuffer(size_t width, size_t height)
{
    glGenTextures(1, &m_tex);
   glBindTexture(GL_TEXTURE_2D, m_tex);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //NULL means reserve texture memory, but texels are undefined
   //**** Tell OpenGL to reserve level 0
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
   //You must reserve memory for other mipmaps levels as well either by making a series of calls to
   //glTexImage2D or use glGenerateMipmapEXT(GL_TEXTURE_2D).
   //Here, we'll use :
   glGenerateMipmapEXT(GL_TEXTURE_2D);
   //-------------------------
   glGenFramebuffersEXT(1, &m_fbo);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
   //Attach 2D texture to this FBO
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_tex, 0);
   //-------------------------
   glGenRenderbuffersEXT(1, &m_depth);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 256, 256);
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
   //-------------------------
   //and now you can render to GL_TEXTURE_2D
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //-------------------------

   // draw here!

   //-------------------------
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);

   glBindTexture(GL_TEXTURE_2D, m_tex);
   glGenerateMipmapEXT(GL_TEXTURE_2D);
}