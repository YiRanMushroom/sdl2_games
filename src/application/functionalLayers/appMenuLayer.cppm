module;

#include <utility>
#include <iterator>
#include <AppMacros.h>
#include <imgui.h>

export module appMenuLayer;

import ywl.prelude;
import layerFramework;
import imguiUtilities;
import IComponent;
import Buttons;
import applicationResources;
import applicationSharedState;
import selectGameLayer;
import applicationConstants;
import imguiWindows;

using std::shared_ptr;
using std::string;
using std::make_unique;
using std::make_shared;

namespace application {
    export class AppMenuLayer extends public application::BasicMouseHandledLayer {
    public:
        shared_ptr<string> imGuiWindowHolder;

        inline static bool imguiWindowOpen;

        AppMenuLayer(IVirtualMachineContextProvider &provider) {
            auto selectGameButton = SimpleConstantTextButtonFactory{}
                    .withText("Select Game")
                    .withRelativePosition(0.5, 0.4)
                    .withRelativeHeight(0.08)
                    .whenPressed([&provider, this](int button) {
                        AppLogMessage("Select Game: Button clicked with button: {}"_fmt(button));

                        this->m_state = LayerState::disabled;

                        provider.getDeferredTasks().emplace(
                            [&provider, currentIterator = provider.getCurrentIterator(), this] {
                                provider.getLayers().insert(
                                    std::next(currentIterator),
                                    make_unique<SelectGameLayer>(provider, [this] {
                                        this->m_state = LayerState::enabled;
                                    }));
                            });
                    })
                    .build(provider.getRenderer(), provider.getWindow());
            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(selectGameButton)));

            auto configButton = SimpleConstantTextButtonFactory{}
                    .withText("Config")
                    .withRelativePosition(0.5, 0.5)
                    .withRelativeHeight(0.08)
                    .whenPressed([](int button) {
                        AppLogMessage("Config: Button clicked with button: {}"_fmt(button));
                    })
                    .build(provider.getRenderer(), provider.getWindow());
            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(configButton)));

            auto exitButton = SimpleConstantTextButtonFactory{}
                    .withText("Exit")
                    .withRelativePosition(0.5, 0.6)
                    .withRelativeHeight(0.08)
                    .whenPressed([](int button) {
                        AppLogMessage("Exit: Button clicked with button: {}"_fmt(button));
                        application::isRunning = false;
                    })
                    .build(provider.getRenderer(), provider.getWindow());
            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(exitButton)));

            imGuiWindowHolder = addImGuiDisplayWindow("appMenuLayer", &imguiWindowOpen, [](bool *) {
                ImGui::Text("Welcome to the Game Launcher!");
            });
        }
    };
}
