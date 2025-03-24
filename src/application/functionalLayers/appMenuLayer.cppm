module;

#include <SDL_events.h>
#include <imgui.h>

export module appMenuLayer;

import ywl.prelude;
import workableLayer;
import imguiUtilities;
import Buttons;
import applicationResources;
import applicationSharedState;
import selectGameLayer;
import applicationConstants;
import imguiWindows;
import Components;

using std::shared_ptr;
using std::string;
using std::make_unique;
using std::make_shared;

namespace application {
    export class AppMenuLayer : public application::WorkableLayer {
    private:
        application::components::LayerComponentHolder holder;
    public:
        shared_ptr <string> imGuiWindowHolder;

        inline static bool imguiWindowOpen;

        AppMenuLayer(IVirtualMachineContextProvider &provider) {
            auto selectGameButton = SimpleConstantTextButtonFactory{}
                    .withText("Select Game")
                    .withRelativePosition(0.5, 0.4)
                    .withRelativeHeight(0.08)
                    .whenPressed([&provider, this](int button) {
                        AppLogMessage("Select Game: Button clicked with button: {}"_fmt(button));

                        ywl::utils::print_ln("Layer has been disabled").flush();

                        this->m_state = LayerState::disabled;

                        provider.getDeferredTasks().emplace(
                                [&provider, currentIterator = provider.getCurrentIterator(), this] {
                                    provider.getLayers().insert(
                                            std::next(currentIterator),
                                            make_unique<SelectGameLayer>(provider, [this] {
                                                ywl::utils::print_ln("Layer has been enabled").flush();
                                                this->m_state = LayerState::enabled;
                                            }));
                                });
                    })
                    .build(provider.getRenderer(), provider.getWindow());

            components::addComponentToHandler(holder,
                                              std::make_unique<SimpleConstantTextButton>(std::move(selectGameButton)));

            auto configButton = SimpleConstantTextButtonFactory{}
                    .withText("Config")
                    .withRelativePosition(0.5, 0.5)
                    .withRelativeHeight(0.08)
                    .whenPressed([](int button) {
                        AppLogMessage("Config: Button clicked with button: {}"_fmt(button));
                    })
                    .build(provider.getRenderer(), provider.getWindow());

            components::addComponentToHandler(holder,
                                              std::make_unique<SimpleConstantTextButton>(std::move(configButton)));

            auto exitButton = SimpleConstantTextButtonFactory{}
                    .withText("Exit")
                    .withRelativePosition(0.5, 0.6)
                    .withRelativeHeight(0.08)
                    .whenPressed([](int button) {
                        AppLogMessage("Exit: Button clicked with button: {}"_fmt(button));
                        application::isRunning = false;
                    })
                    .build(provider.getRenderer(), provider.getWindow());
//            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(exitButton)));
            components::addComponentToHandler(holder,
                                              std::make_unique<SimpleConstantTextButton>(std::move(exitButton)));

            imGuiWindowHolder = addImGuiDisplayWindow("appMenuLayer", &imguiWindowOpen, [](bool *) {
                ImGui::Text("Welcome to the Game Launcher!");
            });
        }

        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider &provider) override {
            return holder.handle(event, &provider);
        }

        void render(IVirtualMachineContextProvider &provider) override {
            holder.onRender(&provider);
        }
    };
}
