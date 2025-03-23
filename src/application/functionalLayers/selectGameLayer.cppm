module;

#include <SDL2/SDL.h>

export module selectGameLayer;

import ywl.prelude;
import workableLayer;
import imguiUtilities;
import Buttons;
import applicationResources;
import applicationSharedState;
import applicationConstants;
import snakeGameLayer;
import Components;

using std::function;
using std::make_shared;

namespace application {
    export class SelectGameLayer : public application::WorkableLayer {
        function<void()> m_onDestruct;
        application::components::LayerComponentHolder holder;
    public:

        ~SelectGameLayer() override {
            if (m_onDestruct) {
                m_onDestruct();
            }
        }

        SelectGameLayer(const SelectGameLayer &) = delete;

        SelectGameLayer &operator=(const SelectGameLayer &) = delete;

        SelectGameLayer(IVirtualMachineContextProvider &provider, function<void()> destructor) : m_onDestruct(
                std::move(destructor)) {
            auto &&fontHolder = *application::openSansHolder;
            auto texture = fontHolder.getTextureBlended(50, "SNAKE",
                                                        constants::default_font_color,
                                                        provider.getRenderer());

            auto unique = std::make_unique<SimpleConstantTextButton>
                    (SimpleConstantTextButtonFactory{}
                             .withRelativePosition(0.5, 0.2)
                             .withRelativeHeight(0.1)
                             .withText("SNAKE",
                                       constants::default_font_color)
                             .ofBackGroundColor(
                                     constants::default_button_color)
                             .whenPressed(
                                     [&provider, currentIterator = provider.getCurrentIterator(), this](
                                             int button) {
                                         AppLogMessage(
                                                 "Snake: Button clicked with button: {}"_fmt(
                                                         button));
                                         provider.getDeferredTasks().emplace(
                                                 [&provider, currentIterator = provider.getCurrentIterator(), this] {
                                                     provider.getLayers().insert(
                                                             currentIterator,
                                                             make_unique<SnakeGameLayer>(
                                                                     std::move(
                                                                             m_onDestruct),
                                                                     provider));
                                                     provider.getLayers().erase(
                                                             currentIterator);
                                                 });
                                     }).build(provider.getRenderer(),
                                              provider.getWindow()));

            application::components::addComponentToHandler(holder, std::move(unique));
        }

        bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider &provider) override {
            auto ret = holder.handle(event, &provider);
            if (ret) {
                return true;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    provider.getDeferredTasks().emplace([it = provider.getCurrentIterator(), &provider] {
                        provider.getLayers().erase(it);
                    });
                    return true;
                }
            }

            return false;
        }

        void render(IVirtualMachineContextProvider &provider) override {
            return holder.onRender(&provider);
        }
    };
}
