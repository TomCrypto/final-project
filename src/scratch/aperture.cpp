#include "scratch/aperture.hpp"

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include <cstdlib>
#include <cstdint>

image aperture::generate(size_t width, size_t height, int sides)
{
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
     
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
     
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
     
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        abort();

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,width,height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_TRIANGLES);
    
    for (int t = 0; t < sides; ++t)
    {
        const float radius = 0.8f;
        const float z0 = 0.0f;
    
        float x1 = cos(2 * M_PI / sides * t) * radius;
        float y1 = sin(2 * M_PI / sides * t) * radius;
        
        float x2 = cos(2 * M_PI / sides * (t + 1)) * radius;
        float y2 = sin(2 * M_PI / sides * (t + 1)) * radius;
        
        glVertex3f(x1, y1, z0);
        glVertex3f(x2, y2, z0);
        glVertex3f(0.0f, 0.0f, z0);
    }
    
    
    glEnd();
    
    glutSwapBuffers();
    
    image output(width, height);
    
    uint8_t* buf = (uint8_t*)malloc(width * height * 3);
    uint8_t* old = buf;
    
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
    
    for (size_t y = 0; y < height; ++y)
    {
        glm::vec4* ptr = output[y];
        
        for (size_t x = 0; x < width; ++x)
        {
            ptr->x = (float)(buf[0]) / 255.0f;
            ptr->y = (float)(buf[1]) / 255.0f;
            ptr->z = (float)(buf[2]) / 255.0f;
        
            buf += 3;
            ++ptr;
        }
    }
    
    free(old);
    
    return output;
}
