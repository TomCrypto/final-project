#include "gui/bar.h"

#include <stdexcept>

namespace gui
{
    basic_bar::basic_bar(const std::string& name)
    {
        if (!(m_bar = TwNewBar((m_name = name).c_str())))
            throw std::runtime_error("failed to create tweakbar");
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

        TwAddVarRW(m_bar,
            "Rotation", TW_TYPE_FLOAT, &rotation,
            "min=0 max=360 step=1.0 help='Rotate the skeleton'");
        
        TwAddVarRW(m_bar,
            "Exposure", TW_TYPE_FLOAT, &exposure,
            "min=0.01 max=3 step=0.01 help='Tonemapping exposure'");

        TwDefine((m_name + " contained=true").c_str());
    }
}
