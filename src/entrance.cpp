#include <SDL2/SDL.h>

import application;
import ywl.app.vm;

int SDL_main(int argc, char *argv[]) {
    return ywl::app::vm::run<application_main>(argc, argv);
}
