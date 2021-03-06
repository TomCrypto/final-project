#include <easylogging.h>
#include "gui/tweakbar.h"
#include <stdexcept>

static void TW_CALL sun_timeofday_scb(const void *value, void *user_data)
{
    atmosphere* vars = (atmosphere*)user_data;
    vars->timeofday = *(float*)value;
    vars->sunColor = skybox::calcSunColor(vars->timeofday, vars->turbidity);
}

static void TW_CALL sun_timeofday_gcb(void *value, void *user_data)
{
    atmosphere* vars = (atmosphere*)user_data;
    *(float*)value = vars->timeofday;
}

namespace gui
{
    // Use this for button callbacks, pass the bool
    static void TW_CALL btn_cb(void *user_data)
    {
        *((bool*)user_data) = true;
    }

    basic_bar::basic_bar(const std::string& name)
    {
        if (!(m_bar = TwNewBar((m_name = name).c_str()))) {
            LOG(ERROR) << "Failed to create tweakbar.";
            LOG(TRACE) << "TwNewBar failed.";
            throw std::runtime_error("");
        }
    }

    basic_bar::~basic_bar()
    {
        TwDeleteBar(m_bar);
    }

    void basic_bar::refresh()
    {
        TwRefreshBar(m_bar);
    }

    main_bar::main_bar(const std::string& name,
                       const std::string& title) :
        basic_bar(name)
    {
        /* === atmospheric options === */

        atmos_vars.timeofday = 8.5f;
        atmos_vars.ray = 3500.0f;
        atmos_vars.mie = 2.3f;
        atmos_vars.extinction = 0.05f;
        atmos_vars.turbidity = 2.0f;
        atmos_vars.sunBrightness = 10000.0f;
        sun_timeofday_scb(&atmos_vars.timeofday, &atmos_vars);

        TwAddVarRW(m_bar,
            "rayleigh_multiplier", TW_TYPE_FLOAT, &atmos_vars.ray,
            " label='Rayleigh Multiplier' group='Atmospheric'"
            " min=1 step=10");
        TwAddVarRW(m_bar,
            "mie_multiplier", TW_TYPE_FLOAT, &atmos_vars.mie,
            " label='Mie Multiplier' group='Atmospheric'"
            " min=0.1 step=0.05");
        TwAddVarRW(m_bar,
            "extinction", TW_TYPE_FLOAT, &atmos_vars.extinction,
            " label='Extinction' group='Atmospheric'"
            " min=0.01 max=1 step=0.01");

        TwAddVarRW(m_bar,
            "turbidity", TW_TYPE_FLOAT, &atmos_vars.turbidity,
            " label='Turbidity' group='Atmospheric'"
            " min=1.3 max=4 step=0.01");

        TwAddVarRW(m_bar,
            "sunBrightness", TW_TYPE_FLOAT, &atmos_vars.sunBrightness,
            " label='Sun Brightness' group='Atmospheric'"
            " min=500 max=30000 step=10");

        TwAddVarCB(m_bar,
            "timeofday", TW_TYPE_FLOAT,
            sun_timeofday_scb, sun_timeofday_gcb, &atmos_vars,
            " label='Time of Day' group='Atmospheric'"
            " min=6 max=18 step=0.01");

        TwAddVarRW(m_bar,
            "sunColor", TW_TYPE_COLOR3F, &atmos_vars.sunColor,
            " label='Sun Color' group='Atmospheric'"
            " colormode=hls");

        /* === lens options === */

        lens_exposure = 0.18f;
        lens_overlay = true;
        lens_reflectivity = 0.05f;
        lens_density = 150;
        lens_flare_intensity = 15;
        lens_ghost_count = 125;
        lens_ghost_max_size = 0.2f;
        lens_ghost_brightness = 0.25f;
        lens_aperture = GRAZED;
        lens_aperture_f_number = 1.5f;
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

        TwAddSeparator(m_bar,
            "lens_sep1",
            " group='Lens'");

        TwAddVarRW(m_bar,
            "lens_ghost_count", TW_TYPE_INT32, &lens_ghost_count,
            " label='Ghost count' group='Lens'"
            " min=0 max=500"
            " help='The number of lens flare ghosts'");

        TwAddVarRW(m_bar,
            "lens_ghost_max_size", TW_TYPE_FLOAT, &lens_ghost_max_size,
            " label='Ghost max size' group='Lens'"
            " min=0.05 max=0.3 step=0.01"
            " help='The maximum ghost radius'");

        TwAddVarRW(m_bar,
            "lens_ghost_brightness", TW_TYPE_FLOAT, &lens_ghost_brightness,
            " label='Ghost brightness' group='Lens'"
            " min=0.01 max=0.75 step=0.01"
            " help='The intensity of the ghosts'");

        TwAddSeparator(m_bar,
            "lens_sep2",
            " group='Lens'");

        auto apertureType = TwDefineEnum("transmission_function", nullptr, 0);

        TwAddVarRW(m_bar,
            "lens_aperture", apertureType, &lens_aperture,
            " label='Aperture' group='Lens'"
            " enum='0 {Pentagonal aperture},"
            "1 {Lens with fingerprints},"
            "2 {Grazed lens},"
            "3 {Scratched lens},"
            "4 {Human eye},"
            "5 {Custom aperture}'"
            " help='Lens flare aperture to use'");

        TwAddVarRW(m_bar,
            "lens_aperture_f_number", TW_TYPE_FLOAT, &lens_aperture_f_number,
            " label='F-number' group='Lens'"
            " min=1.0 max=8.0 step=0.1"
            " help='The f-number of the aperture'");

        TwAddButton(m_bar,
            "lens_update_btn", btn_cb, &lens_update_btn,
            " label='Update Aperture' group='Lens'");

        /* === camera options === */

        cam_move_speed = 8.0f;
        cam_sensitivity = 1.5f;
        cam_fov = 60;
        cam_locked = false;

        TwAddVarRW(m_bar,
            "cam_move_speed", TW_TYPE_FLOAT, &cam_move_speed,
            " label='Speed' group='Navigation'"
            " min=0.1 max=25 step=0.1"
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

        TwDefine((m_name + " label='" + title + "'").c_str());
        TwDefine((m_name + " contained=true").c_str());
        TwDefine((m_name + " size='220 450'").c_str());
    }

    debug_bar::debug_bar(const std::string& name,
        const std::string& title) : basic_bar(name) {
        translateLight = glm::vec3(11.01,18.7,23.72);
        TwAddVarRW(m_bar,
            "translateLight", TW_TYPE_DIR3F, &translateLight,
            " label='Lamp Position'");

        /* === general bar settings === */

        TwDefine((m_name + " label='" + title + "'").c_str());
        TwDefine((m_name + " contained=true").c_str());
        TwDefine((m_name + " iconified=true").c_str());
        TwDefine((m_name + " size='220 300'").c_str());
    }
}
