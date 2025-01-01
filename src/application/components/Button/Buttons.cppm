module;

#include <SDL2/SDL.h>
#include <utility>
#include <SDL2/SDL_mixer.h>
#include <tuple>

export module Buttons;

import std_essentials;
import workableLayer;
import template_string;
import SDL2_Utilities;
import IComponent;
import applicationConstants;
import applicationResources;
import freeTypeFont;

export enum class ButtonState {
    Default,
    Hovered,
    Clicked
};

namespace application {
    export class SimpleConstantTextButton :
            public IRenderableComponent, public IWindowEventComponent,
            public IMouseMotionComponent, public IMouseButtonInteractableComponent {
        float xRatio;
        float yRatio;
        float heightRatio;

        function<void(int)> onPressed;

        function<string_view()> textProvider;

        Texture backgroundTextureDefault;
        Texture backgroundTextureHovered;
        Texture backgroundTextureClicked;

        SDL_Rect region;

        FreeTypeFontHolder::DetailedTask fontRendererTasks;

        int startX, startY, maxWidth, height;

        ButtonState state = ButtonState::Default;

    public:
        SimpleConstantTextButton(float xRatio, float yRatio, float heightRatio,
                                 SDL_Color backGroundColor,
                                 function<string_view()> textProvider, function<void(int)> onPressed,
                                 SDL_Renderer *renderer,
                                 SDL_Window *window)
            : xRatio(xRatio),
              yRatio(yRatio),
              heightRatio(heightRatio),
              onPressed(std::move(onPressed)),
              textProvider(std::move(textProvider)),
              backgroundTextureDefault(
                  SDL2_Create_Solid_Color_Texture(backGroundColor, renderer)),
              backgroundTextureHovered(
                  SDL2_Create_Solid_Color_Texture(
                      application::colors::Brighter(
                          backGroundColor), renderer)),
              backgroundTextureClicked(
                  SDL2_Create_Solid_Color_Texture(
                      application::colors::Darker(
                          backGroundColor),
                      renderer)) {
            calculateRegion(window);
        }

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
                Mix_PlayChannel(-1, mixChunks["click"], 0);
                onPressed(button);
                return true;
            }
            return false;
        }

        virtual bool onMouseButtonUp(int x, int y, int, IVirtualMachineContextProvider &) override {
            if (state == ButtonState::Clicked) {
                state = SDL2_PointCollideWithRect(x, y, region)
                            ? ButtonState::Hovered
                            : ButtonState::Default;
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

            auto pushColor = application::colors::PushColor{
                fontRendererTasks.fontAtlasTexture, constants::default_font_color
            };

            FreeTypeFontHolder::renderCenteredDetailedTask(fontRendererTasks, provider.getRenderer());
        }

        bool onWindowSizeChanged(int, int,
                                 IVirtualMachineContextProvider &provider) override {
            calculateRegion(provider.getWindow());
            return false;
        }

        void calculateRegion(SDL_Window *window) {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);

            height = static_cast<int>(static_cast<float>(windowHeight) * heightRatio);
            maxWidth = windowWidth;

            fontRendererTasks = freeTypeFontHolder->generateDetailedTask(
                textProvider(), windowWidth, height);

            int width = fontRendererTasks.w;
            int height = fontRendererTasks.h;
            region = SDL_Rect(
                windowWidth * xRatio - width / 2,
                windowHeight * yRatio - height / 2,
                width, height
            );
            fontRendererTasks.offsetX = region.x;
            fontRendererTasks.offsetY = region.y;
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

        function<void(int)> onPressed;

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
            return self;
        }

        // decltype(auto) ofBackGroundColors(this auto &&self, SDL_Color backGroundColor, SDL_Color backGroundHoveredColor,
        //                                   SDL_Color backGroundClickedColor) {
        //     self.backGroundColor = backGroundColor;
        //     self.backGroundHoveredColor = backGroundHoveredColor;
        //     self.backGroundClickedColor = backGroundClickedColor;
        //     return self;
        // }

        decltype(auto) whenPressed(this auto &&self, function<void(int)> onPressed) {
            self.onPressed = std::move(onPressed);
            return self;
        }

        SimpleConstantTextButton build(SDL_Renderer *renderer, SDL_Window *window) {
            auto &&fontHolder = *openSansHolder;
            fontHolder.loadFont(constants::default_font_size);
            function<string_view()> textProvider = [text = std::move(text)] -> string_view { return text.c_str(); };

            return {
                xRatio,
                yRatio,
                heightRatio,
                backGroundColor,
                std::move(textProvider),
                onPressed,
                renderer,
                window
            };
        }
    };
}
