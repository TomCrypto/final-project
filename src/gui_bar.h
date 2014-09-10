#ifndef GUI_BAR_H
#define GUI_BAR_H

#include "AntTweakBar.h"

class gui_bar
{
public:
    gui_bar(const std::string& title);
    ~gui_bar();
    
    /* generic tweakbar stuff (for initialization) */
    
    void mouse_button_cb(const GLUTmousebuttonfun& cb);
    void mouse_motion_cb(const GLUTmousemotionfun& cb);
    void keyboard_cb(const GLUTkeyboardfun& cb);
    void special_cb(const GLUTspecialfun& cb);
    void resize(int width, int height);
    void draw();
    
    /* variables that will get modified per-frame */
    
    float rotation; // example using assignment 2 skeleton
    
private:
    TwBar *bar;
    
    gui_bar& operator=(const gui_bar& other) = delete;
    gui_bar(const gui_bar& other) = delete;
};

#endif
