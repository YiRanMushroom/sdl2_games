module;

#include <iostream>

export module applicationConfig;
import json;
import fileUtilities;
import applicationSharedState;
import std_overloads;
import snakeGameConfig;

namespace application {
    void loadUIStates(json &j) {
        application::isDebugWindowVisible =
                j["ui"]["isDebugWindowVisible"] | jsonutil::get_or_default(true);
    }

    void saveUIStates(json &j) {
        j["ui"]["isDebugWindowVisible"] = application::isDebugWindowVisible;
    }

    export void loadApplicationSettings() {
        auto [ex, ifs] = openFileInput("saves/configs.json");

        auto j = jsonutil::parse_no_throw(ifs);

        loadUIStates(j);
        loadSnakeGameSettings(j);
    }

    export void saveApplicationSettings() {
        json j;
        saveSnakeGameSettings(j);
        saveUIStates(j);

        auto [ex, ofs] = openFileOutput("saves/configs.json");

        ofs << j.dump(4);
    }
}
