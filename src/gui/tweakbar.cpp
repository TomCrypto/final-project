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
		/* === atmospheric options === */
		
		Atmos.theta = 90.0f;
		Atmos.phi = 90.0f;

        TwAddVarRW(m_bar,
            "theta", TW_TYPE_FLOAT, &Atmos.theta,
            " label='theta' group='Atmospheric'"
            " min=0 max=90.2 step=0.05");

        TwAddVarRW(m_bar,
            "phi", TW_TYPE_FLOAT, &Atmos.phi,
            " label='phi' group='Atmospheric'"
            " min=0 max=180 step=0.05");

        /* === lens options === */

        lens_exposure = 0.18f;
        lens_overlay = true;
        lens_reflectivity = 0.35f;
        lens_density = 70;
        lens_flare_intensity = 25;
        lens_flare_f_number = 1.0f;
        lens_aperture = PENTAGON;
        lens_diff_spread = 0.69f;
        lens_update_btn = true;

        TwAddVarRW(m_bar,
            "lens_exposure", TW_TYPE_FLOAT, &lens_exposure,
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

        TwAddVarRW(m_bar,
            "lens_flare_intensity", TW_TYPE_FLOAT, &lens_flare_intensity,
            " label='Flare intensity' group='Lens'"
            " min=0 max=100 step=0.1"
            " help='Intensity of the lens flares'");

        TwAddVarRW(m_bar,
            "lens_flare_f_number", TW_TYPE_FLOAT, &lens_flare_f_number,
            " label='Flare f-number' group='Lens'"
            " min=1.0 max=3.5 step=0.01"
            " help='The f-number of the aperture'");

        TwAddSeparator(m_bar,
            "lens_sep",
            " group='Lens'");

        auto apertureType = TwDefineEnum("Aperture", nullptr, 0);

        TwAddVarRW(m_bar,
            "lens_aperture", apertureType, &lens_aperture,
            " label='Aperture' group='Lens'"
            " enum='0 {Pentagonal aperture},"
                   "1 {Dirty lens},"
                   "2 {Lens with scratch},"
                   "3 {Octagonal aperture},"
                   "4 {Custom aperture}'"
            " help='Lens flare aperture to use'");

        TwAddVarRW(m_bar,
            "lens_diff_spread", TW_TYPE_FLOAT, &lens_diff_spread,
            " label='Diffraction Spread' group='Lens'"
            " min=0.3 max=1.0 step=0.01"
            " help='Amount of spread in the lens diffraction'");

        TwAddButton(m_bar,
            "lens_update_btn", btn_cb, &lens_update_btn,
            " label='Update Aperture' group='Lens'");

        /* === camera options === */

        cam_move_speed = 3.5f;
        cam_sensitivity = 1.5f;
        cam_fov = 50;
        cam_locked = false;

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
            " min=10 max=70 step=0.1"
            " help='Camera field of view'");

        TwAddVarRO(m_bar,
            "cam_locked", TW_TYPE_BOOLCPP, &cam_locked,
            " label='Cursor Locked' group='Navigation'"
            " help='Right-click to lock or unlock cursor'");

        /* === general bar settings === */

        TwDefine((m_name + " contained=true").c_str());
        TwDefine((m_name + " size='220 360'").c_str());
    }
}
