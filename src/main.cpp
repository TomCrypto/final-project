#include <stdexcept>
#include <cstdlib>
#include <cstdio>

#include "gui/window.h"

int main(int argc, char *argv[])
{
    try
    {
        gui::window::initialize(argc, argv);
        gui::window prog("COMP 308 - Final Project",
                         std::make_pair(1024, 768));

        prog.run();
    }
    catch (const std::exception& e)
    {
        printf("==== ERROR ====\n");
        printf(">> %s\n", e.what());
        printf("===============\n");
        return EXIT_FAILURE;
    }
    catch (...)
    {
        printf("==== ERROR ====\n");
        printf(">> Fatal error!\n");
        printf("===============\n");
        return EXIT_FAILURE;
    }

    gui::window::finalize();
    return EXIT_SUCCESS;
}
