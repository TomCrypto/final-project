/* Purpose:
 *
 *  - controls the program's main loop
 *
 * The window class implements the program's high level logic, as evidenced by
 * the fact that it includes essentially every single gui and core header.
*/

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <glm/glm.hpp>

#include <string>
#include <map>

#include "gui/context.h"
#include "gui/tweakbar.h"
#include "gui/framebuffer.h"
#include "gui/fps_counter.h"
#include "gui/mouse_tracker.h"

#include "core/model.h"
#include "core/camera.h"
#include "core/skybox.h"
#include "core/overlay.h"
#include "core/aperture.h"
#include "core/occlusion.h"

#if _WIN32
#define NO_LENS_FLARES 1
#endif

namespace gui
{
    class window
    {
    public:
        window(const std::string& window_title, const glm::ivec2& dims);
        void run();

    private:
        window& operator=(const window& other);
        window(const window& other);

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

        // GLUT windowing/keyboard stuff
        std::map<int, bool> m_buttons;
        std::map<int, bool> m_keys;
        bool m_cursor_locked;
        glm::ivec2 m_dims;

        // >>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<
        // MUST BE IN ORDER OF INITIALIZATION/DEPENDENCY
        // >>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<

        // Stuff that can be done without OpenGL

        fft_engine m_fft;
        fps_counter m_fps;
        mouse_tracker m_mouse;

        // We initialize OpenGL/GLEW afterwards

        context m_context;

        // Stuff that needs OpenGL functionality

        main_bar m_bar;
        camera m_camera;

        Model m_lighthouse;
        Model m_outbuilding;
        Model m_terrain;
        Model m_tree;
		Model m_light;

        skybox m_skybox;
        #if !NO_LENS_FLARES
        overlay m_overlay;
        aperture m_aperture;
        occlusion m_occlusion;
        #endif
        framebuffer m_framebuffer;
    };
}

#endif
