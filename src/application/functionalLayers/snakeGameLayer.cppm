module;

#include <SDL2/SDL.h>
#include <imgui.h>
#include <utility>
#include <list>
#include <vector>
#include <chrono>
#include <cassert>

export module snakeGameLayer;

import layerFramework;
import std_essentials;
import snakeGameConfig;
import imguiWindows;
import std_overloads;
import randomGenerator;
import SDL2_Utilities;
import applicationConstants;
import textureAtlas;
import applicationResources;

namespace application {
    export class SnakeGameLayer : public application::WorkableLayer {
        function<void()> m_onDestruct;
        shared_ptr<string> imguiWindowHolder;

        inline static bool showSnakeGameLayerWindow = false;

        size_t mapSize = application::snakeSizeMap;

        enum class SnakePartState : uint8_t {
            NONE,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        struct Position {
            int x, y;

            [[nodiscard]] size_t hash_code() const {
                return std::bit_cast<size_t>(*this);
            }

            auto operator<=>(const Position &) const = default;
        };

        struct MapBlockInfo {
            Position position;
            bool isFood;
            SnakePartState frontState;
            SnakePartState backState;
            // SnakePartState should be updated after a new part is added
        };

        std::list<MapBlockInfo *> snakeParts;
        std::vector<std::vector<unique_ptr<MapBlockInfo> > > snakeMap;

        enum class Direction : uint8_t {
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        Direction currentDirection;

        Direction nextDirection;

        std::chrono::time_point<std::chrono::system_clock> lastMoveTime;
        const std::chrono::system_clock::duration moveInterval = std::chrono::milliseconds(500);
        std::chrono::system_clock::duration durationSinceLastMove;

        std::chrono::time_point<std::chrono::system_clock> lastFoodTime;
        const std::chrono::system_clock::duration foodInterval = std::chrono::seconds(3);
        std::chrono::system_clock::duration durationSinceLastFood;

        enum class GameState : uint8_t {
            Running,
            Paused,
            Over, // temporary
            Success,
            Score
        };

        GameState gameState;
        // IVirtualMachineContextProvider *contextProvider;

        unordered_set<Position> emptyPositions{};

        unordered_set<Position> foodPositions{};

        unique_ptr<SDL_Surface, SDL2_SurfaceDestructor> backGroundSurface;

        Texture backGroundTexture;

        int mapSizeScale{};

        SDL_Rect renderRect{};

        SimpleSquareTextureAtlas<5, 75> textureAtlas;

    public:
        ~SnakeGameLayer() override {
            if (m_onDestruct) {
                m_onDestruct();
            }
        }

        void initializeGame(const IVirtualMachineContextProvider &provider) {
            emptyPositions.clear();
            snakeParts.clear();
            foodPositions.clear();
            snakeMap.clear();

            snakeMap.resize(mapSize);
            for (auto &row: snakeMap) {
                row.resize(mapSize);
            }

            for (size_t i = 0; i < mapSize; i++) {
                for (size_t j = 0; j < mapSize; j++) {
                    emptyPositions.emplace(i, j);
                }
            } {
                const size_t xs[3]{mapSize / 2 + 1, mapSize / 2, mapSize / 2 - 1};
                const size_t y = mapSize / 2;

                // snakeParts.emplace_back(Position(xs[0], y), false, SnakePartState::NONE, SnakePartState::LEFT);
                // snakeParts.emplace_back(Position(xs[1], y), false, SnakePartState::RIGHT, SnakePartState::LEFT);
                // snakeParts.emplace_back(Position(xs[2], y), false, SnakePartState::RIGHT, SnakePartState::NONE);

                unique_ptr<MapBlockInfo> parts[3]{
                    make_unique<MapBlockInfo>(MapBlockInfo{
                        Position(xs[0], y), false, SnakePartState::NONE, SnakePartState::LEFT
                    }),
                    make_unique<MapBlockInfo>(MapBlockInfo{
                        Position(xs[1], y), false, SnakePartState::RIGHT, SnakePartState::LEFT
                    }),
                    make_unique<MapBlockInfo>(MapBlockInfo{
                        Position(xs[2], y),
                        false, SnakePartState::RIGHT, SnakePartState::NONE
                    })
                };

                snakeMap[xs[0]][y] = std::move(parts[0]);
                snakeMap[xs[1]][y] = std::move(parts[1]);
                snakeMap[xs[2]][y] = std::move(parts[2]);

                snakeParts.push_back(snakeMap[xs[0]][y].get());
                snakeParts.push_back(snakeMap[xs[1]][y].get());
                snakeParts.push_back(snakeMap[xs[2]][y].get());

                currentDirection = Direction::RIGHT;
                nextDirection = Direction::RIGHT;
            }

            recalculateSize(provider);

            gameState = GameState::Running;

            lastMoveTime = std::chrono::system_clock::now();
            durationSinceLastMove = std::chrono::system_clock::duration::zero();
            lastFoodTime = std::chrono::system_clock::now();
            durationSinceLastFood = std::chrono::system_clock::duration::zero();
        }

        SnakeGameLayer(function<void()> onDestruct, IVirtualMachineContextProvider &provider) : m_onDestruct( // NOLINT
            std::move(onDestruct)) {
            imguiWindowHolder = addImGuiDisplayWindow(
                "Snake Game Layer", &showSnakeGameLayerWindow,
                [this](bool *) {
                    static auto msg = "Snake Map Size: {}"_fmt(application::snakeSizeMap);
                    ImGui::Text("%s", msg.c_str());
                    // for (auto &row: mapString) {
                    //     ImGui::Text("%s", row.c_str());
                    // }
                });

            // initial length is 3;
            assert(mapSize % 2 == 1);

            backGroundSurface = decltype(backGroundSurface){
                SDL_CreateRGBSurface(0, mapSize, mapSize, 32,
                                     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)
            };
            // new scope
            {
                const auto &backGroundColor = application::constants::default_background_color;
                auto brighter = colors::Brighter(backGroundColor);
                auto darker = colors::Darker(backGroundColor);
                for (size_t x = 0; x < mapSize; x++) {
                    for (size_t y = 0; y < mapSize; y++) {
                        SDL_Rect rect(x, y, 1, 1);
                        if ((x + y) % 2 == 0) {
                            SDL_FillRect(backGroundSurface.get(), &rect,
                                         SDL_MapRGBA(backGroundSurface->format, brighter.r, brighter.g, brighter.b,
                                                     brighter.a));
                        } else {
                            SDL_FillRect(backGroundSurface.get(), &rect,
                                         SDL_MapRGBA(backGroundSurface->format, darker.r, darker.g, darker.b,
                                                     darker.a));
                        }
                    }
                }
            }


            textureAtlas = SimpleSquareTextureAtlas<5, 75>("resources/images/snakeGameAtlas.png",
                                                           provider.getRenderer());

            auto &&fontHolder = *openSansHolder;
            fontHolder.loadFont(constants::default_font_size);

            pauseTexture = fontHolder.getTextureBlended(constants::default_font_size, "Paused",
                                                        constants::default_font_color,
                                                        provider.getRenderer());

            reminderTexture = fontHolder.getTextureBlended(constants::default_font_size,
                                                           "Press ESC to exit, or press R to restart",
                                                           constants::default_font_color,
                                                           provider.getRenderer());

            initializeGame(provider);
        }

        void handlePause() {
            gameState = GameState::Paused;
            auto now = std::chrono::system_clock::now();
            durationSinceLastMove += now - lastMoveTime;
            durationSinceLastFood += now - lastFoodTime;
        }

        void cancelPause() {
            auto now = std::chrono::system_clock::now();
            lastMoveTime = now;
            lastFoodTime = now;
            gameState = GameState::Running;
        }

        struct idx {
            static constexpr inline size_t horizontalBody = 0;
            static constexpr inline size_t verticalBody = 1;
            static constexpr inline size_t upRightBody = 2;
            static constexpr inline size_t upLeftBody = 3;
            static constexpr inline size_t downRightBody = 4;
            static constexpr inline size_t downLeftBody = 5;
            static constexpr inline size_t tailUp = 6;
            static constexpr inline size_t tailRight = 7;
            static constexpr inline size_t tailDown = 8;
            static constexpr inline size_t tailLeft = 9;
            static constexpr inline size_t headUp = 10;
            static constexpr inline size_t headRight = 11;
            static constexpr inline size_t headDown = 12;
            static constexpr inline size_t headLeft = 13;
            static constexpr inline size_t food = 14;
        };

        SDL_Rect reminderRect{};
        SDL_Rect gameOverRect{};
        SDL_Rect pauseRect{};

        void recalculateSize(const IVirtualMachineContextProvider &provider) {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(provider.getWindow(), &windowWidth, &windowHeight);
            mapSizeScale = windowHeight / mapSize * 6 / 7;
            renderRect = SDL_Rect(static_cast<float>(windowHeight) / 14 + 50, static_cast<float>(windowHeight) / 14,
                                  mapSize * mapSizeScale,
                                  mapSize * mapSizeScale);
            backGroundTexture = Texture::LoadFromSurface(provider.getRenderer(), backGroundSurface.get());

            if (reminderTexture.get())
                reminderRect = SDL2_RectBuilder{provider.getWindow()}
                        .ofRelativePosition(0.5, 0.7)
                        .withRelativeHeight(0.1, reminderTexture.getAspectRatio())
                        .buildCentered();

            if (pauseTexture.get())
                pauseRect = SDL2_RectBuilder{provider.getWindow()}
                        .ofRelativePosition(0.5, 0.4)
                        .withRelativeHeight(0.2, pauseTexture.getAspectRatio())
                        .buildCentered();

            if (gameOverTexture.get())
                gameOverRect = SDL2_RectBuilder{provider.getWindow()}
                        .ofRelativePosition(0.5, 0.4)
                        .withRelativeHeight(0.2, gameOverTexture.getAspectRatio())
                        .buildCentered();
        }

        bool checkCollisionAndUpdateHead(MapBlockInfo newHead) {
            auto original = snakeParts.front();
            if (newHead.position.x < 0 || newHead.position.x >= mapSize || // NOLINT
                newHead.position.y < 0 || newHead.position.y >= mapSize) { // NOLINT
                return true;
            }

            auto &block = snakeMap[newHead.position.x][newHead.position.y]; // NOLINT

            auto doChange = [&] {
                switch (newHead.backState) {
                    case SnakePartState::LEFT:
                        original->frontState = SnakePartState::RIGHT;
                        break;
                    case SnakePartState::RIGHT:
                        original->frontState = SnakePartState::LEFT;
                        break;
                    case SnakePartState::UP:
                        original->frontState = SnakePartState::DOWN;
                        break;
                    case SnakePartState::DOWN:
                        original->frontState = SnakePartState::UP;
                        break;
                    default: throw std::runtime_error("Invalid back state");
                }

                snakeMap[newHead.position.x][newHead.position.y] = make_unique<MapBlockInfo>(newHead);
                snakeParts.push_front(snakeMap[newHead.position.x][newHead.position.y].get());
            };

            if (block != nullptr) {
                if (block->isFood) {
                    doChange();
                    foodPositions.erase(newHead.position);
                    return false;
                }
                return true;
            }

            doChange();
            removeTail();
            return false;
        }

        void removeTail() {
            auto &tail = snakeParts.back();
            emptyPositions.emplace(tail->position);
            snakeMap[tail->position.x][tail->position.y] = nullptr;
            snakeParts.pop_back();

            snakeParts.back()->backState = SnakePartState::NONE;
        }

        bool advanceSnake() { // if true, game is over
            auto &head = snakeParts.front();
            Position newPosition = head->position;
            SnakePartState newBackState{};
            switch (nextDirection) {
                case Direction::LEFT:
                    newPosition.x--;
                    newBackState = SnakePartState::RIGHT;
                    break;
                case Direction::RIGHT:
                    newPosition.x++;
                    newBackState = SnakePartState::LEFT;
                    break;
                case Direction::UP:
                    newPosition.y--;
                    newBackState = SnakePartState::DOWN;
                    break;
                case Direction::DOWN:
                    newPosition.y++;
                    newBackState = SnakePartState::UP;
                    break;
            }
            currentDirection = nextDirection;
            auto newHead = MapBlockInfo{
                newPosition, false, SnakePartState::NONE, newBackState
            };

            // bool isNewPositionFood = askPositionIsFood(newPosition);

            return checkCollisionAndUpdateHead(newHead);
        }

        void updateSnake() {
            if (advanceSnake()) {
                gameState = GameState::Over;
            }
        }

        void generateFood() {
            if (emptyPositions.empty()) {
                return;
            }

            int x, y;
            if (static_cast<float>(mapSize * mapSize) / static_cast<float>(emptyPositions.size()) > 0.05) {
                do {
                    x = getRandomNumber(0, static_cast<int>(mapSize) - 1);
                    y = getRandomNumber(0, static_cast<int>(mapSize) - 1);
                } while (!emptyPositions.contains({x, y}));
            } else {
                x = emptyPositions.begin()->x;
                y = emptyPositions.begin()->y;
            }

            emptyPositions.erase(Position(x, y));
            foodPositions.emplace(x, y);
            snakeMap[x][y] =
                    make_unique<MapBlockInfo>(
                        Position(x, y), true,
                        SnakePartState::NONE, SnakePartState::NONE
                    );
        }

        void updateSnakeIfTime() {
            auto now = std::chrono::system_clock::now();
            durationSinceLastMove += now - lastMoveTime;
            lastMoveTime = now;
            while (durationSinceLastMove >= moveInterval) {
                durationSinceLastMove -= moveInterval;
                updateSnake();
            }
        }

        void generateFoodIfTime() {
            auto now = std::chrono::system_clock::now();
            durationSinceLastFood += now - lastFoodTime;
            lastFoodTime = now;
            while (durationSinceLastFood >= foodInterval) {
                durationSinceLastFood -= foodInterval;
                generateFood();
            }
        }

        [[nodiscard]] bool notSnake(int x, int y) const {
            if (x < 0 || x >= mapSize || y < 0 || y >= mapSize) {
                return false;
            }
            return snakeMap[x][y] == nullptr || snakeMap[x][y]->isFood;
        }

        void tryChangeTo(Direction newDirection) {
            if (gameState != GameState::Running) {
                return;
            }

            auto [x, y] = snakeParts.front()->position;
            switch (newDirection) {
                case Direction::LEFT:
                    if (currentDirection != Direction::RIGHT && x > 0 && notSnake(x - 1, y)) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::RIGHT:
                    if (currentDirection != Direction::LEFT && x < mapSize - 1 && notSnake(x + 1, y)) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::UP:
                    if (currentDirection != Direction::DOWN && y > 0 && notSnake(x, y - 1)) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::DOWN:
                    if (currentDirection != Direction::UP && y < mapSize - 1 && notSnake(x, y + 1)) {
                        nextDirection = newDirection;
                    }
            }
            return;
        }

        bool onKeyDown(const SDL_Event &event, bool original,
                       IVirtualMachineContextProvider &provider) override {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    provider.getDeferredTasks().emplace(
                        [&provider, currentIterator = provider.getCurrentIterator()] {
                            provider.getLayers().erase(currentIterator);
                        });
                    return true;

                case SDLK_UP:
                    tryChangeTo(Direction::UP);
                    return true;

                case SDLK_DOWN:
                    tryChangeTo(Direction::DOWN);
                    return true;

                case SDLK_LEFT:
                    tryChangeTo(Direction::LEFT);
                    return true;

                case SDLK_RIGHT:
                    tryChangeTo(Direction::RIGHT);
                    return true;

                case SDLK_r:
                    initializeGame(provider);
                    return true;

                case SDLK_SPACE:
                    if (gameState == GameState::Paused) {
                        cancelPause();
                    } else {
                        handlePause();
                    }
                    return true;

                default: return original;
            }
        }

        bool onWindowEvent(const SDL_Event &event, bool original, IVirtualMachineContextProvider &provider) override {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                recalculateSize(provider);
            }
            return original;
        }

        void postEventListening(IVirtualMachineContextProvider &provider) override {
            if (gameState == GameState::Running) {
                updateSnakeIfTime();
                generateFoodIfTime();
            }

            if (gameState == GameState::Over) {
                calculateGameScore(provider);
                if (gameOverTexture.get())
                    gameOverRect = SDL2_RectBuilder{provider.getWindow()}
                            .ofRelativePosition(0.5, 0.4)
                            .withRelativeHeight(0.2, gameOverTexture.getAspectRatio())
                            .buildCentered();
            }
        }

        SDL_Rect renderPositionOf(const Position &position) {
            const auto &[x,y,w,h] = renderRect;
            return SDL_Rect{
                x + position.x * mapSizeScale,
                y + position.y * mapSizeScale,
                mapSizeScale, mapSizeScale
            };
        }

        void renderFood(const IVirtualMachineContextProvider &provider) {
            for (const auto &food: foodPositions) {
                textureAtlas.render<idx::food>(renderPositionOf(food), provider.getRenderer());
            }
        }

        void renderSnake(const IVirtualMachineContextProvider &provider) {
            for (const auto &part: snakeParts) {
                const auto &[position, _, frontState, backState] = *part;
                switch (frontState) {
                    case SnakePartState::NONE: {
                        switch (backState) {
                            case SnakePartState::NONE:
                                throw std::runtime_error("Invalid snake part state");
                                break;
                            case SnakePartState::LEFT:
                                textureAtlas.render<idx::headRight>(renderPositionOf(position), provider.getRenderer());
                                break;
                            case SnakePartState::RIGHT:
                                textureAtlas.render<idx::headLeft>(renderPositionOf(position), provider.getRenderer());
                                break;
                            case SnakePartState::UP:
                                textureAtlas.render<idx::headDown>(renderPositionOf(position), provider.getRenderer());
                                break;
                            case SnakePartState::DOWN:
                                textureAtlas.render<idx::headUp>(renderPositionOf(position), provider.getRenderer());
                                break;
                        }
                        break;
                    }

                    case SnakePartState::LEFT: {
                        switch (backState) {
                            case SnakePartState::NONE:
                                textureAtlas.render<idx::tailLeft>(renderPositionOf(position), provider.getRenderer());
                                break;
                            case SnakePartState::LEFT:
                                throw std::runtime_error("Invalid snake part state");
                                break;
                            case SnakePartState::RIGHT:
                                textureAtlas.render<idx::horizontalBody>(renderPositionOf(position),
                                                                         provider.getRenderer());
                                break;
                            case SnakePartState::UP:
                                textureAtlas.render<idx::upLeftBody>(renderPositionOf(position),
                                                                     provider.getRenderer());
                                break;
                            case SnakePartState::DOWN:
                                textureAtlas.render<idx::downLeftBody>(renderPositionOf(position),
                                                                       provider.getRenderer());
                                break;
                        }
                        break;

                    case SnakePartState::RIGHT: {
                        switch (backState) {
                            case SnakePartState::NONE:
                                textureAtlas.render<idx::tailRight>(renderPositionOf(position),
                                                                    provider.getRenderer());
                                break;
                            case SnakePartState::LEFT:
                                textureAtlas.render<idx::horizontalBody>(renderPositionOf(position),
                                                                         provider.getRenderer());
                                break;
                            case SnakePartState::RIGHT:
                                throw std::runtime_error("Invalid snake part state");
                                break;
                            case SnakePartState::UP:
                                textureAtlas.render<idx::upRightBody>(renderPositionOf(position),
                                                                      provider.getRenderer());
                                break;
                            case SnakePartState::DOWN:
                                textureAtlas.render<idx::downRightBody>(renderPositionOf(position),
                                                                        provider.getRenderer());
                                break;
                        }
                        break;
                    }

                    case SnakePartState::UP: {
                        switch (backState) {
                            case SnakePartState::NONE:
                                textureAtlas.render<idx::tailUp>(renderPositionOf(position), provider.getRenderer());
                                break;
                            case SnakePartState::LEFT:
                                textureAtlas.render<idx::upLeftBody>(renderPositionOf(position),
                                                                     provider.getRenderer());
                                break;
                            case SnakePartState::RIGHT:
                                textureAtlas.render<idx::upRightBody>(renderPositionOf(position),
                                                                      provider.getRenderer());
                                break;
                            case SnakePartState::UP:
                                throw std::runtime_error("Invalid snake part state");
                                break;
                            case SnakePartState::DOWN:
                                textureAtlas.render<idx::verticalBody>(renderPositionOf(position),
                                                                       provider.getRenderer());
                                break;
                        }
                        break;
                    }

                    case SnakePartState::DOWN: {
                        switch (backState) {
                            case SnakePartState::NONE:
                                textureAtlas.render<idx::tailDown>(renderPositionOf(position),
                                                                   provider.getRenderer());
                                break;
                            case SnakePartState::LEFT:
                                textureAtlas.render<idx::downLeftBody>(renderPositionOf(position),
                                                                       provider.getRenderer());
                                break;
                            case SnakePartState::RIGHT:
                                textureAtlas.render<idx::downRightBody>(renderPositionOf(position),
                                                                        provider.getRenderer());
                                break;
                            case SnakePartState::UP:
                                textureAtlas.render<idx::verticalBody>(renderPositionOf(position),
                                                                       provider.getRenderer());
                                break;
                            case SnakePartState::DOWN:
                                throw std::runtime_error("Invalid snake part state");
                                break;
                        }
                        break;
                    }
                    }
                }
            }
        }

        Texture reminderTexture;
        Texture gameOverTexture;
        Texture pauseTexture;

        void calculateGameScore(const IVirtualMachineContextProvider &provider) {
            size_t length = snakeParts.size();
            if (length == mapSize * mapSize) {
                gameState = GameState::Success;
            } else {
                gameState = GameState::Score;
            }

            auto &&fontHolder = *openSansHolder;

            gameOverTexture = fontHolder.getTextureBlended(constants::default_font_size,
                                                           gameState == GameState::Success
                                                               ? "You Win!"
                                                               : "Your Score: {}"_fmt(length),
                                                           constants::default_font_color,
                                                           provider.getRenderer());
        }

        void render(IVirtualMachineContextProvider &provider) override {
            SDL2_RenderTextureRect(provider.getRenderer(), backGroundTexture, renderRect);
            renderFood(provider);
            renderSnake(provider);


            switch (gameState) {
                case GameState::Paused:
                    SDL2_RenderTextureRect(provider.getRenderer(), pauseTexture, pauseRect);
                    break;
                case GameState::Over:
                case GameState::Success:
                case GameState::Score:
                    SDL2_RenderTextureRect(provider.getRenderer(), gameOverTexture, gameOverRect);
                    SDL2_RenderTextureRect(provider.getRenderer(), reminderTexture, reminderRect);
                    break;
                default: break;
            }
        }
    };
}
