module;

#include <SDL2/SDL.h>
#include <utility>

export module selectGameLayer;

import ywl.prelude;
import layerFramework;
import imguiUtilities;
import IComponent;
import Buttons;
import applicationResources;
import applicationSharedState;
import applicationConstants;
import snakeGameLayer;

using std::function;
using std::make_shared;

namespace application {
    export class SelectGameLayer : public application::BasicMouseHandledLayer {
        function<void()> m_onDestruct;

    public:
        ~SelectGameLayer() override {
        }

        SelectGameLayer(const SelectGameLayer &) = delete;

        SelectGameLayer &operator=(const SelectGameLayer &) = delete;

        SelectGameLayer(IVirtualMachineContextProvider &provider, function<void()> destructor) : m_onDestruct(
            std::move(destructor)) {
            auto &&fontHolder = *application::openSansHolder;
            auto texture = fontHolder.getTextureBlended(50, "SNAKE",
                                                        constants::default_font_color,
                                                        provider.getRenderer());

            auto shared = make_shared<SimpleConstantTextButton>(SimpleConstantTextButtonFactory{}
                .withRelativePosition(0.5, 0.2)
                .withRelativeHeight(0.1)
                .withText("SNAKE", constants::default_font_color)
                .ofBackGroundColor(constants::default_button_color)
                .whenPressed([&provider, currentIterator = provider.getCurrentIterator(), this](int button) {
                    AppLogMessage("Snake: Button clicked with button: {}"_fmt(button));
                    provider.getDeferredTasks().emplace(
                        [&provider, currentIterator = provider.getCurrentIterator(), this] {
                            provider.getLayers().insert(
                                currentIterator,
                                make_unique<SnakeGameLayer>(std::move(m_onDestruct), provider));
                            provider.getLayers().erase(currentIterator);
                        });
                }).build(provider.getRenderer(), provider.getWindow()));

            registerBasicMouseRenderableComponents(shared);
        }
    };
}
