// basic gui on top of AntTweakBar

#ifndef GUI_H
#define GUI_H

#include <string>

#include "AntTweakBar.h"

namespace gui
{
    //<< don't forget to call this at start/end of program
    bool init();
    void free();

    //<< callback overrides for the GLUT callback system
    void mouse_button_cb(const GLUTmousebuttonfun& cb);
    void mouse_motion_cb(const GLUTmousemotionfun& cb);
    void keyboard_cb(const GLUTkeyboardfun& cb);
    void special_cb(const GLUTspecialfun& cb);

    //<< draw/resize functions for the gui
    void resize(int width, int height);
    void draw();

    //<< this does nothing, inherit different bars from it
    class basic_bar
    {
    public:
        basic_bar(const std::string& name);
        ~basic_bar();

        void set_title(const std::string& title);

    protected:
        TwBar *m_bar;
        std::string m_name;

        basic_bar& operator=(const basic_bar& other);
        basic_bar(const basic_bar& other);
    };

    //<< the main bar which controls the main parameters
    class main_bar : public basic_bar
    {
    public:
        main_bar(const std::string& name);

        //<< rotation of the skeleton (TEMPORARY)
        float rotation;
    };
}

#endif
