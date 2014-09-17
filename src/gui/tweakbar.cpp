#include "gui/tweakbar.h"

#include <easylogging.h>
#include <FreeImage.h>
#include <stdexcept>

namespace gui
{
    basic_bar::basic_bar(const std::string& name)
    {
        if (!(m_bar = TwNewBar((m_name = name).c_str()))) {
            LOG(ERROR) << "TwNewBar failed";
            throw std::runtime_error("failed to create tweakbar");
        }
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

    main_bar::main_bar(const std::string& name) : basic_bar(name)
    {
        rotation = 0;
        exposure = 0.18f;
        cam_move_speed = 3.5f;
        cam_sensitivity = 1.5f;
        color = glm::vec3(0.75f, 0.25f, 0.25f);

        TwAddVarRW(m_bar,
            "Rotation", TW_TYPE_FLOAT, &rotation,
            " min=0 max=360 step=1.0 help='Rotate the skeleton'");

        TwAddVarRW(m_bar,
            "Exposure", TW_TYPE_FLOAT, &exposure,
            " min=0.01 max=3 step=0.01 help='Tonemapping exposure'");

        TwAddVarRW(m_bar,
            "Color", TW_TYPE_COLOR3F, &color,
            " colormode=hls");

        TwAddVarRW(m_bar,
            "Movement Speed", TW_TYPE_FLOAT, &cam_move_speed,
            " min=0.1 max=5 step=0.1 help='Movement speed (WASD)'"
            " group='Navigation'");

        TwAddVarRW(m_bar,
            "Camera Sensitivity", TW_TYPE_FLOAT, &cam_sensitivity,
            " min=0.01 max=2.5 step=0.01 help='Rotation sensitivity'"
            " group='Navigation'");

        TwDefine((m_name + " contained=true").c_str());
    }
}
