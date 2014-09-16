#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "gui/program.h"

int main(int argc, char *argv[])
{
    try
    {
        gui::program::initialize(argc, argv);
        gui::program prog("COMP 308 - Final Project",
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

    gui::program::finalize();
    return EXIT_SUCCESS;
}