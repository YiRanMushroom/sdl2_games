module;

#include <AppMacros.h>
#include <SDL2/SDL.h>
#include <list>
#include <queue>

export module layerFramework;
export import workableLayer;

import std_essentials;
import SDL2_EventListener;
import IComponent;

namespace application {
    export class MouseComponentAutoHandledLayer extends public WorkableLayer {
    protected:
        vector<unique_ptr<application::IMouseInteractableComponent> > mouseInteractableComponents;

    public:
        bool onMouseMotion(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            for (auto &component: mouseInteractableComponents) {
                original |= component->onMouseMoved(event.motion.x, event.motion.y, provider);
            }

            return original;
        }

        bool onMouseButtonDown(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            if (original) {
                return original;
            }

            for (auto &component: mouseInteractableComponents) {
                original |= component->onMouseButtonDown(event.button.x, event.button.y, event.button.button, provider);
            }

            return original;
        }

        bool onMouseButtonUp(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            for (auto &component: mouseInteractableComponents) {
                original |= component->onMouseButtonUp(event.button.x, event.button.y, event.button.button, provider);
            }

            return original;
        }

        bool onMouseWheel(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            if (original) {
                return original;
            }

            for (auto &component: mouseInteractableComponents) {
                original |= component->onMouseWheel(event.wheel.x, event.wheel.y, event.wheel.direction, provider);
            }

            return original;
        }

        bool onWindowEvent(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            if (event.type != SDL_WINDOWEVENT || event.window.type != SDL_WINDOWEVENT_SIZE_CHANGED) {
                return original;
            }

            for (auto &component: mouseInteractableComponents) {
                original |= component->onWindowSizeChanged(event.window.data1, event.window.data2, provider);
            }

            return original;
        }

        void render(IVirtualMachineContextProvider& provider) override {
            for (auto &component: mouseInteractableComponents) {
                component->onRender(provider);
            }
        }
    };
}
