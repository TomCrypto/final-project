#include <easylogging.h>

#include "gui/tweakbar.h"

#include <FreeImage.h>
#include <stdexcept>

namespace gui
{
    // Use this for button callbacks, pass the bool
    static void TW_CALL btn_cb(void *user_data)
    {
        *((bool*)user_data) = true;
    }

    basic_bar::basic_bar(const std::string& name,
                         const std::string& title)
    {
        if (!(m_bar = TwNewBar((m_name = name).c_str()))) {
            LOG(ERROR) << "Failed to create tweakbar.";
            LOG(TRACE) << "TwNewBar failed.";
            throw std::runtime_error("");
        }
        
        set_title(title);
    }

    basic_bar::~basic_bar()
    {
        TwDeleteBar(m_bar);
    }

    void basic_bar::set_title(const std::string& title)
    {
        TwDefine((m_name + " label='" + title + "'").c_str());
    }

    void basic_bar::refresh()
    {
        TwRefreshBar(m_bar);
    }

    main_bar::main_bar(const std::string& name,
                       const std::string& title)
      : basic_bar(name, title)
    {
        rotation = 0;
        exposure = 0.18f;
        
        lens_overlay = true;
        lens_reflectivity = 0.35f;
        lens_density = 70;
        
        cam_move_speed = 3.5f;
        cam_sensitivity = 1.5f;
        cam_fov = 50;
		skycolor = glm::vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131);
		color1 = glm::vec3(0.75f, 0.25f, 0.25f);
		color2 = glm::vec3(0.75f, 0.25f, 0.25f);
		color3 = glm::vec3(0.75f, 0.25f, 0.25f);
		color4 = glm::vec3(0.75f, 0.25f, 0.25f);
		Atmos.light = glm::vec3(0,-1,0);
        aperture_regen_btn = false;
		Atmos.theta = 90.0f;
		Atmos.phi = 90.0f;

        TwAddVarRW(m_bar,
            "theta", TW_TYPE_FLOAT, &Atmos.theta,
            " label='theta' group='Atmos' min=0 max=180 step=0.1");

        TwAddVarRW(m_bar,
            "phi", TW_TYPE_FLOAT, &Atmos.phi,
            " label='phi' group='Atmos' min=0 max=180 step=0.1");

		TwAddVarRW(m_bar,
			"rayM", TW_TYPE_FLOAT, &Atmos.RayMult,
			" label='Rayleigh Mult' group='Atmos'");
		TwAddVarRW(m_bar,
			"mieM", TW_TYPE_FLOAT, &Atmos.MieMult,
			" label='Mie Mult' group='Atmos'");
		TwAddVarRW(m_bar,
			"inM", TW_TYPE_FLOAT, &Atmos.InMult,
			" label='Inscattering Mult' group='Atmos'");

        TwAddVarRW(m_bar,
            "exposure", TW_TYPE_FLOAT, &exposure,
            " label='Exposure' group='Lens'"
            " min=0.01 max=3 step=0.01"
            " help='Exposure used for tonemapping'");

        TwAddVarRW(m_bar,
            "lens_density", TW_TYPE_INT32, &lens_density,
            " label='Imperfections' group='Lens'"
            " min=0 max=200"
            " help='Amount of lens imperfections'");

        TwAddVarRW(m_bar,
            "lens_reflectivity", TW_TYPE_FLOAT, &lens_reflectivity,
            " label='Reflectivity' group='Lens'"
            " min=0 max=1 step=0.01"
            " help='How reflective the lens coating is'");

        TwAddVarRW(m_bar,
            "lens_overlay", TW_TYPE_BOOLCPP, &lens_overlay,
            " label='Overlay' group='Lens'"
            " help='Whether the lens dirt/imperfections are rendered'");

        // add lens flare settings

        TwAddButton(m_bar, "btn", btn_cb, &aperture_regen_btn,
            " label='New Aperture' group='Lens'");

		/*TwAddVarRW(m_bar,
			"sky_color", TW_TYPE_COLOR3F, &skycolor,
			" label='Sky Color' colormode=hls");*/
		/*TwAddVarRW(m_bar,
			"color1", TW_TYPE_COLOR3F, &color1,
			" label='Color1' colormode=hls");
		TwAddVarRW(m_bar,
			"color2", TW_TYPE_COLOR3F, &color2,
			" label='Color2' colormode=hls");
		TwAddVarRW(m_bar,
			"color3", TW_TYPE_COLOR3F, &color3,
			" label='Color3' colormode=hls");
		TwAddVarRW(m_bar,
			"color4", TW_TYPE_COLOR3F, &color4,
			" label='Color4' colormode=hls");*/

        TwAddVarRW(m_bar,
            "cam_move_speed", TW_TYPE_FLOAT, &cam_move_speed,
            " label='Speed' group='Navigation'"
            " min=0.1 max=5 step=0.1"
            " help='Movement speed (WASD)'");

        TwAddVarRW(m_bar,
            "cam_sensitivity", TW_TYPE_FLOAT, &cam_sensitivity,
            " label='Sensitivity' group='Navigation'"
            " min=0.01 max=2.5 step=0.01"
            " help='Rotation sensitivity'");

        TwAddVarRW(m_bar,
            "cam_fov", TW_TYPE_FLOAT, &cam_fov,
            " label='Field of View' group='Navigation'"
            " min=10 max=90 step=0.1"
            " help='Camera field of view'");

        TwAddVarRO(m_bar,
            "cam_locked", TW_TYPE_BOOLCPP, &cam_locked,
            " label='Cursor Locked' group='Navigation'"
            " help='Right-click to lock or unlock cursor'");

        TwDefine((m_name + " contained=true").c_str());
    }
}
