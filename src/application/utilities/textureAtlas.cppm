module;

#include <SDL2/SDL.h>
#include <SDL_image.h>

export module textureAtlas;

import ywl.prelude;
import SDL2_Utilities;
import workableLayer;

using std::string_view;
using std::unique_ptr;
using std::runtime_error;

export template<size_t sideLength, size_t totalWidth> requires (totalWidth % sideLength == 0)
class SimpleSquareTextureAtlas {
    Texture texture;

public:
    SimpleSquareTextureAtlas() = default;

    SimpleSquareTextureAtlas(string_view resourceLocation, SDL_Renderer *renderer) {
        unique_ptr<SDL_Surface,
        ywl::basic::function_t<SDL_FreeSurface>
        > surface{
            IMG_Load(resourceLocation.data())
        };

        texture = Texture::LoadFromSurface(renderer, surface.get());

        if (texture.getWidth() != totalWidth || texture.getHeight() != sideLength) {
            throw runtime_error("TextureAtlas: texture size does not match the required size");
        }

        SDL_SetTextureScaleMode(texture.get(), SDL_ScaleModeNearest);
    }

    template<size_t idx> requires ((idx + 1) * sideLength <= totalWidth)
    void render(const SDL_Rect rect, SDL_Renderer *renderer) const {
        // use most
        SDL2_RenderTextureClippedRect(renderer, texture,
                                      SDL_Rect{
                                          .x = idx * sideLength,
                                          .y = 0,
                                          .w = sideLength,
                                          .h = sideLength
                                      }, rect);
    }
};
