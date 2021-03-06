#include <easylogging.h>
_INITIALIZE_EASYLOGGINGPP

#include <cstdlib>
#include <cstdio>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#include "gui/window.h"

void setup_logger()
{
    #if defined(_WIN32)
    bool use_colors = false;
    #else
    bool use_colors = (isatty(STDOUT_FILENO) == 1);
    #endif

    const std::string& fail = use_colors ? "\x1b[31;1m" : "";
    const std::string& warn = use_colors ? "\x1b[33;1m" : "";
    const std::string& more = use_colors ? "\x1b[34;1m" : "";
    const std::string& bold = use_colors ? "\x1b[37;1m" : "";
    const std::string& off  = use_colors ? "\x1b[0m"    : "";

    el::Configurations conf;
    conf.setToDefault();
    conf.parseFromText("*GLOBAL:\n TO_FILE=false");
    conf.set(el::Level::Error, el::ConfigurationType::Format,
         fail + "%datetime{%H:%m:%s} FAIL [%fbase:%line]" + off + ": %msg");
    conf.set(el::Level::Warning, el::ConfigurationType::Format,
         warn + "%datetime{%H:%m:%s} WARN" + off + ": %msg");
    conf.set(el::Level::Trace, el::ConfigurationType::Format,
         more + "%datetime{%H:%m:%s} MORE" + off + ": %msg");
    conf.set(el::Level::Info, el::ConfigurationType::Format,
         bold + "%datetime{%H:%m:%s} INFO" + off + ": %msg");
    el::Loggers::reconfigureLogger("default", conf);
}

int main(int argc, char *argv[])
{
    _START_EASYLOGGINGPP(argc, argv);
    setup_logger(); /* config/etc. */
    LOG(INFO) << "Program starting.";

    gui::initialize(argc, argv);

    try {
        gui::window window("COMP 308 - Final Project", glm::ivec2(1280, 720));
        window.run(); /* Will return when the window is closed by the user. */
    } catch (...) {
        LOG(ERROR) << "Aborting.";
        gui::abort_handler();
        return EXIT_FAILURE;
    }

    gui::finalize();
    return EXIT_SUCCESS;
}
