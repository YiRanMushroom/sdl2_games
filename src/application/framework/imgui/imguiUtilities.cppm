module;

#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>

export module imguiUtilities;
import std_essentials;
import std_overloads;

namespace ImGui {
    // export void Log(string_view message) {
    //     ImGui::LogText("%s\n", message.data());
    // }
}

export void AppLogMessage(string_view message) {
    IMGUI_DEBUG_LOG("%s\n", message.data());
    // std::cout << message << std::endl;
}
