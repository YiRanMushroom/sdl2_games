module;

#include <cassert>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL2/SDL.h>
// #include <utility>
#include <cmath>
#include <algorithm>

export module freeTypeFont;
import ywl.prelude;
import SDL2_Utilities;
import imguiUtilities;
import applicationConstants;

using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;
using std::runtime_error;

export class FreeTypeFontHolder {
public:
    // using LibraryPointerType = FT_Library;
    // using LibraryActualType = std::remove_cvref_t<decltype(*std::declval<FT_Library>())>;

private:
    // struct LibraryDeleter {
    //     static void operator()(LibraryActualType *library) {
    //         if (library)
    //             FT_Done_FreeType(library);
    //     }
    // };
    //
    // unique_ptr<LibraryActualType, LibraryDeleter> library;
    // string fontLocation;

    struct CharInfo {
        SDL_Rect posAtlas{};
        int advance{};
        int leftBearing{};
        int upOffset{};
        bool valid{};

        CharInfo(SDL_Rect posAtlas, int advance, int leftBearing, int upOffset)
            : posAtlas(posAtlas), advance(advance), leftBearing(leftBearing), upOffset(upOffset), valid(true) {}

        operator bool() const { // NOLINT
            return valid;
        }

        CharInfo() = default;
    };

    Texture atlas;
    vector<CharInfo> charInfos;

    int lineHeight{};
    float additionalLineSpacing{};

    constexpr static int fontSize = 96;

public:
    FreeTypeFontHolder(string_view fontLocation, SDL_Renderer *renderer,
                       const float additionalLineSpacing = 0.f) : additionalLineSpacing(additionalLineSpacing) {
        // Only ASCII, 48pt. atlas should be 1024 * 512
        {
            FT_Library ft;

            if (FT_Init_FreeType(&ft)) {
                throw runtime_error("Could not init FreeType library");
            }

            constexpr int surfaceWidth = 1024;
            constexpr int surfaceHeight = 1024;

            FT_Face face;
            auto surface = unique_ptr<SDL_Surface, ywl::basic::function_t<SDL_FreeSurface>>{
                SDL_CreateRGBSurfaceWithFormat(0, surfaceWidth, surfaceHeight, 32, SDL_PIXELFORMAT_RGBA32)
            };

            assert(surface);
            // block
            {
                if (FT_New_Face(ft, fontLocation.data(), 0, &face)) {
                    FT_Done_FreeType(ft);
                    throw runtime_error("Could not open font");
                }


                lineHeight = fontSize;

                FT_Set_Pixel_Sizes(face, 0, fontSize);

                charInfos.resize(128);

                constexpr int height = fontSize;

                int x = 0, y = 0;

                for (int i = 0; i < 128; i++) {
                    FT_UInt glyphIndex = FT_Get_Char_Index(face, i);
                    if (!glyphIndex)
                        continue;

                    if (auto error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER)) {
                        AppLogMessage("Error loading glyph: {}"_fmt(error));
                        continue;
                    }

                    const auto slot = face->glyph;

                    const int actualWidth = static_cast<int>(slot->bitmap.width);
                    const int actualHeight = static_cast<int>(slot->bitmap.rows);

                    const int verticalBearing = -(slot->metrics.horiBearingY) >> 6;

                    if (x + actualWidth >= surfaceWidth) {
                        if (x > surfaceWidth) {
                            throw runtime_error("Font atlas too small");
                        }
                        x = 0;
                        y += height;
                        if (y + height >= surfaceHeight) {
                            throw runtime_error("Font atlas too small");
                        }
                    }

                    auto rectAtlas = SDL_Rect{x, y, actualWidth, actualHeight};

                    charInfos[i] = CharInfo{
                        rectAtlas, static_cast<int>(slot->advance.x >> 6),
                        static_cast<int>(slot->metrics.horiBearingX >> 6),
                        verticalBearing
                    };

                    auto *pixels = static_cast<Uint32 *>(surface->pixels);

                    for (int j = 0; j < actualHeight; j++) {
                        for (int k = 0; k < actualWidth; k++) {
                            const auto pixel = slot->bitmap.buffer[j * actualWidth + k];
                            pixels[(y + j) * surfaceWidth + x + k] = SDL_MapRGBA(
                                surface->format, pixel, pixel, pixel, pixel
                            );
                        }
                    }

                    x += actualWidth;
                }

                FT_Done_Face(face);
            }

            atlas = Texture::LoadFromSurface(renderer, surface.get());

            FT_Done_FreeType(ft);
        }
    }

    [[nodiscard]] const Texture &getAtlas() const {
        return atlas;
    }

    [[nodiscard]] const CharInfo &getCharInfo(int index) const {
        return charInfos[index];
    }

    [[nodiscard]] float getLineHeight() const {
        return static_cast<float>(lineHeight) * (1 + additionalLineSpacing);
    }

    struct Task {
        int count;
        int offsetX;
    };

public:
    struct DetailedTask {
        SDL_Texture *fontAtlasTexture{};
        // SDL_Rect hitBox;
        vector<SDL_Rect> targetRects;
        vector<SDL_Rect> sourceRects;

        union {
            int offsetX{};
            int w;
        };

        union {
            int offsetY{};
            int h;
        };
    };


    [[nodiscard]] DetailedTask generateDetailedTask(
        string_view string,
        const int maxWidth, const int height) const {
        float sizeMul = static_cast<float>(height) / static_cast<float>(lineHeight);

        // SDL_Rect returnHitBox = {
        //     startX + static_cast<int>(maxWidth / 2),
        //     static_cast<int>(static_cast<float>(startY) + additionalLineSpacing * fontSize * sizeMul), 0, 0
        // };


        vector<Task> tasks;
        // size_t stringLength = string.length();
        tasks.reserve(std::ceil(
            (static_cast<float>(this->getCharInfo('e').advance) * sizeMul
             + static_cast<float>(this->getCharInfo('M').advance) * sizeMul)
            * static_cast<float>(string.length()) / 2));

        int length = 0;
        int count = 0;

        for (const auto c: string) {
            const auto &charInfo = this->getCharInfo(c);

            if (c == '\n') {
                tasks.push_back({count + 1, (maxWidth - length) / 2});
                length = 0;
                count = 0;
                continue;
            }

            if (length + static_cast<int>(static_cast<float>(charInfo.advance) * sizeMul) > maxWidth) {
                tasks.push_back({count, (maxWidth - length) / 2});
                length = 0;
                count = 0;
            }

            if (!charInfo) {
                AppLogMessage("CharInfo not found for %d"_fmt(c));
                continue;
            }

            length += static_cast<int>(static_cast<float>(charInfo.advance) * sizeMul);
            count++;
        }

        if (count > 0) {
            tasks.push_back({count, (maxWidth - length) / 2});
        }
        // return {std::move(tasks), returnHitBox};

        // ==================================
        // float sizeMul = static_cast<float>(height) / static_cast<float>(lineHeight);

        auto fontAtlasTexture = this->getAtlas().get();

        int idx = 0;

        int currentY = 0;

        DetailedTask task;
        task.fontAtlasTexture = fontAtlasTexture;
        // task.hitBox = {startX, startY, 0, 0};
        task.targetRects.reserve(string.length());
        task.sourceRects.reserve(string.length());

        int minX = INT_MAX;
        int maxX = INT_MIN;
        int minY = INT_MAX;
        int maxY = INT_MIN;

        for (auto [count, offsetX]: tasks) {
            currentY += static_cast<int>(this->getLineHeight() * sizeMul);
            int currentX = offsetX;
            // returnHitBox.x = std::min(returnHitBox.x, currentX);
            for (int i = 0; i < count; i++) {
                const auto &charInfo = this->getCharInfo(string[idx + i]);

                assert(charInfo || string[idx + i] == '\n');
                if (string[idx + i] == '\n') {
                    continue;
                }

                auto targetRect = SDL_Rect(
                    currentX + static_cast<int>(static_cast<float>(charInfo.leftBearing) * sizeMul),
                    currentY + static_cast<int>(static_cast<float>(charInfo.upOffset) * sizeMul),
                    static_cast<int>(static_cast<float>(charInfo.posAtlas.w) * sizeMul),
                    static_cast<int>(static_cast<float>(charInfo.posAtlas.h) * sizeMul));

                task.targetRects.push_back(targetRect);
                task.sourceRects.push_back(charInfo.posAtlas);

                minX = std::min(minX, targetRect.x);
                maxX = std::max(maxX, targetRect.x + targetRect.w);
                minY = std::min(minY, targetRect.y);
                maxY = std::max(maxY, targetRect.y + targetRect.h);

                // SDL_RenderCopy(renderer, fontAtlasTexture, &charInfo.posAtlas,
                //                &targetRect);

                currentX += static_cast<int>(static_cast<float>(charInfo.advance) * sizeMul);
            }
            idx += count;
        }

        for (auto &&targetRect: task.targetRects) {
            targetRect.x -= minX;
            targetRect.y -= minY;
        }

        task.w = maxX - minX;
        task.h = maxY - minY;
        return task;
    }


    static void renderCenteredDetailedTask(const DetailedTask &task, SDL_Renderer *renderer) {
        for (size_t i = 0; i < task.targetRects.size(); i++) {
            SDL_Rect targetRect = task.targetRects[i];
            targetRect.x += task.offsetX;
            targetRect.y += task.offsetY;
            SDL_RenderCopy(renderer, task.fontAtlasTexture, &task.sourceRects[i], &targetRect);
        }
    }

    // void renderCenteredNoReturn(SDL_Renderer *renderer, string_view string, const int startX, const int startY,
    //                             const int maxWidth, const int height) const {
    //     auto detailedTask = generateDetailedTask(string, maxWidth, height);
    //     renderCenteredDetailedTask(detailedTask, renderer);
    //     // auto [tasks, returnHitBox] = calculateHints(string, startX, startY, maxWidth, height);
    //     // renderCenteredWithHint(renderer, string, startX, startY, maxWidth, height, tasks);
    //     // return returnHitBox;
    // }

    // void renderCenteredMultiLine(SDL_Renderer *renderer, string_view string, const int startX, const int startY,
    //                              const int maxWidth, const int height) const {
    //     vector<string_view> lines;
    //     string_view::size_type start = 0;
    //     string_view::size_type end = 0;
    //     while ((end = string.find('\n', start)) != string.npos) {
    //         lines.push_back(string.substr(start, end - start));
    //         start = end + 1;
    //     }
    //
    //     if (start < string.length()) {
    //         lines.push_back(string.substr(start));
    //     }
    //
    //     // SDL_Rect returnHitBox = {startX, startY, 0, 0};
    //     int count = 0;
    //
    //     for (const auto &line: lines) {
    //         // auto r = renderCenteredImpl(renderer, line, startX, returnHitBox.y, maxWidth, height);
    //         // auto [tasks, r] = calculateHints(line, startX, returnHitBox.y, maxWidth, height);
    //         // renderCenteredWithHint(renderer, line, startX, returnHitBox.y, maxWidth, height, tasks);
    //         auto detailedTask = generateDetailedTask(line, maxWidth, height);
    //         detailedTask.offsetX = startX;
    //         detailedTask.offsetY = startY + count * static_cast<int>(this->getLineHeight());
    //         renderCenteredDetailedTask(detailedTask, renderer);
    //
    //         // returnHitBox.h = hitBox.h + static_cast<int>(this->getLineHeight());
    //         // returnHitBox.w = std::max(returnHitBox.w, hitBox.w);
    //         // returnHitBox.x = std::min(returnHitBox.x, hitBox.x + startX);
    //         // returnHitBox.y += std::min(returnHitBox.y, hitBox.y + startY);
    //     }
    //     // return returnHitBox;
    // }
};
