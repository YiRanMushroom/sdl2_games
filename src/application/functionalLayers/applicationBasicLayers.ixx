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
import applicationConstants;
import imguiWindows;

namespace application {
    export class BasicFrameworkLayer extends public WorkableLayer {
    public:
        static inline SDL_Color backGroundColor = application::constants::default_background_color;

        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider &provider) override {
            if (event.type == SDL_QUIT) {
                application::isRunning = false;
                return prevHandled;
            }
            return prevHandled;
        }

        void render(IVirtualMachineContextProvider &provider) override {
            SDL_SetRenderDrawColor(provider.getRenderer(), backGroundColor.r,
                                   backGroundColor.g, backGroundColor.b,
                                   backGroundColor.a);
            SDL_RenderClear(provider.getRenderer());
        }
    };

    export class ImGuiLayer extends public WorkableLayer {
    public:
        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider &) override {
            bool handled = ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_F9:
                        application::isDebugWindowVisible = !application::isDebugWindowVisible;
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

        void render(IVirtualMachineContextProvider &provider) override {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if (application::isDebugWindowVisible) {
                ShowDebugWindow(&application::isDebugWindowVisible);
            }

            // ImGui Layer
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), provider.getRenderer());
        }
    };
}
