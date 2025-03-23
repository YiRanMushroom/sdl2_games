module;

#include <SDL2/SDL.h>
#include <imgui.h>
#include <fstream>
#include <type_traits>

export module application;

import ywl.prelude;
import SDL2_EventListener;
import SDL2_ImGui_VM;
import appEventListener;
import applicationSharedState;
import fileUtilities;
import applicationConfig;
import fontHolder;
import SDL2_Utilities;
import IComponent;
import Buttons;
import imguiUtilities;
import applicationResources;
import appMenuLayer;
import applicationBasicLayers;
import freeTypeFont;

using std::make_unique;

export int application_main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    application::config::loadApplicationSettings();

    auto vm = application::SDL2_ImGUi_VM_Factory{}
            .title("ImGui + SDL2 Test")
            .position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)
            .size(1280, 720)
            .windowFlags(SDL_WINDOW_SHOWN)
            .rendererIndex(-1)
            .rendererFlags(SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
            .build();

    application::loadApplicationResources(vm);

    auto fontAtlas = FreeTypeFontHolder{"resources/fonts/Open_Sans/static/OpenSans-Regular.ttf", vm.getRenderer()};

    auto &&layers = vm.layers;

    layers.push_back(make_unique<application::ImGuiLayer>());

    layers.push_back(make_unique<application::AppMenuLayer>(vm));

    layers.push_back(make_unique<application::BasicFrameworkLayer>());

    while (application::isRunning) { // NOLINT
        vm.preEventListening();

        vm.handleEvents();

        vm.postEventListening();

        vm.render();

        SDL_RenderPresent(vm.getRenderer());

        vm.handleDeferredTasks();
    }

    layers.clear();

    application::unloadApplicationResources();

    application::config::saveApplicationSettings();

    return 0;
}
