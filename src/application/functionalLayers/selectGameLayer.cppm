module;

#include <SDL2/SDL.h>
#include <utility>

export module selectGameLayer;

import layerFramework;
import std_essentials;
import imguiUtilities;
import IComponent;
import Buttons;
import applicationResources;
import std_overloads;
import applicationSharedState;
import applicationConstants;

namespace application {
    export class SelectGameLayer : public application::BasicMouseHandledLayer {
        function<void()> m_onDestruct;

    public:
        virtual ~SelectGameLayer() override {
            if (m_onDestruct) {
                m_onDestruct();
            }
        }

        SelectGameLayer(const SelectGameLayer &) = delete;

        SelectGameLayer &operator=(const SelectGameLayer &) = delete;

        SelectGameLayer(IVirtualMachineContextProvider &provider, function<void()> destructor) : m_onDestruct(
            std::move(destructor)) {
            auto &&fontHolder = *application::openSansHolder;
            auto texture = fontHolder.getTextureBlended(50, "SNAKE", constants::default_font_color,
                                                        provider.getRenderer());

            auto shared = make_shared<SimpleConstantTextButton>
            (0.5, 0.2, 0.1, texture.getAspectRatio(),
             constants::default_button_color, std::move(texture),
             [&](int button) {
                 AppLogMessage(
                     "Snake: Button clicked with button: {}"_fmt(button));
                 provider.getDeferredTasks().emplace(
                     [&provider, currentIterator = provider.
                         getCurrentIterator()] {
                         provider.getLayers().erase(currentIterator);
                     });
             }, provider.getRenderer(), provider.getWindow());

            registerBasicMouseRenderableComponents(shared);
        }
    };
}
