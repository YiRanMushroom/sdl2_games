module;

#include <utility>

export module snakeGameConfig;

import std_essentials;
import json;

namespace application {
    export uint32_t snakeSizeMap;

    export void loadSnakeGameSettings(json &j) {
        snakeSizeMap = j["snakeGame"]["sizeMap"] | jsonutil::get_or_default(35);
        if (snakeSizeMap % 2 == 0) {
            snakeSizeMap++;
        } else if (snakeSizeMap < 5 || snakeSizeMap > 51) {
            snakeSizeMap = 35;
        }
    }

    export void saveSnakeGameSettings(json &j) {
        j["snakeGame"]["sizeMap"] = snakeSizeMap;
        j["snakeGame"]["description"] =
                "sizeMap is the size of the map, it must be greater than 5, and less than 51, and must be an odd number. "
                "Otherwise a default number would be assigned";
    }
}
