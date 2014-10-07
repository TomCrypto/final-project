/* Purpose:
 *
 *  - handles the OpenGL context creation (GLUT & GLEW) and decouples it from
 *    the actual program logic, so that conventional C++ RAII may be used
 *  - also handles general program initialization/finalization
 *  - handles AntTweakBar global context initialization
 *  - has an abort_handler() function for safe abort
 *  - the context class takes event callbacks
 *
 * Note the implementation of this class is a bit convoluted because we wanted
 * to be able to throw exceptions from our window events - doing this directly
 * from a C callback is suicidal, so we need a level of indirection to be able
 * to do it properly, which means having C++ wrappers for each callback.
*/

#ifndef GUI_CONTEXT_H
#define GUI_CONTEXT_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <AntTweakBar.h>
#include <FreeImage.h>
#include <glm/glm.hpp>
#include <fftw3.h>

#include <functional>
#include <stdexcept>
#include <string>

namespace gui
{
    typedef std::function<void(int)>               on_mouse_up_t;
    typedef std::function<void(int)>               on_mouse_down_t;
    typedef std::function<void(const glm::ivec2&)> on_mouse_move_t;
    typedef std::function<void(unsigned char)>     on_key_press_t;
    typedef std::function<void(unsigned char)>     on_key_up_t;
    typedef std::function<void(int)>               on_special_t;
    typedef std::function<void(int)>               on_special_up_t;
    typedef std::function<void(const glm::ivec2&)> on_resize_t;
    typedef std::function<void(void)>              on_display_t;
    typedef std::function<void(void)>              on_update_t;

    void abort_handler();
    void initialize();
    void finalize();

    class context
    {
    public:
        context(const std::string& window_title,
                const glm::ivec2&  window_dims,
                const on_mouse_up_t&   mouse_up,
                const on_mouse_down_t& mouse_down,
                const on_mouse_move_t& mouse_move,
                const on_key_press_t&  key_press,
                const on_key_up_t&     key_up,
                const on_special_t&    special,
                const on_special_up_t& special_up,
                const on_resize_t&     resize,
                const on_display_t&    display,
                const on_update_t&     update);

        ~context();

        const GLint& operator()();
        const bool& failed();
        void set_failed();
        void start();

    private:
        context& operator=(context& other);
        context(context& other);
        
        int m_window;
    };
}

#endif
