module;

#include <AppMacros.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

export module applicationBasicLayers;

import applicationResources;
import layerFramework;
import applicationSharedState;
import imguiUtilities;
import std_overloads;

namespace application {
    export class BasicFrameworkLayer extends public WorkableLayer {
        public:
        static inline Uint8 r = 63;
        static inline Uint8 g = 63;
        static inline Uint8 b = 63;
        static inline Uint8 a = 255;

        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider&) override {
            if (event.type == SDL_QUIT) {
                application::isRunning = false;
                return true;
            }
            return prevHandled;
        }

        void render(IVirtualMachineContextProvider& provider) override {
            SDL_SetRenderDrawColor(provider.getRenderer(), r, g, b, a);
            SDL_RenderClear(provider.getRenderer());
        }
    };

    export class ImGuiLayer extends public WorkableLayer {
    public:
        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider&) override {
            bool handled = ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_F9:
                        application::isDemoWindowVisible = !application::isDemoWindowVisible;
                    handled = true;
                    break;

                    case SDLK_F10:
                        application::ignoreImGuiEventHandlingResult = !application::ignoreImGuiEventHandlingResult;
                    AppLogMessage("Ignore ImGui Event Handling Result: {}"_fmt(
                        application::ignoreImGuiEventHandlingResult));
                    handled = true;
                    break;

                    default:
                        break;
                }
            }

            return application::ignoreImGuiEventHandlingResult ? prevHandled : handled;
        }

        void render(IVirtualMachineContextProvider& provider) override {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if (application::isDemoWindowVisible) {
                ImGui::ShowDemoWindow(&application::isDemoWindowVisible);
            }

            // ImGui Layer
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), provider.getRenderer());
        }
    };
}