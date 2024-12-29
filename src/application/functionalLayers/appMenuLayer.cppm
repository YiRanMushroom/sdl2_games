module;

#include <SDL2/SDL.h>
#include <utility>

export module appMenuLayer;

import layerFramework;
import std_essentials;
import imguiUtilities;
import IComponent;
import Buttons;
import applicationResources;
import std_overloads;
import applicationSharedState;

namespace application {
    export class AppMenuLayer : public application::MouseComponentAutoHandledLayer {
    public:
        AppMenuLayer(const IVirtualMachineContextProvider &provider) {
            auto &&fontHolder = *application::openSansHolder;
            fontHolder.loadFont(50);
            auto selectGameTexture = fontHolder.getTextureBlended(50, "Select Game", SDL_Color{191, 191, 191, 255},
                                                                  provider.getRenderer());
            auto configTexture = fontHolder.getTextureBlended(50, "Config", SDL_Color{191, 191, 191, 255},
                                                              provider.getRenderer());
            auto exitTexture = fontHolder.getTextureBlended(50, "Exit", SDL_Color{191, 191, 191, 255},
                                                            provider.getRenderer());

            mouseInteractableComponents.emplace_back(
                make_unique<SimpleConstantTextButton>(0.5, 0.4, 0.1, selectGameTexture.getAspectRatio(),
                                                      SDL_Color{0, 0, 0, 0}, std::move(selectGameTexture),
                                                      [](int button) {
                                                          AppLogMessage(
                                                              "Select Game: Button clicked with button: {}"_fmt(
                                                                  button));
                                                      }, provider.getRenderer(), provider.getWindow()));

            mouseInteractableComponents.emplace_back(
                make_unique<SimpleConstantTextButton>(0.5, 0.5, 0.1, configTexture.getAspectRatio(),
                                                      SDL_Color{0, 0, 0, 0}, std::move(configTexture),
                                                      [](int button) {
                                                          AppLogMessage(
                                                              "Config: Button clicked with button: {}"_fmt(button));
                                                      }, provider.getRenderer(), provider.getWindow()));

            mouseInteractableComponents.emplace_back(
                make_unique<SimpleConstantTextButton>(0.5, 0.6, 0.1, exitTexture.getAspectRatio(),
                                                      SDL_Color{0, 0, 0, 0}, std::move(exitTexture),
                                                      [](int button) {
                                                          AppLogMessage(
                                                              "Exit: Button clicked with button: {}"_fmt(button));
                                                          application::isRunning = false;
                                                      }, provider.getRenderer(), provider.getWindow()));
        }
    };
}
