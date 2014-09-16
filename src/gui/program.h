#ifndef GUI_PROGRAM_H
#define GUI_PROGRAM_H

#include <string>

#include <FreeImage.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <fftw3.h>

#include "gui/framebuffer.h"
#include "gui/bar.h"

#include "G308_Skeleton.h"
#include "define.h"

namespace gui
{
    // This controls the main window and the program logic. There should only
    // be one program instance, and initialize() should be called first
    class program
    {
    public:
        program(const std::string& window_title,
                const std::pair<int, int>& dims);
        ~program();

        // Runs the window's main loop, returns on window close (or crash)
        void run();

        // To be called at the start and end of the program
        static void initialize(int argc, char* argv[]);
        static void finalize();

        // Different window event callbacks (logic goes here)
        void on_key_press(unsigned char key, int x, int y);
        void on_mouse_down(int button, int x, int y);
        void on_mouse_up(int button, int x, int y);
        void on_special(int key, int x, int y);
        void on_mouse_move(int x, int y);
        void on_resize(int w, int h);
        void on_display();
        void on_init();
        void on_free();
    private:
        program& operator=(const program& other);
        program(const program& other);
        
        // GLUT Window stuff
        int width();
        int height();
        int m_window;

        // Our own stuff
        main_bar* m_bar;
        Skeleton* skeleton;
        fbuffer* buf;
    };
}

#endif