module;

import ywl.prelude;

export module IComponentBase;

namespace application {
    class IVirtualMachineContextProvider;

    export class IComponentBase {
        virtual ~IComponentBase() = default;
    };

    namespace components {
        export class IRenderableComponent : public IComponentBase {
            virtual void onRender(IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseMotionComponent : public IComponentBase {
            virtual bool onMouseMotion(int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseButtonInteractableComponent : public IComponentBase {
            virtual bool onMouseButtonDown(int, int, int, IVirtualMachineContextProvider *) = 0;
            virtual bool onMouseButtonUp(int, int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IWindowEventComponent : public IComponentBase {
            virtual bool onWindowSizeChanged(int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseWheelInteractableComponent : public IComponentBase {
            virtual bool onMouseWheel(int, int, unsigned int, IVirtualMachineContextProvider *) = 0;
        };
    }
}
