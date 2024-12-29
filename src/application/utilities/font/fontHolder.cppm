module;

#include <SDL2/SDL_ttf.h>
#include <utility>

export module fontHolder;
import std_essentials;
import std_overloads;
import SDL2_Utilities;

export import SDL2_Utilities;

export class FontHolder {
    unordered_map<int32_t, unique_ptr<TTF_Font, SDL2_FontDestructor> > fonts{};
    const string fontPath;

public:
    static inline bool allowImplicitLoad = true;

    FontHolder(const FontHolder &) = delete;

    FontHolder &operator=(const FontHolder &) = delete;

    // not copy and move assignable, constructible
    explicit FontHolder(string fontPath) : fontPath(std::move(fontPath)) {}

    TTF_Font *loadFont(int32_t fontSize) {
        if (fonts.contains(fontSize)) {
            return fonts[fontSize].get();
        }

        auto font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            throw runtime_error(TTF_GetError());
        }

        fonts[fontSize] = unique_ptr<TTF_Font, SDL2_FontDestructor>(font);
        return font;
    }

    TTF_Font *operator[](int32_t fontSize) {
        if (fonts.contains(fontSize)) {
            return fonts[fontSize].get();
        }

        if (allowImplicitLoad) {
            return loadFont(fontSize);
        }

        throw runtime_error(R"(Font with size: {} is not loaded,
            to allow implicit load set FontHolder::allowImplicitLoad to true)"_fmt(fontSize));
    }

    unique_ptr<SDL_Surface, SDL2_SurfaceDestructor> getSurfaceBlended(int32_t fontSize, string_view text,
                                                                      SDL_Color color) {
        auto font = (*this)[fontSize];
        auto surface = unique_ptr<SDL_Surface, SDL2_SurfaceDestructor>
                (TTF_RenderText_Blended(font, text.data(), color));
        if (!surface) {
            throw runtime_error(TTF_GetError());
        }

        return surface;
    }

    Texture getTextureBlended(int32_t fontSize, string_view text, SDL_Color color, SDL_Renderer *renderer) {
        auto surface = getSurfaceBlended(fontSize, text, color);
        return Texture::LoadFromSurface(renderer, surface.get());
    }
};
