module;

#include <SDL2/SDL.h>

export module IComponent;
import workableLayer;

namespace application {
    export class IMouseInteractableComponent {
    public:
        virtual ~IMouseInteractableComponent() = default;

        virtual bool onMouseMoved(int x, int y, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseButtonDown(int x, int y, int button, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseButtonUp(int x, int y, int button, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onMouseWheel(int x, int y, unsigned int direction, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual bool onWindowSizeChanged(int width, int height, IVirtualMachineContextProvider &) {
            return false;
        }

        virtual void onRender(IVirtualMachineContextProvider &provider) {}
    };
}

