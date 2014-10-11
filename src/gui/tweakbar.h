// extremely simple wrapper around an AntTweakBar bar instance

#ifndef GUI_TWEAKBAR_H
#define GUI_TWEAKBAR_H

#include <AntTweakBar.h>
#include <glm/glm.hpp>

#include "core/skybox.h"
#include "core/aperture.h"

#include <string>

namespace gui
{
    // Basic AntTweakBar bar which takes care of boilerplate stuff
    // Do not instantiate it, rather extend main_bar further below
    class basic_bar
    {
    public:
        basic_bar(const std::string& name,
                  const std::string& title);
        ~basic_bar();

        void set_title(const std::string& title);

        void refresh();

    protected:
        TwBar* m_bar;
        std::string m_name;

        basic_bar& operator=(const basic_bar& other);
        basic_bar(const basic_bar& other);
    };

    // The main configuration bar for the program, just add more
    // members and initialize them in the constructor
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
        float lens_flare_f_number;
        int lens_ghost_count;
        float lens_ghost_max_size;
        transmission_function lens_aperture;
        float lens_diff_spread;
        bool lens_update_btn;

        /* === camera options === */

        float cam_move_speed;
        float cam_sensitivity;
        bool cam_locked;
        float cam_fov;
    };
}

#endif
