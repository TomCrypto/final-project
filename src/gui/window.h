#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <easylogging.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <FreeImage.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdexcept>
#include <fftw3.h>
#include <string>
#include <map>

#include "gui/framebuffer.h"
#include "gui/fps_counter.h"
#include "gui/raw_mouse.h"
#include "gui/tweakbar.h"

#include "core/model.h"
#include "core/camera.h"
#include "core/aperture.h"

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
        window(const std::string& window_title, const glm::ivec2& dims);
        ~window();

        // Runs the window's main loop, returns on window close (or crash)
        void run();

        // To be called at the start and end of the program
        static void initialize(int argc, char* argv[]);
        static void finalize();

        // Different window event callbacks (logic goes here)
        void on_key_press(unsigned char key);
        void on_key_up(unsigned char key);
        void on_mouse_down(int button);
        void on_mouse_up(int button);
        void on_special_up(int key);
        void on_special(int key);
        void on_mouse_move(const glm::ivec2& pos);
        void on_resize(const glm::ivec2& new_dims);
        void on_display();
        void on_update();
        void on_load();
        void on_init();
        void on_free();
    private:
        window& operator=(const window& other);
        window(const window& other);

        // GLUT windowing/keyboard stuff
        std::map<int, bool> m_buttons;
        std::map<int, bool> m_keys;
        bool m_lock_cursor;
        glm::ivec2 m_dims;
        int m_window;

        // Our own stuff
        fps_counter m_fps;
        raw_mouse m_mouse;
        main_bar* m_bar;
        framebuffer* m_framebuffer;
        aperture* m_aperture;
        camera m_cam;
		Model* m_obj;
    };
}

#endif
