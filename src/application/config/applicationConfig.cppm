module;

#include <iostream>

export module applicationConfig;
import json;
import fileUtilities;
import applicationSharedState;
import std_overloads;

void loadUIStates() {
    auto [ex, ifs] = openFileInput("saves/uiStates.json");

    auto j = jsonutil::parse_no_throw(ifs);

    application::isDemoWindowVisible =
            j | jsonutil::read_or_default("isDemoWindowVisible", true);
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
