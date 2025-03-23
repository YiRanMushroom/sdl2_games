module;

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

export module SDL2_Utilities;

import ywl.prelude;

using std::unique_ptr;
using std::runtime_error;
using std::optional;
using std::shared_ptr;

/*export struct SDL2_FontDestructor {
    static void operator()(TTF_Font *font) {
        TTF_CloseFont(font);
    }
};

export struct SDL2_SurfaceDestructor {
    static void operator()(SDL_Surface *surface) {
        SDL_FreeSurface(surface);
    }
};

export struct SDL2_TextureDestructor {
    static void operator()(SDL_Texture *texture) {
        SDL_DestroyTexture(texture);
    }
};*/

export class Texture {
    unique_ptr<SDL_Texture, ywl::basic::function_t<SDL_DestroyTexture>> texture; // NOLINT
    int32_t width{};
    int32_t height{};

public:
    Texture(const Texture &) = delete;

    Texture() = default;

    Texture(unique_ptr<SDL_Texture, ywl::basic::function_t<SDL_DestroyTexture>> texture, int32_t width, int32_t height)
        : texture(std::move(texture)), width(width), height(height) {}

    Texture &operator=(const Texture &) = delete;

    Texture(Texture &&) noexcept = default;

    Texture &operator=(Texture &&) noexcept = default;

    static Texture LoadFromSurface(SDL_Renderer *renderer, SDL_Surface *surface) {
        auto sdl_texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!sdl_texture) {
            throw runtime_error(SDL_GetError());
        }

        return {
            unique_ptr<SDL_Texture, ywl::basic::function_t<SDL_DestroyTexture>>{sdl_texture},
            surface->w, surface->h
        };
    }

    SDL_Texture *get() const {
        return texture.get();
    }

    SDL_Texture *operator*() const {
        return texture.get();
    }

    int32_t getWidth() const {
        return width;
    }

    int32_t getHeight() const {
        return height;
    }

    float getAspectRatio() const {
        return static_cast<float>(width) / height;
    }
};

export void SDL2_RenderTextureRect(SDL_Renderer *renderer, const Texture &texture, const SDL_Rect &rect) {
    SDL_RenderCopy(renderer, texture.get(), nullptr, &rect);
}

export void SDL2_RenderTextureClippedRect(SDL_Renderer *renderer, const Texture &texture, const SDL_Rect &clip,
                                          const SDL_Rect &rect) {
    SDL_RenderCopy(renderer, texture.get(), &clip, &rect);
}

export Texture SDL2_Create_Solid_Color_Texture(SDL_Color color,
                                               SDL_Renderer *renderer, int width = 128, int height = 128) {
    // Create an SDL_Surface with the specified width, height, and color depth
    auto surface = unique_ptr<SDL_Surface, ywl::basic::function_t<SDL_FreeSurface>>{
        SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000,
                             0x0000FF00, 0x000000FF, 0xFF000000)
    };
    if (!surface) {
        throw runtime_error(SDL_GetError());
    }

    // Fill the surface with the specified color
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));

    // Create a texture from the surface
    auto texture = unique_ptr<SDL_Texture, ywl::basic::function_t<SDL_DestroyTexture>>
            {SDL_CreateTextureFromSurface(renderer, surface.get())};
    if (!texture) {
        throw runtime_error(SDL_GetError());
    }

    return {std::move(texture), width, height};
}

export bool SDL2_PointCollideWithRect(int x, int y, const SDL_Rect &rect) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

// enum class SDL2_RectBuilderType : uint8_t {
//     Unspecified = 0,
//     Absolute,
//     Relative
// };
//
// enum class SDL2_RectBuilderSizeSpecifierType : uint8_t {
//     Unspecified = 0,
//     Width,
//     Height,
// };

export class SDL2_RectBuilder {
private:
    // SDL2_RectBuilderType positionType = SDL2_RectBuilderType::Unspecified;
    // SDL2_RectBuilderType sizeType = SDL2_RectBuilderType::Unspecified;
    //
    // SDL2_RectBuilderSizeSpecifierType sizeSpecifierType = SDL2_RectBuilderSizeSpecifierType::Unspecified;

    optional<std::pair<int, int> > windowSize{};

    int x{}, y{}, width{}, height{};

    std::pair<int, int> getWindowSize() const {
        if (!windowSize) {
            throw runtime_error("SDL2_RectBuilder: window is not set");
        }

        return {windowSize->first, windowSize->second};
    }

public:
    SDL2_RectBuilder() = default;

    SDL2_RectBuilder(SDL_Window *window) {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        windowSize = {windowWidth, windowHeight};
    }

    SDL2_RectBuilder(int windowWidth, int windowHeight) : windowSize{{windowWidth, windowHeight}} {}

    decltype(auto) ofAbsolutePosition(this auto &&self, int x, int y) {
        self.x = x;
        self.y = y;
        return self;
    }

    decltype(auto) ofRelativePosition(this auto &&self, float xRatio, float yRatio) {
        auto [windowWidth, windowHeight] = self.getWindowSize();

        self.x = static_cast<int>(windowWidth * xRatio);
        self.y = static_cast<int>(windowHeight * yRatio);
        return self;
    }

    decltype(auto) withAbsoluteSize(this auto &&self, int width, int height) {
        self.width = width;
        self.height = height;
        return self;
    }

    decltype(auto) withRelativeWidth(this auto &&self, float widthRatio, float aspectRatio) {
        auto [windowWidth, windowHeight] = self.getWindowSize();

        self.width = static_cast<int>(windowWidth * widthRatio);
        self.height = static_cast<int>(self.width / aspectRatio);
        return self;
    }

    decltype(auto) withRelativeHeight(this auto &&self, float heightRatio, float aspectRatio) {
        auto [windowWidth, windowHeight] = self.getWindowSize();

        self.height = static_cast<int>(windowHeight * heightRatio);
        self.width = static_cast<int>(self.height * aspectRatio);
        return self;
    }

    SDL_Rect buildCentered() const {
        return {
            (x - width / 2),
            (y - height / 2),
            width, height
        };
    }

    SDL_Rect buildLeftTop() const {
        return {x, y, width, height};
    }
};

export class SharedTexture {
    shared_ptr<SDL_Texture> texture;
    int32_t width{};
    int32_t height{};

public:
    SharedTexture(const SharedTexture &) = default;

    SharedTexture() = default;

private:
    SharedTexture(shared_ptr<SDL_Texture> texture, int32_t width, int32_t height)
        : texture(std::move(texture)), width(width), height(height) {}

public:
    SharedTexture &operator=(const SharedTexture &) = default;

    SharedTexture(SharedTexture &&) noexcept = default;

    SharedTexture &operator=(SharedTexture &&) noexcept = default;

    static SharedTexture LoadFromSurface(SDL_Renderer *renderer, SDL_Surface *surface) {
        auto sdl_texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!sdl_texture) {
            throw runtime_error(SDL_GetError());
        }

        return {
            shared_ptr<SDL_Texture>{sdl_texture, SDL_DestroyTexture},
            surface->w, surface->h
        };
    }

    [[nodiscard]] SDL_Texture *get() const {
        return texture.get();
    }

    SDL_Texture *operator*() const {
        return texture.get();
    }

    [[nodiscard]] int32_t getWidth() const {
        return width;
    }

    [[nodiscard]] int32_t getHeight() const {
        return height;
    }

    [[nodiscard]] float getAspectRatio() const {
        return static_cast<float>(width) / height;
    }
};

export namespace application::colors {
    SDL_Color Brighter(const SDL_Color &color) {
        return SDL_Color{
            static_cast<uint8_t>(color.r * 1.1), static_cast<uint8_t>(color.g * 1.1),
            static_cast<uint8_t>(color.b * 1.1), color.a
        };
    }

    SDL_Color Darker(const SDL_Color &color) {
        return SDL_Color{
            static_cast<uint8_t>(color.r * 0.9), static_cast<uint8_t>(color.g * 0.9),
            static_cast<uint8_t>(color.b * 0.9), color.a
        };
    }
}


struct mixChunkDeleter {
    static void operator()(Mix_Chunk *chunk) {
        Mix_FreeChunk(chunk);
    }
};

struct mixMusicDeleter {
    static void operator()(Mix_Music *music) {
        Mix_FreeMusic(music);
    }
};

export class MixChunk {
    unique_ptr<Mix_Chunk, mixChunkDeleter> chunk;

public:
    MixChunk(Mix_Chunk *chunk) : chunk(chunk) {}

    Mix_Chunk *get() const {
        return chunk.get();
    }

    operator Mix_Chunk *() const {
        return chunk.get();
    }
};

export class SharedMixChunk {
    shared_ptr<Mix_Chunk> chunk;

public:
    SharedMixChunk() = default;

    SharedMixChunk(Mix_Chunk *chunk) : chunk{chunk, mixChunkDeleter{}} {}

    Mix_Chunk *get() const {
        return chunk.get();
    }

    operator Mix_Chunk *() const {
        return chunk.get();
    }
};

export class MixMusic {
    unique_ptr<Mix_Music, mixMusicDeleter> music;

public:
    MixMusic(Mix_Music *music) : music(music) {}

    Mix_Music *get() const {
        return music.get();
    }

    operator Mix_Music *() const {
        return music.get();
    }
};

export class SharedMixMusic {
    shared_ptr<Mix_Music> music;

public:
    SharedMixMusic() = default;

    SharedMixMusic(Mix_Music *music) : music{music, mixMusicDeleter{}} {}

    Mix_Music *get() const {
        return music.get();
    }

    operator Mix_Music *() const {
        return music.get();
    }
};

namespace application::colors {
    export struct PushColor {
        SDL_Texture *texture;

        PushColor(SDL_Texture *texture, SDL_Color color) : texture(texture) {
            SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(texture, color.a);
        }

        ~PushColor() {
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
        }
    };
}
