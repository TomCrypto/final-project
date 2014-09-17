#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <FreeImage.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <fftw3.h>

#include "gui/framebuffer.h"
#include "gui/tweakbar.h"

#include "G308_Skeleton.h"
#include "define.h"
#include "core/camera.h"

namespace gui
{
    // This controls the main window and the program logic. There should only
    // be one instance of this class, and initialize() should be called first
    class window
    {
    public:
        window(const std::string& window_title,
                const std::pair<int, int>& dims);
        ~window();

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
        void on_update();
        void on_init();
        void on_free();
    private:
        window& operator=(const window& other);
        window(const window& other);

        // GLUT window stuff
        int width();
        int height();
        int m_window;

        // Our own stuff
        main_bar* m_bar;
        Skeleton* skeleton;
        fbuffer* buf;
        camera m_cam;
    };
}

#endif
