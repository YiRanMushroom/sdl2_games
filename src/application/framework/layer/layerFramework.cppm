module;

#include <AppMacros.h>
#include <SDL2/SDL.h>
#include <list>
#include <queue>

export module layerFramework;
export import workableLayer;

import ywl.prelude;
import SDL2_EventListener;
import IComponent;

using std::unique_ptr;
using std::list;
using std::function;
using std::runtime_error;
using std::vector;
using std::shared_ptr;

namespace application {
    export interface IMouseMotionAutoHandler {
        virtual ~IMouseMotionAutoHandler() = default;

        vector<shared_ptr<IMouseMotionComponent> > mouseMoveInteractableComponents;

        virtual bool onMouseMotion(const SDL_Event &event, bool handled,
                                   IVirtualMachineContextProvider &provider) {
            for (auto &component: mouseMoveInteractableComponents) {
                handled |= component->onMouseMotion(event.motion.x, event.motion.y, provider);
            }
            return handled;
        }

        void addMouseMoveInteractableComponent(shared_ptr<IMouseMotionComponent> component) {
            mouseMoveInteractableComponents.push_back(std::move(component));
        }
    };

    export interface IMouseButtonAutoHandler {
        virtual ~IMouseButtonAutoHandler() = default;

        vector<shared_ptr<IMouseButtonInteractableComponent> > mouseButtonInteractableComponents;

        virtual bool onMouseButtonDown(const SDL_Event &event, bool handled,
                                       IVirtualMachineContextProvider &provider) {
            for (auto &component: mouseButtonInteractableComponents) {
                handled |= component->onMouseButtonDown(event.button.x, event.button.y, event.button.button, provider);
            }
            return handled;
        }

        virtual bool onMouseButtonUp(const SDL_Event &event, bool handled,
                                     IVirtualMachineContextProvider &provider) {
            for (auto &component: mouseButtonInteractableComponents) {
                handled |= component->onMouseButtonUp(event.button.x, event.button.y, event.button.button, provider);
            }
            return handled;
        }

        void addMouseButtonInteractableComponent(shared_ptr<IMouseButtonInteractableComponent> component) {
            mouseButtonInteractableComponents.push_back(std::move(component));
        }
    };

    export interface IMouseWheelAutoHandler {
        virtual ~IMouseWheelAutoHandler() = default;

        vector<shared_ptr<IMouseWheelInteractableComponent> > mouseWheelInteractableComponents;

        virtual bool onMouseWheel(const SDL_Event &event, bool handled,
                                  IVirtualMachineContextProvider &provider) {
            for (auto &component: mouseWheelInteractableComponents) {
                handled |= component->onMouseWheel(event.wheel.x, event.wheel.y, event.wheel.direction, provider);
            }
            return handled;
        }

        void addMouseWheelInteractableComponent(shared_ptr<IMouseWheelInteractableComponent> component) {
            mouseWheelInteractableComponents.push_back(std::move(component));
        }
    };

    export interface IRenderableAutoHandler {
        virtual ~IRenderableAutoHandler() = default;

        vector<shared_ptr<IRenderableComponent> > renderableComponents;

        virtual void render(IVirtualMachineContextProvider &provider) {
            for (auto &component: renderableComponents) {
                component->onRender(provider);
            }
        }

        void addRenderableComponent(shared_ptr<IRenderableComponent> component) {
            renderableComponents.push_back(std::move(component));
        }
    };

    export interface IBasicMouseAutoHandler extends IMouseMotionAutoHandler, IMouseButtonAutoHandler {};

    export interface IWindowEventAutoHandler {
        virtual ~IWindowEventAutoHandler() = default;

        vector<shared_ptr<IWindowEventComponent> > windowSizeChangedComponents;

        virtual bool onWindowEvent(const SDL_Event &event, bool handled, IVirtualMachineContextProvider &provider) {
            if (event.type != SDL_WINDOWEVENT || event.window.type != SDL_WINDOWEVENT_SIZE_CHANGED) {
                return handled;
            }

            for (auto &component: windowSizeChangedComponents) {
                handled |= component->onWindowSizeChanged(event.window.data1, event.window.data2, provider);
            }

            return handled;
        }

        void addWindowSizeChangedComponent(shared_ptr<IWindowEventComponent> component) {
            windowSizeChangedComponents.push_back(std::move(component));
        }
    };

    export class BasicMouseHandledLayer
            : public WorkableLayer,
              public IBasicMouseAutoHandler,
              public IRenderableAutoHandler,
              public IWindowEventAutoHandler {
    public:
        ~BasicMouseHandledLayer() override = default;

        // using IBasicMouseAutoHandler::onMouseButtonDown;
        virtual bool onMouseButtonDown(const SDL_Event &event, bool original,
                                       IVirtualMachineContextProvider &provider) override {
            return IBasicMouseAutoHandler::onMouseButtonDown(event, original, provider);
        }

        // using IBasicMouseAutoHandler::onMouseButtonUp;
        virtual bool onMouseButtonUp(const SDL_Event &event, bool original,
                                     IVirtualMachineContextProvider &provider) override {
            return IBasicMouseAutoHandler::onMouseButtonUp(event, original, provider);
        }

        // using IBasicMouseAutoHandler::onMouseMotion;
        virtual bool onMouseMotion(const SDL_Event &event, bool original, IVirtualMachineContextProvider &
                                   provider) override {
            return IBasicMouseAutoHandler::onMouseMotion(event, original, provider);
        }

        // using IRenderableAutoHandler::render;
        virtual void render(IVirtualMachineContextProvider &provider) override {
            return IRenderableAutoHandler::render(provider);
        }

        // using IWindowEventAutoHandler::onWindowEvent;
        virtual bool onWindowEvent(const SDL_Event &event, bool original,
                                   IVirtualMachineContextProvider &provider) override {
            return IWindowEventAutoHandler::onWindowEvent(event, original, provider);
        }
        void registerBasicMouseRenderableComponents(const auto &components) {
            addMouseMoveInteractableComponent(components);
            addMouseButtonInteractableComponent(components);
            addRenderableComponent(components);
            addWindowSizeChangedComponent(components);
        }
    };
}
