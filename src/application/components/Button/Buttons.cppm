module;

#include <SDL2/SDL.h>
#include <utility>
#include <AppMacros.h>
#include <tuple>

export module Buttons;

import std_essentials;
import workableLayer;
import template_string;
import SDL2_Utilities;
import IComponent;
import applicationConstants;
import applicationResources;

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
    export class SimpleConstantTextButton implements
            public IRenderableComponent, public IWindowEventComponent,
            public IMouseMotionComponent, public IMouseButtonInteractableComponent {
        float xRatio;
        float yRatio;
        float heightRatio;
        float aspectRatio;

        function<void(int)> onPopUp;

        Texture fontTexture;

        Texture backgroundTextureDefault;
        Texture backgroundTextureHovered;
        Texture backgroundTextureClicked;

        SDL_Rect region;

        ButtonState state = ButtonState::Default;

        int button{};

    public:
        SimpleConstantTextButton(float xRatio, float yRatio, float heightRatio, float aspectRatio,
                                 SDL_Color backGroundColor,
                                 Texture fontTexture, function<void(int)> onPopUp, SDL_Renderer *renderer,
                                 SDL_Window *window)
            : xRatio(xRatio),
              yRatio(yRatio),
              heightRatio(heightRatio),
              aspectRatio(aspectRatio), onPopUp(std::move(onPopUp)),
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

        virtual bool onMouseMotion(int x, int y, IVirtualMachineContextProvider &) override {
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
                this->button = button;
                return true;
            }
            return false;
        }

        virtual bool onMouseButtonUp(int x, int y, int, IVirtualMachineContextProvider &) override {
            if (state == ButtonState::Clicked) {
                if (SDL2_PointCollideWithRect(x, y, region)) {
                    onPopUp(this->button);
                }
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

        std::tuple<
            shared_ptr<IRenderableComponent>,
            shared_ptr<IMouseMotionComponent>,
            shared_ptr<IMouseButtonInteractableComponent>,
            shared_ptr<IWindowEventComponent>
        > breakDown(this SimpleConstantTextButton &&self) { //NOLINT
            shared_ptr<SimpleConstantTextButton> selfPtr = make_shared<SimpleConstantTextButton>(std::move(self));
            return {
                selfPtr,
                selfPtr,
                selfPtr,
                selfPtr
            };
        }
    };

    export class SimpleConstantTextButtonFactory {
        float xRatio{};
        float yRatio{};
        float heightRatio{};

        function<void(int)> onPopUp;

        string text;
        SDL_Color textColor{};

        SDL_Color backGroundColor = constants::default_button_color;
        // SDL_Color backGroundHoveredColor = application::__impl::Get_Default_Hovered_Color(backGroundColor);
        // SDL_Color backGroundClickedColor = application::__impl::Get_Default_Clicked_Color(backGroundColor);

    public:
        SimpleConstantTextButtonFactory() = default;

        decltype(auto) withRelativePosition(this auto &&self, float xRatio, float yRatio) {
            self.xRatio = xRatio;
            self.yRatio = yRatio;
            return self;
        }

        decltype(auto) withRelativeHeight(this auto &&self, float heightRatio) {
            self.heightRatio = heightRatio;
            return self;
        }

        decltype(auto) withText(this auto &&self, string text, SDL_Color textColor = constants::default_font_color) {
            self.text = std::move(text);
            self.textColor = textColor;
            return self;
        }

        decltype(auto) ofBackGroundColor(this auto &&self, SDL_Color backGroundColor) {
            self.backGroundColor = backGroundColor;
            self.backGroundHoveredColor = application::__impl::Get_Default_Hovered_Color(backGroundColor);
            self.backGroundClickedColor = application::__impl::Get_Default_Clicked_Color(backGroundColor);
            return self;
        }

        // decltype(auto) ofBackGroundColors(this auto &&self, SDL_Color backGroundColor, SDL_Color backGroundHoveredColor,
        //                                   SDL_Color backGroundClickedColor) {
        //     self.backGroundColor = backGroundColor;
        //     self.backGroundHoveredColor = backGroundHoveredColor;
        //     self.backGroundClickedColor = backGroundClickedColor;
        //     return self;
        // }

        decltype(auto) whenPopUp(this auto &&self, function<void(int)> onPopUp) {
            self.onPopUp = std::move(onPopUp);
            return self;
        }

        SimpleConstantTextButton build(SDL_Renderer *renderer, SDL_Window *window) {
            auto &&fontHolder = *openSansHolder;
            fontHolder.loadFont(constants::default_font_size);
            auto fontTexture = fontHolder.getTextureBlended(constants::default_font_size, text, textColor, renderer);

            return {
                xRatio, yRatio, heightRatio,
                fontTexture.getAspectRatio(), backGroundColor,
                std::move(fontTexture), onPopUp, renderer, window
            };
        }
    };
}
