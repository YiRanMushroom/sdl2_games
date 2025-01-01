module;

#include <SDL2/SDL_mixer.h>
#include <cassert>

export module applicationResources;

import std_essentials;
import fontHolder;
import freeTypeFont;
import workableLayer;

namespace application {
    export unique_ptr<FontHolder> openSansHolder;

    export unordered_map<string, SharedMixChunk> mixChunks;

    export unique_ptr<FreeTypeFontHolder> freeTypeFontHolder;

    export void loadApplicationResources(const IVirtualMachineContextProvider &provider) {
        openSansHolder = make_unique<FontHolder>("resources/fonts/Open_Sans/static/OpenSans-Regular.ttf");
        mixChunks["click"] = SharedMixChunk{Mix_LoadWAV("resources/sound_effects/click.ogg")};
        assert(mixChunks["click"]);
        mixChunks["snake/eat"] = SharedMixChunk{Mix_LoadWAV("resources/sound_effects/snake/eat.ogg")};
        assert(mixChunks["snake/eat"]);
        mixChunks["snake/game_over"] = SharedMixChunk{Mix_LoadWAV("resources/sound_effects/snake/game_over.wav")};
        assert(mixChunks["snake/game_over"]);
        mixChunks["snake/success"] = SharedMixChunk{Mix_LoadWAV("resources/sound_effects/snake/success.wav")};
        assert(mixChunks["snake/success"]);

        freeTypeFontHolder = make_unique<FreeTypeFontHolder>("resources/fonts/Open_Sans/static/OpenSans-Regular.ttf",
                                                             provider.getRenderer());
    }

    export void unloadApplicationResources() {
        openSansHolder.reset();

        freeTypeFontHolder.reset();
        Mix_HaltChannel(-1);
        Mix_HaltMusic();
        mixChunks.clear();
    }
}
