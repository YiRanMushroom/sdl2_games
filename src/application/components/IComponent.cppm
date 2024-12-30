module;

#include <SDL2/SDL.h>
#include <AppMacros.h>

export module IComponent;
import workableLayer;

namespace application {
    export interface IWindowEventComponent {
        virtual ~IWindowEventComponent() = default;

        virtual bool onWindowSizeChanged(int, int, IVirtualMachineContextProvider &) {
            return false;
        }
    };

    export interface IRenderableComponent {
        virtual ~IRenderableComponent() = default;

        virtual void onRender(IVirtualMachineContextProvider &) {}
    };

    export interface IMouseMotionComponent {
        virtual ~IMouseMotionComponent() = default;

        virtual bool onMouseMotion(int, int, IVirtualMachineContextProvider &) {
            return false;
        }
    };

    export interface IMouseButtonInteractableComponent {
        virtual ~IMouseButtonInteractableComponent() = default;

        virtual bool onMouseButtonDown(int, int, int, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseButtonUp(int, int, int, IVirtualMachineContextProvider &) {
            return false;
        }
    };

    export interface IMouseWheelInteractableComponent {
        virtual ~IMouseWheelInteractableComponent() = default;

        virtual bool onMouseWheel(int, int, unsigned int, IVirtualMachineContextProvider &) {
            return false;
        }
    };

    export interface IMouseInteractableComponent extends IMouseMotionComponent,
                                                         IMouseButtonInteractableComponent,
                                                         IMouseWheelInteractableComponent {};

    export interface IMouseInteractableWindowSizeChangedRenderableComponent
            extends IMouseInteractableComponent, IWindowEventComponent, IRenderableComponent {
    public:
        virtual ~IMouseInteractableWindowSizeChangedRenderableComponent() = default;

        virtual bool onMouseMotion(int, int, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseButtonDown(int, int, int, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseButtonUp(int, int, int, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseWheel(int, int, unsigned int, IVirtualMachineContextProvider &) {
            return false;
        }
    };
}
