/* Purpose:
 *
 *  - very simple wrapper around the AntTweakBar GUI library
 *
 * This class contains all our program parameters, which are directly modified
 * by the AntTweakBar library and can then be read directly each frame through
 * the m_bar instance in the window class.
*/

#ifndef GUI_TWEAKBAR_H
#define GUI_TWEAKBAR_H

#include <AntTweakBar.h>
#include <glm/glm.hpp>

// TODO: move those parameter structs/types into a types.h header?

#include "core/skybox.h"
#include "core/aperture.h"

#include <string>

namespace gui
{
    // Basic AntTweakBar bar which takes care of boilerplate stuff
    // Do not instantiate it, please extend main_bar further below
    class basic_bar
    {
    public:
        basic_bar(const std::string& name);
        ~basic_bar();

        void refresh();

    protected:
        TwBar* m_bar;
        std::string m_name;

        basic_bar& operator=(const basic_bar& other);
        basic_bar(const basic_bar& other);
    };

    // The main configuration bar for the program, just add more
    // members and create/initialize them in the constructor
    class main_bar : public basic_bar
    {
    public:
        main_bar(const std::string& name,
                 const std::string& title);

        /* === atmospheric options === */

		atmos Atmos;

        /* === lens options === */

        float lens_exposure;
        int lens_density;
        float lens_reflectivity;
        bool lens_overlay;
        float lens_flare_intensity;
        int lens_ghost_count;
        float lens_ghost_max_size;
        float lens_ghost_brightness;
        transmission_function lens_aperture;
        float lens_aperture_f_number;
        bool lens_update_btn;

        /* === camera options === */

        float cam_move_speed;
        float cam_sensitivity;
        bool cam_locked;
        float cam_fov;

        glm::vec3 light_pos;
        glm::vec3 light_pos_2;
    };
}

#endif
