#include <easylogging.h>

#include "gui/context.h"

#include <AntTweakBar.h>
#include <FreeImage.h>
#include <fftw3.h>

namespace gui
{
    static int saved_argc;
    static char **saved_argv;
    static bool has_failed;
    static int window_ready;
    static on_mouse_up_t   on_mouse_up;
    static on_mouse_down_t on_mouse_down;
    static on_mouse_move_t on_mouse_move;
    static on_key_press_t  on_key_press;
    static on_key_up_t     on_key_up;
    static on_special_t    on_special;
    static on_special_up_t on_special_up;
    static on_resize_t     on_resize;
    static on_display_t    on_display;
    static on_update_t     on_update;

    static void mouse_dispatch_cb(int button, int state)
    {
        if (state == GLUT_UP)
            on_mouse_up(button);
        else if (state == GLUT_DOWN)
            on_mouse_down(button);
    }

    static void button_cb(int button, int state, int x, int y)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            int retval = TwEventMouseButtonGLUT(button, state, x, y);
            if (!retval) mouse_dispatch_cb(button, state);
         } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
         }
    }

    static void motion_cb(int x, int y)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            int retval = TwEventMouseMotionGLUT(x, y);
            if (!retval) on_mouse_move(glm::ivec2(x, y));
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void keyboard_cb(unsigned char key, int x, int y)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            int retval = TwEventKeyboardGLUT(key, x, y);
            if (!retval) on_key_press(key);
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void keyboard_up_cb(unsigned char key, int /*x*/, int /*y*/)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            on_key_up(key);
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void special_cb(int key, int x, int y)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            int retval = TwEventSpecialGLUT(key, x, y);
            if (!retval) on_special(key);
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void special_up_cb(int key, int /*x*/, int /*y*/)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            on_special_up(key);
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void resize_cb(int w, int h)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            TwWindowSize(w, h);
            on_resize(glm::ivec2(w, h));
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void display_cb()
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            on_display();
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    static void update_cb(int /*value*/)
    {
        if (has_failed) {
            glutLeaveMainLoop();
        } else try {
            glutTimerFunc(16, update_cb, 0);
            glutPostRedisplay();
            on_update();
        } catch (...) {
            has_failed = true;
            glutLeaveMainLoop();
        }
    }

    context::context(const std::string& window_title,
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
                     const on_update_t&     update)
    {
        if (window_ready) {
            LOG(ERROR) << "Internal logic error - double context creation.";
            throw std::logic_error(""); // this should not happen, but check
        }

        glutInit(&saved_argc, saved_argv);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowSize(window_dims.x, window_dims.y);
        glutInitWindowPosition(100, 100);
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
                      GLUT_ACTION_GLUTMAINLOOP_RETURNS);
        m_window = glutCreateWindow(window_title.c_str());
        glutTimerFunc(16, update_cb, 0);
        window_ready = true;

        on_mouse_up   = mouse_up;
        on_mouse_down = mouse_down;
        on_mouse_move = mouse_move;
        on_key_press  = key_press;
        on_key_up     = key_up;
        on_special    = special;
        on_special_up = special_up;
        on_resize     = resize;
        on_display    = display;
        on_update     = update;

        TwGLUTModifiersFunc(glutGetModifiers);
        glutKeyboardUpFunc(keyboard_up_cb);
        glutSpecialUpFunc(special_up_cb);
        glutKeyboardFunc(keyboard_cb);
        glutSpecialFunc(special_cb);

        glutPassiveMotionFunc(motion_cb);
        glutMotionFunc(motion_cb);
        glutMouseFunc(button_cb);

        glutDisplayFunc(display_cb);
        glutReshapeFunc(resize_cb);

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            LOG(ERROR) << "Failed to initialize GLEW (glewInit failed).";
            LOG(TRACE) << "Error: " << (char*)glewGetErrorString(err);
            throw std::runtime_error("");
        }

        if (!GLEW_VERSION_2_1) {
            LOG(ERROR) << "OpenGL version 2.1 not found, aborting.";
            throw std::runtime_error("");
        }

        LOG(INFO) << "OpenGL version string: "
                  << (char*)glGetString(GL_VERSION)
                  << ".";

        LOG(INFO) << "Renderer driver string: "
                  << (char*)glGetString(GL_RENDERER)
                  << ".";

        LOG(INFO) << "GLSL version string: "
                  << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << ".";

        if (GLEW_VERSION_3_3) {
            LOG(TRACE) << "OpenGL version 3.3 available, using "
                       << "TW_OPENGL_CORE.";

            if (TwInit(TW_OPENGL_CORE, NULL) != 1) {
                LOG(ERROR) << "Failed to initialize AntTweakBar.";
                throw std::runtime_error("");
            }
        } else {
            LOG(TRACE) << "OpenGL 3.3 not available, "
                       << "falling back to TW_OPENGL.";

            if (TwInit(TW_OPENGL, NULL) != 1) {
                LOG(ERROR) << "Failed to initialize AntTweakBar.";
                throw std::runtime_error("");
            }
        }
    }

    context::~context()
    {
        TwTerminate();
    }

    const int& context::operator()()
    {
        return m_window;
    }

    const bool& context::failed()
    {
        return has_failed;
    }

    void context::set_failed()
    {
        has_failed = true;
    }

    void context::start()
    {
        glutMainLoop();
    }

    void abort_handler()
    {
        if (window_ready) {
            has_failed = true;
            glutTimerFunc(16, update_cb, 0);
            glutMainLoop();
        }
    }

    static void fi_error_handler(FREE_IMAGE_FORMAT fif, const char *msg)
    {
        LOG(WARNING) << "FreeImage error: " << msg << ".";
        const char *fmt = FreeImage_GetFormatFromFIF(fif);
        if (fmt) LOG(TRACE) << "Details: " << fmt << ".";
    }

    void initialize(int argc, char *argv[])
    {
        LOG(INFO) << "Initializing FFTW (multithreaded, 8 threads).";

        fftwf_init_threads();
        fftwf_plan_with_nthreads(8);

        LOG(TRACE) << "FFTW ready (" << "3.3.4" /* fftwf_version */ << ").";
        LOG(INFO) << "Initializing FreeImage.";

        FreeImage_Initialise();
        FreeImage_SetOutputMessage(fi_error_handler);

        LOG(TRACE) << "FreeImage ready (" << FreeImage_GetVersion() << ").";

        saved_argc = argc;
        saved_argv = argv;
    }

    void finalize()
    {
        LOG(INFO) << "Shutting down.";

        FreeImage_DeInitialise();
        fftwf_cleanup_threads();
        fftwf_cleanup();
    }
}
