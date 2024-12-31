module;

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

export module textureAtlas;

import std_essentials;
import SDL2_Utilities;
import workableLayer;

export template<size_t sideLength, size_t totalWidth> requires (totalWidth % sideLength == 0)
class SimpleSquareTextureAtlas {
    Texture texture;

public:
    SimpleSquareTextureAtlas() = default;

    SimpleSquareTextureAtlas(string_view resourceLocation, SDL_Renderer *renderer) {
        unique_ptr<SDL_Surface, SDL2_SurfaceDestructor> surface{
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
