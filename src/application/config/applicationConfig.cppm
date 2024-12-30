module;

#include <iostream>

export module applicationConfig;
import json;
import fileUtilities;
import applicationSharedState;
import std_overloads;


namespace application {
    void loadUIStates() {
        auto [ex, ifs] = openFileInput("saves/uiStates.json");

        auto j = jsonutil::parse_no_throw(ifs);

        application::isDemoWindowVisible =
                j["isDemoWindowVisible"] | jsonutil::get_or_default(true);
    }


    void saveUIStates() {
        json j;
        j["isDemoWindowVisible"] = application::isDemoWindowVisible;

        auto [ex, ofs] = openFileOutput("saves/uiStates.json");

        ofs << j.dump(4);
    }

    export void loadApplicationSettings() {
        loadUIStates();
    }

    export void saveApplicationSettings() {
        saveUIStates();
    }
}