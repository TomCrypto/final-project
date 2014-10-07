#include <easylogging.h>

#include "gui/window.h"

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
    static window* cur_wnd = nullptr;

    static void mouse_dispatch_cb(int button, int state)
    {
        if (state == GLUT_UP)
            return cur_wnd->on_mouse_up(button);
        else if (state == GLUT_DOWN)
            return cur_wnd->on_mouse_down(button);
    }

    static void button_cb(int button, int state, int x, int y)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                int retval = TwEventMouseButtonGLUT(button, state, x, y);
                if (!retval) mouse_dispatch_cb(button, state);
            }
         } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
         }
    }

    static void motion_cb(int x, int y)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                int retval = TwEventMouseMotionGLUT(x, y);
                if (!retval) cur_wnd->on_mouse_move(glm::ivec2(x, y));
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void keyboard_cb(unsigned char key, int x, int y)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                int retval = TwEventKeyboardGLUT(key, x, y);
                if (!retval) cur_wnd->on_key_press(key);
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void keyboard_up_cb(unsigned char key, int /*x*/, int /*y*/)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) cur_wnd->on_key_up(key);
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void special_cb(int key, int x, int y)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                int retval = TwEventSpecialGLUT(key, x, y);
                if (!retval) cur_wnd->on_special(key);
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void special_up_cb(int key, int /*x*/, int /*y*/)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) cur_wnd->on_special_up(key);
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void resize_cb(int w, int h)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                TwWindowSize(w, h);
                cur_wnd->on_resize(glm::ivec2(w, h));
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void display_cb()
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) cur_wnd->on_display();
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void update_cb(int /*value*/)
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                int period = (int)1000.0f / target_fps;
                glutTimerFunc(period, update_cb, 0);
                cur_wnd->on_update();
                glutPostRedisplay();
            }
        } catch (...) {
            exception::fail();
            glutLeaveMainLoop();
        }
    }

    static void shutdown_cb()
    {
        if (exception::has_failed())
            glutLeaveMainLoop();
        else try {
            if (cur_wnd) {
                cur_wnd->on_free();
                TwTerminate();
            }
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
        if (fmt) LOG(TRACE) << "Details: " << fmt << ".";
    }

    void window::initialize()
    {
        LOG(INFO) << "Initializing FFTW (multithreaded, 8 threads).";

        fftwf_init_threads();
        fftwf_plan_with_nthreads(8);

        LOG(TRACE) << "FFTW ready (" << "3.3.4" /* fftwf_version */ << ").";
        LOG(INFO) << "Initializing FreeImage.";

        FreeImage_Initialise();
        FreeImage_SetOutputMessage(fi_error_handler);

        LOG(TRACE) << "FreeImage ready (" << FreeImage_GetVersion() << ").";
    }

    void window::finalize()
    {
        LOG(INFO) << "Shutting down.";

        FreeImage_DeInitialise();
        fftwf_cleanup_threads();
        fftwf_cleanup();
    }

    /* ==================================================================== */
    /* ==================================================================== */
    /* ============== GENERIC WINDOWING SETUP AND SERVICES ================ */
    /* ==================================================================== */
    /* ==================================================================== */

    window::window(const std::string& window_title, const glm::ivec2& dims)
        : m_fps(51), m_lock_cursor(false), m_dims(dims), m_window(-1),
          m_fft(glm::ivec2(3072, 3072))
    {
        try {
            on_load();

            int fake_argc = 1;
            char* fake_argv[2] = { (char*)"foo", 0 };
            glutInit(&fake_argc, fake_argv); // bit of a hack...
            glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
            glutInitWindowSize(dims.x, dims.y);
            glutInitWindowPosition(
                (glutGet(GLUT_SCREEN_WIDTH)-dims.x)/2,
                (glutGet(GLUT_SCREEN_HEIGHT)-dims.y)/2);
            glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
                          GLUT_ACTION_GLUTMAINLOOP_RETURNS);
            m_window = glutCreateWindow(window_title.c_str());

            TwGLUTModifiersFunc(glutGetModifiers);
            glutKeyboardFunc(keyboard_cb);
            glutKeyboardUpFunc(keyboard_up_cb);
            glutSpecialFunc(special_cb);
            glutSpecialUpFunc(special_up_cb);

            glutPassiveMotionFunc(motion_cb);
            glutMotionFunc(motion_cb);
            glutMouseFunc(button_cb);

            glutDisplayFunc(display_cb);
            glutReshapeFunc(resize_cb);
            glutCloseFunc(shutdown_cb);

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

            LOG(INFO) << "Target framerate is "
                      << target_fps
                      << " frames per second.";

            if (GLEW_VERSION_3_3) {
                LOG(TRACE) << "OpenGL version 3.3 available, using "
                           << "TW_OPENGL_CORE (better performance).";

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

            cur_wnd = this;
            on_init();
        } catch (...) {
            /* We do this because some drivers such as AMD like to segfault if
             * glutMainLoop isn't called at all (due to, say, an error). So we
             * simply set the exception here which will cause the next call to
             * glutMainLoop to instantly return with an error.
            */

            exception::fail();
        }
    }

    window::~window()
    {
        cur_wnd = 0; /* see on_free() */
    }

    void window::run()
    {
        if (m_window != -1) {
            /* Timer to start up the loop */
            glutTimerFunc(16, update_cb, 0);
            glutMainLoop();
        }
    }

    void window::on_key_up(unsigned char key)
    {
        m_keys[key] = false;
    }

    void window::on_key_press(unsigned char key)
    {
        m_keys[key] = true;
    }

    void window::on_special_up(int key)
    {
        m_keys[key] = false;
    }

    void window::on_special(int key)
    {
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
        /* Putwork in here that can be done without any OpenGL support such as
         * loading stuff from files into CPU buffers. You cannot use functions
         * from GL, GLU, GLUT, GLEW, or AntTweakBar functions in here as there
         * is no OpenGL context active yet.
        */
		LOG(INFO) << "Loading models";

		//m_lighthouse = new Model("lighthouse/Lighthouse.obj");
		//m_outbuilding = new Model("lighthouse/OutBuilding.obj");

		//m_terrain = new Model("lighthouse/Terrain.obj");
		//m_tree = new Model("lighthouse/Trees.obj");
		LOG(INFO) << "All Models Loaded.";

        //m_aperture = new aperture(glm::ivec2(1024, 1024),
        //                          aperture_params(),
        //                          m_fft);

        #if 0
        LOG(INFO) << "Generating aperture.";

        auto ap = m_aperture->gen_aperture(glm::ivec2(1024, 1024));
        ap = ap.resize(glm::ivec2(350, 350));
        ap = ap.enlarge(glm::ivec2(1024, 1024));

        LOG(INFO) << "Generating chromatic FFT.";

        auto cfft = m_aperture->get_cfft(ap, glm::ivec2(1024, 1024));
        cfft.save("flare.exr");

        auto test = m_aperture->get_flare(cfft, 8);
        test.save("convolved8.exr");

        test = m_aperture->get_flare(cfft, 16);
        test.save("convolved16.exr");
        #endif
    }

    void window::on_init()
    {
        LOG(INFO) << "Creating tweak bar.";

        m_bar = new main_bar("main");
        m_bar->set_title("Configuration");

        LOG(INFO) << "Setting up fixed function pipeline.";

	    LOG(INFO) << "Creating framebuffer.";

        m_framebuffer = new framebuffer(m_dims);

		LOG(INFO) << "Creating camera.";

        m_cam = camera(m_dims, glm::vec3(0, 3, -5), glm::vec3(0, 0, 1),
                       m_bar->cam_fov * glm::pi<float>() / 180);
		LOG(INFO) << "Creating skybox.";
        m_sky = new skybox();
        
        LOG(INFO) << "Creating overlay.";
        m_overlay = new overlay(m_bar->lens_density);

        LOG(INFO) << "Creating occlusion.";
        m_occlusion = new occlusion();
        
        m_aperture = new aperture(glm::ivec2(1024, 1024),
                                  aperture_params(),
                                  m_fft);
        
		LOG(INFO) << "Finished creating stuff.";
    }

    void window::on_free()
    {
        /* This probably deserves some kind of explanation. The reason we free
         * all our stuff here and not in the destructor, is because due to the
         * way GLUT works, the OpenGL context is gone the moment the window is
         * closed, and hence will be long gone by the time the destructor gets
         * called. However graphics resources should be cleaned up before that
         * happens, thus the on_free() function is hooked up not to the window
         * destructor but to shutdown_cb()->glutCloseFunc which is essentially
         * our only chance to execute cleanup code within an OpenGL context.
        */

		delete m_bar;
		//delete m_lighthouse;
		//delete m_outbuilding;
		//delete m_terrain;
		//delete m_tree;
        delete m_framebuffer;
        delete m_sky;
        delete m_aperture;
        delete m_overlay;
        delete m_occlusion;
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
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		float v[4];
		v[0] = v[1] = v[2] = 0.5f*0.4f; v[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_AMBIENT, v);
		v[0] = v[1] = v[2] = 3*0.8f; v[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
		v[0] = m_bar->LightSun.x; v[1] = m_bar->LightSun.y; v[2] = m_bar->LightSun.z; v[3] = 0.0f;
		glLightfv(GL_LIGHT0, GL_POSITION, v);
	    glEnable(GL_COLOR_MATERIAL);
	    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	    glShadeModel(GL_SMOOTH);

        m_sky->display(m_cam,m_bar->Atmos);


		glPushMatrix();
		//glScalef(0.05f, 0.05f, 0.05f);
		//glColor3f(m_bar->color1.x, m_bar->color1.y, m_bar->color1.z);
		//m_lighthouse->display();
		//glColor3f(m_bar->color2.x, m_bar->color2.y, m_bar->color2.z);
		//m_outbuilding->display();
		//glColor3f(m_bar->color3.x, m_bar->color3.y, m_bar->color3.z);
		//m_terrain->display();
		//glColor3f(m_bar->color4.x, m_bar->color4.y, m_bar->color4.z);
		//m_tree->display();
		glPopMatrix();
        glDisable(GL_DEPTH_TEST);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_COLOR_MATERIAL);

        #if 0
        m_aperture->render(m_cam);
        #endif

        /*====================================================================
         * 3D RENDERING STOPS HERE - SCREEN SPACE POSTPROCESSING STARTS HERE
         *====================================================================*/
        
        /* TEMPORARY: recalculate sun position here to pass to overlay.
         * later this could be done by e.g. asking m_sky for it. */

        /*glm::vec4 sun_pos = -glm::vec4(-cos(glm::radians(-m_bar->Atmos.theta)),
                                       sin(glm::radians(-m_bar->Atmos.theta)),
                                       m_bar->Atmos.phi / 90,
                                       0.0f);*/

        glm::vec4 sun_pos = -glm::vec4(glm::cos(glm::radians(m_bar->Atmos.theta)),
                                       glm::sin(glm::radians(m_bar->Atmos.theta))*glm::cos(glm::radians(m_bar->Atmos.phi)),
                                       glm::sin(glm::radians(m_bar->Atmos.theta))*glm::sin(glm::radians(m_bar->Atmos.phi)),
                                       0.0f);

        glm::vec3 sun_strength = glm::vec3(1000, 1000, 1000);
        
        float sun_radius = 0.20f; // experimentally determined - radius of sun as viewed by camera

        std::vector<light> lights;
        lights.push_back(light(sun_pos, sun_strength, sun_radius));

        // OCCLUSION QUERY <<< HERE >>>

        const gl::texture2D& occlusion = m_occlusion->query(lights, *m_framebuffer, m_cam);
        //image img = image(glm::ivec2(8, 8), occlusion());
        //img.save("test.exr");

        // END OCCLUSION QUERY

        m_aperture->render(lights, occlusion, m_cam);
        // lens flares would go here, using lights+occlusion+m_cam (pretty much like the overlay)

        if (m_bar->lens_overlay) {
            m_overlay->render(lights, occlusion, m_cam, m_bar->lens_reflectivity);
        }

        /*====================================================================
         * POSTPROCESSING STOPS HERE - TONEMAPPING AND GUI RENDERING STARTS HERE
         *====================================================================*/

        // Step 3: render tonemapped HDR render to backbuffer

        m_framebuffer->render(m_bar->exposure);

        // Step 4: draw the AntTweakBar overlay on top

        TwDraw();

        /*====================================================================
         * RENDERING COMPLETE
         *====================================================================*/

        // Step 5: present the result to the screen

        glutSwapBuffers();
        m_fps.add_frame();
        if (m_fps.average_ready()) {
            int period = (int)(30.0 * target_fps);
            int fps = (int)(1.0 / m_fps.get_average() + 0.5);
            LOG_EVERY_N(period, INFO) << fps << " frames per second.";
        }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOG(WARNING) << "OpenGL: " << (char*)gluErrorString(err) << ".";
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

        if (m_bar->aperture_regen_btn) {
            m_bar->aperture_regen_btn = false;
            #if 0
            LOG(INFO) << "Regenerating aperture (this may take a while)";

            auto ap = m_aperture->gen_aperture(glm::ivec2(1024, 1024));
            ap = ap.resize(glm::ivec2(350));
            ap = ap.enlarge(glm::ivec2(1024));

            LOG(INFO) << "Generating chromatic FFT.";

            auto cfft = m_aperture->get_cfft(ap, glm::ivec2(1024, 1024));
            cfft.save("flare.exr");

            auto test = m_aperture->get_flare(cfft, 8);
            test.save("convolved8.exr");

            test = m_aperture->get_flare(cfft, 16);
            test.save("convolved16.exr");

            test = m_aperture->get_flare(cfft, 2);
            test.save("convolved2.exr");

            LOG(INFO) << "Done!";
            #else
            LOG(INFO) << "Disabled for now!";
            #endif
        }

        if (m_overlay->get_density() != m_bar->lens_density) {
            m_overlay->regenerate_film(m_bar->lens_density);
        }

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
