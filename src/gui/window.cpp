#include <easylogging.h>
#include "gui/window.h"

using namespace std::placeholders;

namespace gui
{
    window::window(const std::string& window_title, const glm::ivec2& dims)
        : m_cursor_locked(false), m_dims(dims), m_fps(51),
          #if 0
          m_lighthouse("lighthouse/Lighthouse.obj"),
          m_outbuilding("lighthouse/OutBuilding.obj"),
          m_terrain("lighthouse/Terrain.obj"),
          m_tree("lighthouse/Tree.obj"),
          #endif
          m_fft(glm::ivec2(3072)),
          m_context(window_title, dims,
                    std::bind(&window::on_mouse_up, this, _1),
                    std::bind(&window::on_mouse_down, this, _1),
                    std::bind(&window::on_mouse_move, this, _1),
                    std::bind(&window::on_key_press, this, _1),
                    std::bind(&window::on_key_up, this, _1),
                    std::bind(&window::on_special, this, _1),
                    std::bind(&window::on_special_up, this, _1),
                    std::bind(&window::on_resize, this, _1),
                    std::bind(&window::on_display, this),
                    std::bind(&window::on_update, this)),
          m_bar("main"),
          m_cam(m_dims, glm::vec3(0, 3, -5), glm::vec3(0, 0, 1),
                m_bar.cam_fov * glm::pi<float>() / 180),
          m_skybox(),
          m_overlay(m_bar.lens_density),
          m_aperture(glm::ivec2(1024, 1024),
                     aperture_params(),
                     m_fft),
          m_occlusion(),
          m_framebuffer(m_dims)
    {
        
    }

    void window::run()
    {
        m_context.start();
    }

    void window::on_resize(const glm::ivec2& new_dims)
    {
        m_dims = new_dims;

        m_framebuffer.resize(m_dims);
        m_cam.resize(m_dims);
    }

    void window::on_display()
    {
        // Step 1: bind and clear the HDR framebuffer, to render in it

        m_framebuffer.bind();
        m_framebuffer.clear(true);

        // Step 2: draw our objects and effects in the HDR framebuffer

        glViewport(0, 0, m_dims.x, m_dims.y);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_cam.proj()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_cam.view()));

        glRotatef(m_bar.rotation, 0, 1, 0);

        glEnable(GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		float v[4];
		v[0] = v[1] = v[2] = 0.5f*0.4f; v[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_AMBIENT, v);
		v[0] = v[1] = v[2] = 3*0.8f; v[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
		v[0] = m_bar.LightSun.x; v[1] = m_bar.LightSun.y; v[2] = m_bar.LightSun.z; v[3] = 0.0f;
		glLightfv(GL_LIGHT0, GL_POSITION, v);
	    glEnable(GL_COLOR_MATERIAL);
	    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	    glShadeModel(GL_SMOOTH);

        m_skybox.display(m_cam,m_bar.Atmos);


		glPushMatrix();
		//glScalef(0.05f, 0.05f, 0.05f);
		//glColor3f(m_bar.color1.x, m_bar.color1.y, m_bar.color1.z);
		//m_lighthouse->display();
		//glColor3f(m_bar.color2.x, m_bar.color2.y, m_bar.color2.z);
		//m_outbuilding->display();
		//glColor3f(m_bar.color3.x, m_bar.color3.y, m_bar.color3.z);
		//m_terrain->display();
		//glColor3f(m_bar.color4.x, m_bar.color4.y, m_bar.color4.z);
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

        /*glm::vec4 sun_pos = -glm::vec4(-cos(glm::radians(-m_bar.Atmos.theta)),
                                       sin(glm::radians(-m_bar.Atmos.theta)),
                                       m_bar.Atmos.phi / 90,
                                       0.0f);*/

        glm::vec4 sun_pos = -glm::vec4(glm::cos(glm::radians(m_bar.Atmos.theta)),
                                       glm::sin(glm::radians(m_bar.Atmos.theta))*glm::cos(glm::radians(m_bar.Atmos.phi)),
                                       glm::sin(glm::radians(m_bar.Atmos.theta))*glm::sin(glm::radians(m_bar.Atmos.phi)),
                                       0.0f);

        glm::vec3 sun_strength = glm::vec3(10000, 10000, 10000);
        
        float sun_radius = 0.12f; // experimentally determined - radius of sun as viewed by camera

        std::vector<light> lights;
        lights.push_back(light(sun_pos, sun_strength, sun_radius));

        // OCCLUSION QUERY <<< HERE >>>

        const gl::texture2D& occlusion = m_occlusion.query(lights, m_framebuffer, m_cam);
        //image img = image(glm::ivec2(8, 8), occlusion());
        //img.save("test.exr");

        // END OCCLUSION QUERY

        m_aperture.render(lights, occlusion, m_cam);

        if (m_bar.lens_overlay) {
            m_overlay.render(lights, occlusion, m_cam, m_bar.lens_reflectivity);
        }

        /*====================================================================
         * POSTPROCESSING STOPS HERE - TONEMAPPING AND GUI RENDERING STARTS HERE
         *====================================================================*/

        // Step 3: render tonemapped HDR render to backbuffer

        m_framebuffer.render(m_bar.exposure);

        // Step 4: draw the AntTweakBar overlay on top

        TwDraw();

        /*====================================================================
         * RENDERING COMPLETE
         *====================================================================*/

        // Step 5: present the result to the screen

        glutSwapBuffers();
        m_fps.add_frame();
        if (m_fps.average_ready()) {
            int period = (int)(30.0 * 60.0f);
            int fps = (int)(1.0 / m_fps.get_average() + 0.5);
            LOG_EVERY_N(period, INFO) << fps << " frames per second.";
        }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOG(WARNING) << "OpenGL: " << (char*)gluErrorString(err) << ".";
        }
    }

    void window::on_update()
    {
        m_cam.set_fov(m_bar.cam_fov * glm::pi<float>() / 180);
        m_bar.cam_locked = m_cursor_locked;
        m_bar.refresh();

        if (m_bar.aperture_regen_btn) {
            m_bar.aperture_regen_btn = false;
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

        if (m_overlay.get_density() != m_bar.lens_density) {
            m_overlay.regenerate_film(m_bar.lens_density);
        }

        if (m_keys[27 /* escape */]) {
            glutLeaveMainLoop();
            return;
        }

        float move_speed = m_bar.cam_move_speed / 60.0f;

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

        if (m_cursor_locked) {
            glutWarpPointer(m_dims.x / 2, m_dims.y / 2);
            m_mouse.set_pos((glm::vec2)m_dims / (2.0f * m_dims.x));
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
            m_cursor_locked = !m_cursor_locked;

            glutSetCursor(m_cursor_locked ? GLUT_CURSOR_NONE
                                          : GLUT_CURSOR_INHERIT);
        }
    }
    
    void window::on_mouse_move(const glm::ivec2& pos)
    {
        auto mouse_pos = (glm::vec2)pos / (float)m_dims.x;

        if (m_cursor_locked || m_buttons[GLUT_LEFT_BUTTON]) {
            m_cam.turn(m_mouse.delta(mouse_pos) * m_bar.cam_sensitivity);
        }

        m_mouse.set_pos(mouse_pos);
    }
}
