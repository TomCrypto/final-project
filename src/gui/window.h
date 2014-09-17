#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <easylogging.h>
#include <glm/gtc/type_ptr.hpp>
#include <FreeImage.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdexcept>
#include <fftw3.h>
#include <string>
#include <map>

#include "gui/framebuffer.h"
#include "gui/tweakbar.h"

#include "core/model.h"
#include "core/camera.h"

#include "gui/fps_counter.h"

namespace gui
{
    // This is for deferred exception handling, because throwing exceptions in
    // C callbacks is a nice way to destroy your stack, as well as because of
    // some GLUT peculiarities.
    class exception
    {
    public:
        static bool has_failed() {
            return m_failed;
        }
        
        static void fail() {
            m_failed = true;
        }
    private:
        static bool m_failed;
    };

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

        // Special error-handling stuff (see window.cpp)
        static bool has_failed();
        static void set_failed(const std::exception& e);
        static std::exception get_last_exception();

        // Different window event callbacks (logic goes here)
        void on_key_press(unsigned char key, int x, int y);
        void on_key_up(unsigned char key, int x, int y);
        void on_mouse_down(int button, int x, int y);
        void on_mouse_up(int button, int x, int y);
        void on_special_up(int key, int x, int y);
        void on_special(int key, int x, int y);
        void on_mouse_move(int x, int y);
        void on_resize(int w, int h);
        void on_display();
        void on_update();
        void on_load(int w, int h);
        void on_init();
        void on_free();
    private:
        window& operator=(const window& other);
        window(const window& other);

        // GLUT windowing/keyboard stuff
        std::map<int, bool> m_keys;
        int width();
        int height();
        int m_window;

        // Our own stuff
        fps_counter m_fps;
        main_bar* m_bar;
        framebuffer* m_framebuffer;
        camera m_cam;
		Model* m_obj;
    };
}

#endif
