module;

#include <imgui.h>
#include <utility>

export module imguiWindows;

import std_essentials;
import applicationSharedState;

namespace application {
    export unordered_map<string, std::pair<bool *, function<void(bool *)> > > imguiDisplayWindows;

    struct ImGuiDisplayWindowDeleter {
        static void operator()(const string *name) {
            imguiDisplayWindows.erase(*name);
            delete name;
        }
    };

    export shared_ptr<string> addImGuiDisplayWindow(string name, bool *p_open, function<void(bool *)> display) {
        auto returnPtr = shared_ptr<string>(new string(std::move(name)), ImGuiDisplayWindowDeleter());
        imguiDisplayWindows[*returnPtr] = {p_open, std::move(display)};
        return returnPtr;
    }

    export void ShowDebugWindow(bool *p_open) {
        ImGui::Begin("Debug", p_open);
        static bool showDebugLogWindow = false;

        ImGui::Checkbox("Show Debug Log Window", &showDebugLogWindow);

        ImGui::Checkbox("Ignore ImGui Event Handling Result", &application::ignoreImGuiEventHandlingResult);

        for (auto &[name, pair]: imguiDisplayWindows) {
            auto &[p_open, display] = pair;
            ImGui::Checkbox(name.c_str(), p_open);
        }

        ImGui::End();

        if (showDebugLogWindow) {
            ImGui::ShowDebugLogWindow(&showDebugLogWindow);
        }

        for (auto &[name, pair]: imguiDisplayWindows) {
            auto &[p_open, display] = pair;
            if (*p_open) {
                ImGui::Begin(name.c_str(), p_open);
                display(p_open);
                ImGui::End();
            }
        }
    }
}
