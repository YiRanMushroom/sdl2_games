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

export class SelectGameLayer : public application::MouseComponentAutoHandledLayer {
    function<void()> m_onDestruct;

public:
    ~SelectGameLayer() override {
        if (m_onDestruct) {
            m_onDestruct();
        }
    }

    SelectGameLayer(const SelectGameLayer &) = delete;

    SelectGameLayer &operator=(const SelectGameLayer &) = delete;

    SelectGameLayer(function<void()> destructor) : m_onDestruct(std::move(destructor)) {
        auto &&fontHolder = *application::openSansHolder;
        // auto texture = fontHolder.getTextureBlended(50, "Snake", SDL_Color{191, 191, 191, 255},
        //                                             application::renderer);
        // mouseInteractableComponents.emplace_back(
        //     make_unique<SimpleConstantTextButton>(0.5, 0.4, 0.1, 1.0,
        //                                           SDL_Color{0, 0, 0, 0}, nullptr,
        //                                           [](int button) {
        //                                               AppLogMessage(
        //                                                   "Select Game: Button clicked with button: {}"_fmt(button));
        //                                           }, nullptr, nullptr));
    }
};
