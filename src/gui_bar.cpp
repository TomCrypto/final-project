#include "gui_bar.h"

#include <stdexcept>
#include <GL/glut.h>

static GLUTmousebuttonfun button_cb;
static GLUTmousemotionfun motion_cb;
static GLUTkeyboardfun keybd_cb;
static GLUTspecialfun spec_cb;

static void __button_cb(int glutButton, int glutState, int mouseX, int mouseY)
{
    int retval = TwEventMouseButtonGLUT(glutButton, glutState, mouseX, mouseY);
    if (!retval && button_cb) button_cb(glutButton, glutState, mouseX, mouseY);
}

static void __motion_cb(int mouseX, int mouseY)
{
    int retval = TwEventMouseMotionGLUT(mouseX, mouseY);
    if (!retval && motion_cb) motion_cb(mouseX, mouseY);
}

static void __keyboard_cb(unsigned char glutKey, int mouseX, int mouseY)
{
    int retval = TwEventKeyboardGLUT(glutKey, mouseX, mouseY);
    if (!retval && keybd_cb) keybd_cb(glutKey, mouseX, mouseY);
}

static void __special_cb(int glutKey, int mouseX, int mouseY)
{
    int retval = TwEventSpecialGLUT(glutKey, mouseX, mouseY);
    if (!retval && spec_cb) spec_cb(glutKey, mouseX, mouseY);
}

/* ============================================================================ */

gui_bar::gui_bar(const std::string& title)
{
    TwInit(TW_OPENGL_CORE, NULL);
    bar = TwNewBar(title.c_str());
    if (!bar) throw std::runtime_error("failed to create tweakbar");

    TwGLUTModifiersFunc(glutGetModifiers);
    glutMouseFunc(__button_cb);
    glutMotionFunc(__motion_cb);
    glutPassiveMotionFunc(__motion_cb);
    glutKeyboardFunc(__keyboard_cb);
    glutSpecialFunc(__special_cb);
    
    /* init variables here */
    
    TwAddVarRW(bar, "Rotation", TW_TYPE_FLOAT, &this->rotation,
               " min=0 max=360 step=1.0 help='Rotate the skeleton'");
}

void gui_bar::mouse_button_cb(const GLUTmousebuttonfun& cb)
{
    button_cb = cb;
}

void gui_bar::mouse_motion_cb(const GLUTmousemotionfun& cb)
{
    motion_cb = cb;
}

void gui_bar::keyboard_cb(const GLUTkeyboardfun& cb)
{
    keybd_cb = cb;
}

void gui_bar::special_cb(const GLUTspecialfun& cb)
{
    spec_cb = cb;
}

void gui_bar::resize(int width, int height)
{
    TwWindowSize(width, height);
}

void gui_bar::draw()
{
    TwDraw();
}

gui_bar::~gui_bar()
{
    TwTerminate();
}
