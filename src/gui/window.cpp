#include "gui/window.h"

#include <GL/freeglut.h>
#include <stdexcept>

namespace gui
{
    /* ==================================================================== */
    /* ==================================================================== */
    /* ========== BOILERPLATE GLUT CALLBACK STUFF - SCROLL DOWN =========== */
    /* ==================================================================== */
    /* ==================================================================== */

    bool exception::m_failed = false;
    static const int target_fps = 60;
    static window* prog = nullptr;
    static char** _argv;
    static int _argc;

    static void on_mouse_dispatch(int button, int state)
    {
        switch (state)
        {
            case GLUT_UP:
                return prog->on_mouse_up(button);
            case GLUT_DOWN:
                return prog->on_mouse_down(button);
        }
    }

    static void __button_cb(int glutButton, int glutState,
                            int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) {
                int retval = TwEventMouseButtonGLUT(glutButton, glutState,
                                                    mouseX, mouseY);
                if (!retval) on_mouse_dispatch(glutButton, glutState);
            }
         } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
         }
    }

    static void __motion_cb(int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try
        {
            if (prog) {
                int retval = TwEventMouseMotionGLUT(mouseX, mouseY);
                if (!retval) prog->on_mouse_move(glm::ivec2(mouseX, mouseY));
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void __keyboard_cb(unsigned char glutKey,
                              int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try
        {
            if (prog) {
                int retval = TwEventKeyboardGLUT(glutKey, mouseX, mouseY);
                if (!retval) prog->on_key_press(glutKey);
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }
    
    static void __keyboard_up_cb(unsigned char glutKey,
                                 int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try
        {
            if (prog) prog->on_key_up(glutKey);
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void __special_cb(int glutKey, int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) {
                int retval = TwEventSpecialGLUT(glutKey, mouseX, mouseY);
                if (!retval) prog->on_special(glutKey);
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }
    
    static void __special_up_cb(int glutKey, int mouseX, int mouseY)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) {
                int retval = TwEventSpecialGLUT(glutKey, mouseX, mouseY);
                if (!retval) prog->on_special_up(glutKey);
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void __resize_cb(int width, int height)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try
        {
            if (prog) {
                TwWindowSize(width, height);
                prog->on_resize(glm::ivec2(width, height));
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void __display_cb()
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) prog->on_display();
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void __update_cb(int value)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) {
                glutTimerFunc((int)(1000.0f / target_fps),
                              __update_cb, 0);
                prog->on_update();
                glutPostRedisplay();
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }
    
    static void __shutdown_cb()
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (prog) prog->on_free();
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    /* ==================================================================== */
    /* ==================================================================== */
    /* =========== LIBRARY SETUP/TEARDOWN (EXCEPT OPENGL/ETC) ============= */
    /* ==================================================================== */
    /* ==================================================================== */

    static void fi_error_handler(FREE_IMAGE_FORMAT fif, const char *msg)
    {
        LOG(WARNING) << "FreeImage error: " << msg << ".";
        const char *fmt = FreeImage_GetFormatFromFIF(fif);
        if (fmt) LOG(TRACE) << "Context: " << fmt << ".";
    }

    void window::initialize(int argc, char* argv[])
    {
        LOG(INFO) << "Initializing FFTW - multithreaded, 8 threads.";
        LOG(TRACE) << "FFTW 3.3.4.";

        fftwf_init_threads();
        fftwf_plan_with_nthreads(8);
        
        LOG(INFO) << "Initializing FreeImage.";
        
        FreeImage_Initialise();
        FreeImage_SetOutputMessage(fi_error_handler);
        
        LOG(TRACE) << "FreeImage " << FreeImage_GetVersion() << ".";

        _argv = argv;
        _argc = argc;
    }

    void window::finalize()
    {
        LOG(INFO) << "Deinitializing everything.";

        FreeImage_DeInitialise();
        fftwf_cleanup_threads();
        fftwf_cleanup();
        
        LOG(INFO) << "Deinitialization complete.";
    }

    /* ==================================================================== */
    /* ==================================================================== */
    /* ============== GENERIC WINDOWING SETUP AND SERVICES ================ */
    /* ==================================================================== */
    /* ==================================================================== */

    window::window(const std::string& window_title, const glm::ivec2& dims)
        : m_fps(51), m_lock_cursor(false), m_dims(dims)
    {
        if (prog != nullptr) {
            LOG(ERROR) << "Internal error, window already opened.";
            throw 0;
        }

        on_load();
        
        try
        {
            glutInit(&_argc, _argv);
            glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
            glutInitWindowSize(dims.x, dims.y);
            glutInitWindowPosition(
                (glutGet(GLUT_SCREEN_WIDTH)-dims.x)/2,
                (glutGet(GLUT_SCREEN_HEIGHT)-dims.y)/2);
            glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
                          GLUT_ACTION_GLUTMAINLOOP_RETURNS);
            m_window = glutCreateWindow(window_title.c_str());
            TwGLUTModifiersFunc(glutGetModifiers);
            glutPassiveMotionFunc(__motion_cb);
            glutKeyboardFunc(__keyboard_cb);
            glutKeyboardUpFunc(__keyboard_up_cb);
            glutSpecialFunc(__special_cb);
            glutSpecialUpFunc(__special_up_cb);
            glutDisplayFunc(__display_cb);
            glutReshapeFunc(__resize_cb);
            glutCloseFunc(__shutdown_cb);
            glutMotionFunc(__motion_cb);
            glutMouseFunc(__button_cb);

            GLenum err = glewInit();
            if (err != GLEW_OK)
            {
                LOG(ERROR) << "GLEW could not be initialized (glewInit failed)";
                LOG(TRACE) << "Error: " << std::string((char*)glewGetErrorString(err));
	            throw 0;
            }

            if (!GLEW_VERSION_2_1)
            {
                LOG(ERROR) << "OpenGL version 2.1 not found, aborting.";
                throw 0;
            }

            LOG(INFO) << "OpenGL version string: " << (char*)glGetString(GL_VERSION) << ".";
            LOG(INFO) << "Renderer string      : " << (char*)glGetString(GL_RENDERER) << ".";
            LOG(INFO) << "GLSL version string  : " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << ".";
            LOG(INFO) << "Target framerate is " << target_fps << " frames per second.";

            if (GLEW_VERSION_3_3) /* Might as well use core profile for AntTweakBar */
            {
                LOG(TRACE) << "OpenGL version 3.3 available, configuring "
                              "AntTweakBar with TW_OPENGL_CORE.";
            
                if (TwInit(TW_OPENGL_CORE, NULL) != 1)
                {
                    LOG(ERROR) << "Failed to initialize AntTweakBar";
                    throw 0;
                }
            }
            else
            {
                LOG(TRACE) << "OpenGL version 3.3 not available, configuring "
                              "AntTweakBar with TW_OPENGL fallback.";
            
                if (TwInit(TW_OPENGL, NULL) != 1)
                {
                    LOG(ERROR) << "Failed to initialize AntTweakBar";
                    throw 0;
                }
            }

            prog = this;
            on_init();
        }
        catch (...)
        {
            /* We do this because some drivers like AMD like to segfault if
             * glutMainLoop isn't called at all (due to, say, an error). So
             * we simply set the exception here so that the next call to
             * glutMainLoop will instantly return with an error.
            */
        
            exception::fail();
        }
    }

    window::~window()
    {
        prog = 0; /* see on_free() */
    }

    void window::run()
    {
        // The timer is to get the loop going
        // without requiring user interaction
        glutTimerFunc(16, __update_cb, 0);
        glutMainLoop();
    }

    void window::on_key_up(unsigned char key) {
        m_keys[key] = false;
    }

    void window::on_key_press(unsigned char key) {
        m_keys[key] = true;
    }

    void window::on_special_up(int key) {
        m_keys[key] = false;
    }

    void window::on_special(int key) {
        m_keys[key] = true;
    }

    void window::on_mouse_up(int button)
    {
        m_buttons[button] = false;
    }

    void window::on_mouse_down(int button)
    {
        m_buttons[button] = true;

        if (button == GLUT_RIGHT_BUTTON) {
            m_lock_cursor = !m_lock_cursor;

            glutSetCursor(m_lock_cursor ? GLUT_CURSOR_NONE
                                        : GLUT_CURSOR_INHERIT);
        }
    }

    /* ==================================================================== */
    /* ==================================================================== */
    /* =========================== PROGRAM LOGIC ========================== */
    /* ==================================================================== */
    /* ==================================================================== */

    void window::on_load()
    {
        // put work in here that can be done without any OpenGL support
        // like loading stuff from files into CPU buffers

        // you cannot use any OpenGL, GLU, GLUT, GLEW, or AntTweakBar
        // functions in here as there is no OpenGL context active yet
    }

    void window::on_init()
    {
        LOG(INFO) << "Creating tweak bar.";

        m_bar = new main_bar("main");
        m_bar->set_title("Configuration");
        
        LOG(INFO) << "Loading model.";

        m_obj = new Model("Bunny.obj");

        LOG(INFO) << "Setting up fixed function pipeline.";

        float direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	    float diffintensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	    float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	    glLightfv(GL_LIGHT0, GL_POSITION, direction);
	    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	    glEnable(GL_LIGHT0);
	    
	    LOG(INFO) << "Creating framebuffer.";

        m_framebuffer = new framebuffer(m_dims);

        LOG(INFO) << "Creating camera.";

        m_cam = camera(m_dims, glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
                       m_bar->cam_fov * glm::pi<float>() / 180);
    }

    void window::on_free()
    {
        /* This probably deserves some kind of explanation. The reason we call
         * TwTerminate() here and not in the destructor, is because due to the
         * way GLUT works, the OpenGL context is gone the moment the window is
         * closed, and hence will be long gone by the time the destructor gets
         * called. However graphics resources should be cleaned up before that
         * happens, thus the on_free() callback is hooked up not to the window
         * destructor but to __shutdown_cb()->glutCloseFunc which is basically
         * our only chance to execute cleanup code within an OpenGL context.
        */

        delete m_bar;
        delete m_obj;
        delete m_framebuffer;
        
        TwTerminate();
    }

    void window::on_resize(const glm::ivec2& new_dims)
    {
        m_dims = new_dims;

        m_framebuffer->resize(m_dims);
        m_cam.resize(m_dims);
    }

    void window::on_display()
    {
        // Step 1: bind and clear the HDR framebuffer, to render in it

        m_framebuffer->bind();
        m_framebuffer->clear(true);

        // Step 2: draw our objects and effects in the HDR framebuffer

        glViewport(0, 0, m_dims.x, m_dims.y);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_cam.proj()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_cam.view()));

        glRotatef(m_bar->rotation, 0, 1, 0);

        glEnable(GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);
	    glEnable(GL_COLOR_MATERIAL);
	    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	    glShadeModel(GL_SMOOTH);

		glColor3f(m_bar->color.x, m_bar->color.y, m_bar->color.z);
		m_obj->display();

        glDisable(GL_DEPTH_TEST);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_COLOR_MATERIAL);

        // Step 3: render tonemapped HDR render to backbuffer

        m_framebuffer->render(m_bar->exposure);

        // Step 4: draw the AntTweakBar overlay on top

        TwDraw();

        // Step 5: present the result to the screen

        glutSwapBuffers();
        m_fps.add_frame();
        if (m_fps.average_ready()) {
            int period = (int)(30.0 * target_fps);
            int fps = (int)(1.0 / m_fps.get_average() + 0.5);
            LOG_EVERY_N(period, INFO) << fps << " frames per second.";
        }
    }

    void window::on_mouse_move(const glm::ivec2& pos)
    {
        auto mouse_pos = (glm::vec2)pos / (float)m_dims.x; 
        
        if (m_lock_cursor || m_buttons[GLUT_LEFT_BUTTON]) {
            auto ds = m_mouse.delta(mouse_pos);
            m_cam.turn(ds * m_bar->cam_sensitivity);
        }

        m_mouse.set_pos(mouse_pos);
    }

    void window::on_update()
    {
        m_cam.set_fov(m_bar->cam_fov * glm::pi<float>() / 180);
        m_bar->cam_locked = m_lock_cursor;
        m_bar->refresh();

        if (m_keys[27 /* escape */]) {
            glutLeaveMainLoop();
            return;
        }

        float move_speed = m_bar->cam_move_speed / target_fps;

        if (m_keys['w'])
            m_cam.move(glm::vec3(0.0f, 0.0f, -1.0f) * move_speed);
        if (m_keys['s'])
            m_cam.move(glm::vec3(0.0f, 0.0f, +1.0f) * move_speed);
        if (m_keys['a'])
            m_cam.move(glm::vec3(-1.0f, 0.0f, 0.0f) * move_speed);
        if (m_keys['d'])
            m_cam.move(glm::vec3(+1.0f, 0.0f, 0.0f) * move_speed);
        if (m_keys['c'])
            m_cam.move(glm::vec3(0.0f, -1.0f, 0.0f) * move_speed);
        if (m_keys[' '])
            m_cam.move(glm::vec3(0.0f, +1.0f, 0.0f) * move_speed);

        if (m_lock_cursor) {
            glutWarpPointer(m_dims.x / 2, m_dims.y / 2);
            m_mouse.set_pos((glm::vec2)m_dims / (2.0f * m_dims.x));
        }
    }
}
