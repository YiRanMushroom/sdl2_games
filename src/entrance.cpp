#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int application_main(int argc, char *argv[]);

int SDL_main(int argc, char *argv[]) {
    return application_main(argc, argv);
}
