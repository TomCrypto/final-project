#include "gui.hpp"

#include <stdexcept>
#include <GL/glut.h>

namespace gui
{
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

    bool init()
    {

        bool success = (bool)TwInit(TW_OPENGL_CORE, NULL);
        if (success)
        {
            TwGLUTModifiersFunc(glutGetModifiers);
            glutMouseFunc(__button_cb);
            glutMotionFunc(__motion_cb);
            glutPassiveMotionFunc(__motion_cb);
            glutKeyboardFunc(__keyboard_cb);
            glutSpecialFunc(__special_cb);
        }
        return success;
    }

    void free()
    {
        TwTerminate();

        button_cb = nullptr;
        motion_cb = nullptr;
        keybd_cb = nullptr;
        spec_cb = nullptr;
    }

    void mouse_button_cb(const GLUTmousebuttonfun& cb)
    {
        button_cb = cb;
    }

    void mouse_motion_cb(const GLUTmousemotionfun& cb)
    {
        motion_cb = cb;
    }

    void keyboard_cb(const GLUTkeyboardfun& cb)
    {
        keybd_cb = cb;
    }

    void special_cb(const GLUTspecialfun& cb)
    {
        spec_cb = cb;
    }

    void resize(int width, int height)
    {
        TwWindowSize(width, height);
    }

    void draw()
    {
        TwDraw();
    }

    /* ==================================================================== */

    basic_bar::basic_bar(const std::string& name)
    {
        if (!(m_bar = TwNewBar((m_name = name).c_str())))
            throw std::runtime_error("failed to create tweakbar");
    }

    basic_bar::~basic_bar()
    {
        TwDeleteBar(m_bar);
    }

    void basic_bar::set_title(const std::string& title)
    {
        TwDefine((m_name + " label='" + title + "'").c_str());
    }

    /* ==================================================================== */

    main_bar::main_bar(const std::string& name) : basic_bar(name)
    {
        TwAddVarRW(m_bar,
            "Rotation", TW_TYPE_FLOAT, &this->rotation,
            "min=0 max=360 step=1.0 help='Rotate the skeleton'");
    }
}
