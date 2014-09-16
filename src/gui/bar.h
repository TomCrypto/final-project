#ifndef GUI_BAR_H
#define GUI_BAR_H

#include <AntTweakBar.h>
#include <string>

namespace gui
{
    // Basic AntTweakBar bar which takes care of boilerplate stuff
    // Do not instantiate it, rather extend main_bar further below
    class basic_bar
    {
    public:
        basic_bar(const std::string& name);
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
        main_bar(const std::string& name);

        float rotation;
        float exposure;
    };
}

#endif
