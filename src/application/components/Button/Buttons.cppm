module;

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

export module Buttons;

import ywl.prelude;
import workableLayer;
import SDL2_Utilities;
import applicationConstants;
import applicationResources;
import freeTypeFont;
import Components;

using std::function;
using std::string_view;
using std::runtime_error;
using std::shared_ptr;
using std::make_shared;
using std::string;

export enum class ButtonState {
    Default,
    Hovered,
    Clicked
};

namespace application {
    export class SimpleConstantTextButton :
            public components::DerivedComponentFrom<
                    components::IRenderableComponent,
                    components::IMouseButtonUpComponent,
                    components::IMouseButtonDownComponent,
                    components::IWindowEventComponent
            > {
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

        bool onMouseButtonDown(int x, int y, int button, IVirtualMachineContextProvider *) override {
            if (SDL2_PointCollideWithRect(x, y, region)) {
                Mix_PlayChannel(-1, mixChunks["click"], 0);
                onPressed(button);
                return true;
            }
            return false;
        }

        bool onMouseButtonUp(int x, int y, int, IVirtualMachineContextProvider *) override {
            if (state == ButtonState::Clicked) {
                state = SDL2_PointCollideWithRect(x, y, region)
                        ? ButtonState::Hovered
                        : ButtonState::Default;
            }

            return false;
        }

        void onRender(IVirtualMachineContextProvider *provider) override {
            int x, y;
            bool leftButtonPressed = SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT);
            if (SDL2_PointCollideWithRect(x, y, region)) {
                if (leftButtonPressed) {
                    state = ButtonState::Clicked;
                } else {
                    state = ButtonState::Hovered;
                }
            } else {
                state = ButtonState::Default;
            }

            SDL2_RenderTextureRect(provider->getRenderer(), [&] -> const Texture & {
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

            FreeTypeFontHolder::renderCenteredDetailedTask(fontRendererTasks, provider->getRenderer());
        }

        bool onWindowSizeChanged(int, int,
                                 IVirtualMachineContextProvider *provider) override {
            calculateRegion(provider->getWindow());
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
    };

    export class SimpleConstantTextButtonFactory {
        float xRatio{};
        float yRatio{};
        float heightRatio{};

        function<void(int)> onPressed;

        string text;
        SDL_Color textColor{};

        SDL_Color backGroundColor = constants::default_button_color;

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

        decltype(auto) whenPressed(this auto &&self, function<void(int)> onPressed) {
            self.onPressed = std::move(onPressed);
            return self;
        }

        SimpleConstantTextButton build(SDL_Renderer *renderer, SDL_Window *window) {
            auto &&fontHolder = *openSansHolder;
            fontHolder.loadFont(constants::default_font_size);
            function < string_view() > textProvider =
                    [text = std::move(text)]() -> string_view {
                        return text;
                    };

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
