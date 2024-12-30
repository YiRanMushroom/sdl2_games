module;

#include <utility>
#include <iterator>
#include <AppMacros.h>

export module appMenuLayer;

import layerFramework;
import std_essentials;
import imguiUtilities;
import IComponent;
import Buttons;
import applicationResources;
import std_overloads;
import applicationSharedState;
import selectGameLayer;
import applicationConstants;

namespace application {
    export class AppMenuLayer extends public application::BasicMouseHandledLayer {
    public:
        AppMenuLayer(IVirtualMachineContextProvider &provider) {
            auto selectGameButton = SimpleConstantTextButtonFactory{}
                    .withText("Select Game")
                    .withRelativePosition(0.5, 0.4)
                    .withRelativeHeight(0.1)
                    .whenPopUp([&provider, this](int button) {
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
                    .withRelativeHeight(0.1)
                    .whenPopUp([](int button) {
                        AppLogMessage("Config: Button clicked with button: {}"_fmt(button));
                    })
                    .build(provider.getRenderer(), provider.getWindow());
            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(configButton)));

            auto exitButton = SimpleConstantTextButtonFactory{}
                    .withText("Exit")
                    .withRelativePosition(0.5, 0.6)
                    .withRelativeHeight(0.1)
                    .whenPopUp([](int button) {
                        AppLogMessage("Exit: Button clicked with button: {}"_fmt(button));
                        application::isRunning = false;
                    })
                    .build(provider.getRenderer(), provider.getWindow());
            registerBasicMouseRenderableComponents(make_shared<SimpleConstantTextButton>(std::move(exitButton)));
        }
    };
}
