module;

#include <SDL2/SDL.h>

export module applicationConstants;

namespace application::constants {
    export constexpr int default_font_size = 50;
    export constexpr SDL_Color default_font_color = {191, 191, 191, 255};
    export constexpr SDL_Color default_background_color = {63, 63, 63, 255};
    export constexpr SDL_Color default_button_color = {63, 63, 63, 255};
}
