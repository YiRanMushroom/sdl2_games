module;

#include <utility>

export module snakeGameConfig;

import ywl.prelude;
import json;

namespace application::config::snakeGame {
    export uint32_t sizeMap;

    export uint32_t moveInterval;

    export uint32_t foodGenerateInterval;

    export void loadSnakeGameSettings(const json &j) {
        sizeMap = j | jsonutil::get_or_default("snakeGame", "sizeMap", "value", 35);

        if (sizeMap % 2 == 0) {
            // sizeMap++;
        } else if (sizeMap < 5 || sizeMap > 51) {
            sizeMap = 35;
        }

        // moveInterval = j["snakeGame"]["moveInterval"]["value"] | jsonutil::get_or_default(500);
        moveInterval = j | jsonutil::get_or_default("snakeGame", "moveInterval", "value", 500);
        // foodGenerateInterval = j["snakeGame"]["foodGenerateInterval"]["value"] | jsonutil::get_or_default(2000);
        foodGenerateInterval = j | jsonutil::get_or_default("snakeGame", "foodGenerateInterval", "value", 2000);
    }

    export void saveSnakeGameSettings(json &j) {
        j["snakeGame"]["sizeMap"]["value"] = sizeMap;
        j["snakeGame"]["sizeMap"]["description"] =
                "sizeMap is the size of the map, it must be greater than 5, and less than 51, and must be an odd number. "
                "Otherwise a default number would be assigned";

        j["snakeGame"]["moveInterval"]["value"] = moveInterval;
        j["snakeGame"]["moveInterval"]["description"] =
                "moveInterval is the interval of the snake movement, it must be greater than 0. "
                "Otherwise a default number would be assigned";

        j["snakeGame"]["foodGenerateInterval"]["value"] = foodGenerateInterval;
        j["snakeGame"]["foodGenerateInterval"]["description"] =
                "foodGenerateInterval is the interval of the food generation, it must be greater than 0. "
                "Otherwise a default number would be assigned";
    }
}
