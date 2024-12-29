module;

#include <SDL2/SDL.h>

export module IComponent;
import workableLayer;

namespace application {
    export class IMouseInteractableComponent {
    public:
        virtual ~IMouseInteractableComponent() = default;

        virtual bool onMouseMoved(int, int, IVirtualMachineContextProvider &) {
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

        virtual bool onWindowSizeChanged(int, int, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual void onRender(IVirtualMachineContextProvider &) {}
    };
}
