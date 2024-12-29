module;

#include <SDL2/SDL.h>
#include <utility>
#include <AppMacros.h>

export module Buttons;

import std_essentials;
import workableLayer;
import template_string;
import SDL2_Utilities;
import IComponent;

namespace application::__impl {
    SDL_Color Get_Default_Hovered_Color(const SDL_Color &color) {
        return SDL_Color{
            static_cast<uint8_t>(color.r * 1.1), static_cast<uint8_t>(color.g * 1.1),
            static_cast<uint8_t>(color.b * 1.1), color.a
        };
    }

    SDL_Color Get_Default_Clicked_Color(const SDL_Color &color) {
        return SDL_Color{
            static_cast<uint8_t>(color.r * 0.9), static_cast<uint8_t>(color.g * 0.9),
            static_cast<uint8_t>(color.b * 0.9), color.a
        };
    }
}

export enum class ButtonState {
    Default,
    Hovered,
    Clicked
};

namespace application {
    export class SimpleConstantTextButton implements public IMouseInteractableComponent {
        float xRatio;
        float yRatio;
        float heightRatio;
        float aspectRatio;

        function<void(int)> onClick;

        Texture fontTexture;

        Texture backgroundTextureDefault;
        Texture backgroundTextureHovered;
        Texture backgroundTextureClicked;

        SDL_Rect region;

        ButtonState state = ButtonState::Default;

    public:
        SimpleConstantTextButton(float xRatio, float yRatio, float heightRatio, float aspectRatio,
                                 SDL_Color backGroundColor,
                                 Texture fontTexture, function<void(int)> onClick, SDL_Renderer *renderer,
                                 SDL_Window *window) : xRatio(xRatio),
                                                       yRatio(yRatio),
                                                       heightRatio(heightRatio),
                                                       aspectRatio(aspectRatio), onClick(std::move(onClick)),
                                                       fontTexture(std::move(fontTexture)),
                                                       backgroundTextureDefault(
                                                           SDL2_Create_Solid_Color_Texture(backGroundColor, renderer)),
                                                       backgroundTextureHovered(
                                                           SDL2_Create_Solid_Color_Texture(
                                                               application::__impl::Get_Default_Hovered_Color(
                                                                   backGroundColor), renderer)),
                                                       backgroundTextureClicked(
                                                           SDL2_Create_Solid_Color_Texture(
                                                               application::__impl::Get_Default_Clicked_Color(
                                                                   backGroundColor),
                                                               renderer)), region(
                                                           SDL2_RectBuilder(window)
                                                           .ofRelativePosition(xRatio, yRatio)
                                                           .withRelativeHeight(heightRatio, aspectRatio)
                                                           .buildCentered()
                                                       ) {}

        virtual bool onMouseMoved(int x, int y, IVirtualMachineContextProvider &) override {
            if (state == ButtonState::Clicked) {
                return false;
            }

            if (SDL2_PointCollideWithRect(x, y, region)) {
                state = ButtonState::Hovered;
            } else {
                state = ButtonState::Default;
            }

            return false;
        }

        virtual bool onMouseButtonDown(int x, int y, int button, IVirtualMachineContextProvider &) override {
            if (SDL2_PointCollideWithRect(x, y, region)) {
                state = ButtonState::Clicked;
                onClick(button);
                return true;
            }
            return false;
        }

        virtual bool onMouseButtonUp(int x, int y, int, IVirtualMachineContextProvider &) override {
            if (state == ButtonState::Clicked) {
                state = SDL2_PointCollideWithRect(x, y, region) ? ButtonState::Hovered : ButtonState::Default;
            }

            return false;
        }

        virtual void onRender(IVirtualMachineContextProvider &provider) override {
            SDL2_RenderTextureRect(provider.getRenderer(), [&]-> const Texture &{
                switch (state) {
                    case ButtonState::Default:
                        return backgroundTextureDefault;
                    case ButtonState::Hovered:
                        return backgroundTextureHovered;
                    case ButtonState::Clicked:
                        return backgroundTextureClicked;
                    default:
                        throw runtime_error("Invalid ButtonState");
                }
            }(), region);

            SDL2_RenderTextureRect(provider.getRenderer(), fontTexture, region);
        }

        virtual bool onWindowSizeChanged(int, int,
                                         IVirtualMachineContextProvider &provider) override {
            region = SDL2_RectBuilder(provider.getWindow())
                    .ofRelativePosition(xRatio, yRatio)
                    .withRelativeHeight(heightRatio, aspectRatio)
                    .buildCentered();
            return false;
        }
    };
}
