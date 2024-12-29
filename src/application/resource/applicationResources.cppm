module;

export module applicationResources;

import std_essentials;
import fontHolder;

namespace application {
    export unique_ptr<FontHolder> openSansHolder;

    export void loadApplicationResources() {
        openSansHolder = make_unique<FontHolder>("resources/fonts/Open_Sans/static/OpenSans-Regular.ttf");
    }

    export void unloadApplicationResources() {
        openSansHolder.reset();
    }
}
