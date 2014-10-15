#include <easylogging.h>

#include "gui/window.h"

#include <functional>
#include <stdexcept>

using namespace std::placeholders;

namespace gui
{
    const glm::vec3& initial_camera_pos = glm::vec3(0, 25, -5);
    const glm::vec3& initial_camera_dir = glm::vec3(-0.2f, -0.1f, 0.9f);

    window::window(const std::string& window_title, const glm::ivec2& dims) :
        m_cursor_locked(false), m_dims(dims), m_fps(60),
        m_fft(glm::ivec2(2048)),
        m_context(window_title, dims,
                  std::bind(&window::on_mouse_up,   this, _1),
                  std::bind(&window::on_mouse_down, this, _1),
                  std::bind(&window::on_mouse_move, this, _1),
                  std::bind(&window::on_key_press,  this, _1),
                  std::bind(&window::on_key_up,     this, _1),
                  std::bind(&window::on_special,    this, _1),
                  std::bind(&window::on_special_up, this, _1),
                  std::bind(&window::on_resize,     this, _1),
                  std::bind(&window::on_display,    this),
                  std::bind(&window::on_update,     this)),
        m_bar("main", "Configuration"),
        m_camera(m_dims,
                 initial_camera_pos,
                 initial_camera_dir,
                 glm::radians(m_bar.cam_fov)),
				 m_lighthouse("models/Lighthouse.obj"),
        m_outbuilding("models/OutBuilding.obj"),
        m_terrain("models/Terrain.obj"),
        m_tree("models/Tree.obj"),
		m_light("models/STLamp.obj"),
        m_skybox(),
        #if !NO_LENS_FLARES
        m_overlay(m_bar.lens_density),
        m_aperture(m_fft),
        m_occlusion(),
        #endif
        m_framebuffer(m_dims)
    {
        LOG(INFO) << "All components initialized.";
        LOG(TRACE) << "Window resolution is "
                   << m_dims.x << " by "
                   << m_dims.y << " pixels.";

		on_update();
    }

    void window::run()
    {
        m_context.main_loop();
    }

    void window::on_resize(const glm::ivec2& new_dims)
    {
        m_framebuffer.resize(m_dims = new_dims);
        m_camera.resize(m_dims = new_dims);
    }

    void window::on_display()
    {
        /* TEMPORARY: recalculate sun light data here to pass to overlay.
         * later this could be done by e.g. asking m_skybox for it. */

        float sun_radius = 0.02f; // experimentally determined - radius of sun as viewed by camera

        std::vector<light> lights;
        lights.push_back(light(glm::vec4(skybox::calcSunDir(m_bar.Atmos.theta, m_bar.Atmos.phi), 0.0f),
                               m_bar.Atmos.sunColor,
                               glm::vec3(1, 1, 1), sun_radius, LIGHT_NORMAL, true));

        //lights.push_back(light(glm::vec4(10, 0, 5, 1), glm::vec3(0), 0.5f));

        // --- end of light precomputations ---

        m_framebuffer.bind();
        m_framebuffer.clear(true);

        m_skybox.display(m_camera,m_bar.Atmos);

        //m_lighthouse.display(m_camera, lights);
		//m_outbuilding.display(m_camera, lights);
		//m_terrain.display(m_camera, lights);
		//m_tree.display(m_camera, lights);

		//translate somehow
		m_light.display(m_camera, lights);

        #if !NO_LENS_FLARES
        const auto& occlusion = m_occlusion.query(
            lights, m_framebuffer, m_camera
        );

        m_aperture.render_flare(lights, occlusion, m_camera,
                                m_bar.lens_flare_intensity);

        m_aperture.render_ghosts(lights, occlusion, m_camera,
                                 m_bar.lens_flare_intensity,
                                 m_bar.lens_ghost_count,
                                 m_bar.lens_ghost_max_size,
                                 m_bar.lens_ghost_brightness);

        if (m_bar.lens_overlay) {
            m_overlay.render(lights, occlusion, m_camera,
                             m_bar.lens_reflectivity);
        }
        #endif

        m_framebuffer.render(m_bar.lens_exposure);

        TwDraw();

        glutSwapBuffers();
        m_fps.add_frame();
        if (m_fps.average_ready()) {
            const int period = (int)(20.0 * 60.0f);
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
        #if !NO_LENS_FLARES
        if (m_bar.lens_update_btn) {
            m_bar.lens_update_btn = false;

            m_aperture.load_aperture(m_bar.lens_aperture,
                                     m_bar.lens_aperture_f_number);
        }

        if (m_overlay.get_density() != m_bar.lens_density) {
            m_overlay.regenerate_film(m_bar.lens_density);
        }
        #endif

        float move_speed = m_bar.cam_move_speed / 60.0f;

        if (m_keys['w'])
            m_camera.move(glm::vec3(0.0f, 0.0f, -1.0f) * move_speed);
        if (m_keys['s'])
            m_camera.move(glm::vec3(0.0f, 0.0f, +1.0f) * move_speed);
        if (m_keys['a'])
            m_camera.move(glm::vec3(-1.0f, 0.0f, 0.0f) * move_speed);
        if (m_keys['d'])
            m_camera.move(glm::vec3(+1.0f, 0.0f, 0.0f) * move_speed);
        if (m_keys['c'])
            m_camera.move(glm::vec3(0.0f, -1.0f, 0.0f) * move_speed);
        if (m_keys[' '])
            m_camera.move(glm::vec3(0.0f, +1.0f, 0.0f) * move_speed);

        if (m_cursor_locked) {
            glutWarpPointer(m_dims.x / 2, m_dims.y / 2);
            m_mouse.set_pos((glm::vec2)(m_dims / 2) / (float)m_dims.x);
        }

        m_camera.set_fov(glm::radians(m_bar.cam_fov));
        m_bar.cam_locked = m_cursor_locked;
        m_bar.refresh();

        if (m_keys[27 /* escape */]) {
            glutLeaveMainLoop();
            return;
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
            m_camera.turn(m_mouse.delta(mouse_pos) * m_bar.cam_sensitivity);
        }

        m_mouse.set_pos(mouse_pos);
    }
}
