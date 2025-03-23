module;

#include <iostream>

export module applicationConfig;
import json;
import fileUtilities;
import applicationSharedState;
import snakeGameConfig;

namespace application::config {
    void loadUIStates(const json &j) {
        application::isDebugWindowVisible =
            j | jsonutil::get_or_default("ui", "isDebugWindowVisible", false);
                // j["ui"]["isDebugWindowVisible"] | jsonutil::get_or_default(false);
    }

    void saveUIStates(json &j) {
        j["ui"]["isDebugWindowVisible"] = application::isDebugWindowVisible;
    }

    export void loadApplicationSettings() {
        auto [ex, ifs] = openFileInput("saves/configs.json");

        auto j = jsonutil::parse_no_throw(ifs);

        loadUIStates(j);
        snakeGame::loadSnakeGameSettings(j);
    }

    export void saveApplicationSettings() {
        json j;
        snakeGame::saveSnakeGameSettings(j);
        saveUIStates(j);

        auto [ex, ofs] = openFileOutput("saves/configs.json");

        ofs << j.dump(4);
    }
}
