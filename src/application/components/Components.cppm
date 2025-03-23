module;

#include <SDL_image.h>

export module Components;

import ywl.prelude;
import workableLayer;

namespace application {
//    class IVirtualMachineContextProvider;
    namespace components {
        export class IComponent {
        public:
            virtual ~IComponent() = default;
        };

        export class IRenderableComponent {
        public:
            virtual void onRender(IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseMotionComponent {
        public:
            virtual bool onMouseMotion(int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseButtonDownComponent {
        public:
            virtual bool onMouseButtonDown(int, int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseButtonUpComponent {
        public:
            virtual bool onMouseButtonUp(int, int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IWindowEventComponent {
        public:
            virtual bool onWindowSizeChanged(int, int, IVirtualMachineContextProvider *) = 0;
        };

        export class IMouseWheelInteractableComponent {
        public:
            virtual bool onMouseWheel(int, int, unsigned int, IVirtualMachineContextProvider *) = 0;
        };

        export class LayerComponentHolder {
            std::vector <std::unique_ptr<IComponent>> m_componentsHolder;

            std::tuple <
            std::vector<IRenderableComponent *>,
            std::vector<IMouseMotionComponent *>,
            std::vector<IMouseButtonDownComponent *>,
            std::vector<IMouseButtonUpComponent *>,
            std::vector<IWindowEventComponent *>,
            std::vector<IMouseWheelInteractableComponent *>
            > m_components;

        public:
            template<typename... Ts,
                    std::convertible_to<const IComponent &> T>
            void add(std::unique_ptr <T> component) {
                T * ptr = component.get();
                m_componentsHolder.push_back(std::move(component));
                ((std::get < std::vector < Ts * >> (m_components).push_back(ptr)), ...);
            }

            void onRender(IVirtualMachineContextProvider *provider) const {
                for (auto &component: std::get <
                                      std::vector < IRenderableComponent * >
                                      > (m_components)) {
                    component->onRender(provider);
                }
            }

            bool handle(const SDL_Event &event, IVirtualMachineContextProvider *provider) const {
                switch (event.type) {
                    case SDL_MOUSEMOTION: {
                        bool handled = false;
                        for (auto &component: std::get <
                                              std::vector < IMouseMotionComponent * >
                                              > (m_components)) {
                            handled |= component->onMouseMotion(event.motion.x, event.motion.y, provider);
                        }
                        return handled;
                    }
                    case SDL_MOUSEBUTTONDOWN: {
                        bool handled = false;
                        for (auto &component: std::get <
                                              std::vector < IMouseButtonDownComponent * >> (m_components)) {
                            handled |= component->onMouseButtonDown(event.button.x, event.button.y, event.button.button,
                                                                    provider);
                        }
                        return handled;
                    }
                    case SDL_MOUSEBUTTONUP: {
                        bool handled = false;
                        for (auto &component: std::get <
                                              std::vector < IMouseButtonUpComponent * >> (m_components)) {
                            handled |= component->onMouseButtonUp(event.button.x, event.button.y, event.button.button,
                                                                  provider);
                        }
                        return handled;
                    }
                    case SDL_WINDOWEVENT: {
                        if (event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED) {
                            bool handled = false;
                            for (auto &component: std::get <
                                                  std::vector < IWindowEventComponent * >> (m_components)) {
                                handled |= component->onWindowSizeChanged(
                                        event.window.data1, event.window.data2, provider);
                            }
                            return handled;
                        }
                        return false;
                    }
                    default:
                        return false;
                }
            }
        };

        class DerivedFromFlag {

        };

        export template<typename ... Ts>
        class DerivedComponentFrom : public IComponent, public DerivedFromFlag, public Ts ... {
        public:
            using component_types = std::tuple<Ts...>;
        };

        template<typename>
        struct add_component_to_handler_t {
            static_assert(false, "add_component_to_handler_t must be specialized");
        };

        template<typename, typename>
        struct add_component_to_handler_impl_t {
            static_assert(false, "add_component_to_handler_impl_t must be specialized");
        };

        template<typename T, typename ... Ts>
        struct add_component_to_handler_impl_t<T, std::tuple < Ts...>> {
        static void add(LayerComponentHolder &holder, std::unique_ptr <T> component) {
            holder.add<Ts...>(std::move(component));
        }
    };

    template<std::convertible_to<const DerivedFromFlag &> T>
    struct add_component_to_handler_t<T> : public add_component_to_handler_impl_t<T, typename T::component_types> {
    };

    export template<typename T>
    void addComponentToHandler(LayerComponentHolder &holder, std::unique_ptr <T> component) {
        add_component_to_handler_t<T>::add(holder, std::move(component));
    }
}
}
