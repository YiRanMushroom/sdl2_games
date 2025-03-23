module;

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <imgui.h>

export module snakeGameLayer;

import ywl.prelude;
import snakeGameConfig;
import imguiWindows;
import randomGenerator;
import SDL2_Utilities;
import applicationConstants;
import textureAtlas;
import applicationResources;
import freeTypeFont;
import workableLayer;

using std::unique_ptr;
using std::function;
using std::shared_ptr;
using std::string;
using std::unordered_set;
using std::make_unique;
using std::string_view;

namespace application {
    export class SnakeGameLayer : public application::WorkableLayer {
        function<void()> m_onDestruct;
        shared_ptr<string> imguiWindowHolder;

        inline static bool showSnakeGameLayerWindow = false;

        size_t mapSize;

        enum class SnakePartState : uint8_t {
            NONE,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        struct Position {
            int x, y;

            [[nodiscard]] size_t hash_code(ywl_overload_flag_t) const {
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
        std::chrono::system_clock::duration moveInterval;
        std::chrono::system_clock::duration durationSinceLastMove;

        std::chrono::time_point<std::chrono::system_clock> lastFoodTime;
        std::chrono::system_clock::duration foodGenerateInterval;
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

        unique_ptr<SDL_Surface, ywl::basic::function_t<SDL_FreeSurface> > backGroundSurface;

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

            gameOverStringProvider = nullptr;

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

                updateSnakeLengthString(provider);

                emptyPositions.erase(Position(xs[0], y));
                emptyPositions.erase(Position(xs[1], y));
                emptyPositions.erase(Position(xs[2], y));

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

        void updateSnakeLengthString(const IVirtualMachineContextProvider &provider) {
            snakeLengthString = "Snake Length: {}"_fmt(snakeParts.size());
            int windowWidth, windowHeight;
            SDL_GetWindowSize(provider.getWindow(), &windowWidth, &windowHeight);
            snakeLengthTask = freeTypeFontHolder->generateDetailedTask(
                snakeLengthString, windowWidth / 4, windowHeight / 20);
            snakeLengthTask.offsetX = (windowWidth - snakeLengthTask.w) * 5 / 6;
            snakeLengthTask.offsetY = (windowHeight - snakeLengthTask.h) * 3 / 4;
        }

        SnakeGameLayer(function<void()> onDestruct, IVirtualMachineContextProvider &provider) : m_onDestruct( // NOLINT
            std::move(onDestruct)) {
            imguiWindowHolder = addImGuiDisplayWindow(
                "Snake Game Layer", &showSnakeGameLayerWindow,
                [this](bool *) {
                    static auto msg = "Snake Map Size: {}"_fmt(application::config::snakeGame::sizeMap);
                    ImGui::Text("%s", msg.c_str());
                    // for (auto &row: mapString) {
                    //     ImGui::Text("%s", row.c_str());
                    // }
                });

            mapSize = application::config::snakeGame::sizeMap;
            moveInterval = std::chrono::milliseconds(application::config::snakeGame::moveInterval);
            foodGenerateInterval = std::chrono::milliseconds(application::config::snakeGame::foodGenerateInterval);

            // initial length is 3;
            // assert(mapSize % 2 == 1);

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

            // pauseTexture = fontHolder.getTextureBlended(constants::default_font_size, "Paused",
            //                                             constants::default_font_color,
            //                                             provider.getRenderer());
            //
            // reminderTexture = fontHolder.getTextureBlended(constants::default_font_size,
            //                                                "Press ESC to exitor press R to restart",
            //                                                constants::default_font_color,
            //                                                provider.getRenderer());

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

        // std::pair<int, int> pauseOffset, reminderOffset, gameOverOffset;

        FreeTypeFontHolder::DetailedTask pauseTask, reminderTask, gameOverTask, tipsTask, snakeLengthTask;

        string snakeLengthString;

        function<string_view()> gameOverStringProvider;

        void recalculateSize(const IVirtualMachineContextProvider &provider) {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(provider.getWindow(), &windowWidth, &windowHeight);
            mapSizeScale = windowHeight / mapSize * 6 / 7;
            renderRect = SDL_Rect(static_cast<float>(windowHeight) / 14 + 50, static_cast<float>(windowHeight) / 14,
                                  mapSize * mapSizeScale,
                                  mapSize * mapSizeScale);
            backGroundTexture = Texture::LoadFromSurface(provider.getRenderer(), backGroundSurface.get());

            // if (reminderTexture.get())
            //     reminderRect = SDL2_RectBuilder{provider.getWindow()}
            //             .ofRelativePosition(0.5, 0.7)
            //             .withRelativeHeight(0.1, reminderTexture.getAspectRatio())
            //             .buildCentered();


            // reminderOffset = {
            //     (windowWidth - std::get<1>(remainderTaskInfos)) / 2,
            //     (windowHeight - std::get<2>(remainderTaskInfos)) * 2 / 3
            // };

            reminderTask = freeTypeFontHolder->generateDetailedTask(
                "Press ESC to exit\nOr press R to restart", windowWidth, windowHeight / 10);
            reminderTask.offsetX = (windowWidth - reminderTask.w) / 2;
            reminderTask.offsetY = (windowHeight - reminderTask.h) * 2 / 3;

            // if (pauseTexture.get())
            //     pauseRect = SDL2_RectBuilder{provider.getWindow()}
            //             .ofRelativePosition(0.5, 0.4)
            //             .withRelativeHeight(0.2, pauseTexture.getAspectRatio())
            //             .buildCentered();
            // auto pauseTaskInfos = freeTypeFontHolder->
            //         generateDetailedTask("Paused", windowWidth, windowHeight / 3);
            // pauseTask = std::move(std::get<0>(pauseTaskInfos));
            // pauseOffset = {
            //     (windowWidth - std::get<1>(pauseTaskInfos)) / 2,
            //     (windowHeight - std::get<2>(pauseTaskInfos)) / 2
            // };

            pauseTask = freeTypeFontHolder->generateDetailedTask("Paused", windowWidth, windowHeight / 3);
            pauseTask.offsetX = (windowWidth - pauseTask.w) / 2;
            pauseTask.offsetY = (windowHeight - pauseTask.h) / 2;

            // if (gameOverTexture.get())
            //     gameOverRect = SDL2_RectBuilder{provider.getWindow()}
            //             .ofRelativePosition(0.5, 0.4)
            //             .withRelativeHeight(0.2, gameOverTexture.getAspectRatio())
            //             .buildCentered();
            if (gameOverStringProvider) {
                // auto gameOverTaskInfos = freeTypeFontHolder->generateDetailedTask(
                //     gameOverStringProvider(), windowWidth, windowHeight / 5);
                // gameOverTask = std::move(std::get<0>(gameOverTaskInfos));
                // gameOverOffset = {
                //     (windowWidth - std::get<1>(gameOverTaskInfos)) / 2,
                //     (windowHeight - std::get<2>(gameOverTaskInfos)) / 3
                // };

                gameOverTask = freeTypeFontHolder->generateDetailedTask(
                    gameOverStringProvider(), windowWidth, windowHeight / 5);
                gameOverTask.offsetX = (windowWidth - gameOverTask.w) / 2;
                gameOverTask.offsetY = (windowHeight - gameOverTask.h) / 3;
            }

            tipsTask = freeTypeFontHolder->generateDetailedTask(
                "Press SPACE to pause\nPress ESC to exit\nPress R to restart\nPress arrow keys to\nchange direction",
                windowWidth / 4, windowHeight / 25);
            tipsTask.offsetX = (windowWidth - tipsTask.w) * 5 / 6;
            tipsTask.offsetY = (windowHeight - tipsTask.h) / 3;
        }

        bool checkCollisionAndUpdateHead(MapBlockInfo newHead, const IVirtualMachineContextProvider &provider) {
            auto original = snakeParts.front();
            if (newHead.position.x < 0 || newHead.position.x >= mapSize || // NOLINT
                newHead.position.y < 0 || newHead.position.y >= mapSize) {
                // NOLINT
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
                emptyPositions.erase(newHead.position);

                if (emptyPositions.empty() && foodPositions.empty()) {
                    gameState = GameState::Over;
                }
            };

            if (block != nullptr) {
                if (block->isFood) {
                    doChange();
                    foodPositions.erase(newHead.position);
                    updateSnakeLengthString(provider);
                    Mix_PlayChannel(-1, mixChunks["snake/eat"].get(), 0);
                    return false;
                }

                if (block->backState != SnakePartState::NONE) {
                    return true;
                }
            }

            removeTail();
            doChange();
            return false;
        }

        void removeTail() {
            auto &tail = snakeParts.back();
            emptyPositions.emplace(tail->position);
            snakeMap[tail->position.x][tail->position.y] = nullptr;
            snakeParts.pop_back();

            snakeParts.back()->backState = SnakePartState::NONE;
        }

        bool advanceSnake(const IVirtualMachineContextProvider &provider) {
            // if true, game is over
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

            return checkCollisionAndUpdateHead(newHead, provider);
        }

        void updateSnake(const IVirtualMachineContextProvider &provider) {
            if (advanceSnake(provider)) {
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

        void updateSnakeIfTime(const IVirtualMachineContextProvider &provider) {
            auto now = std::chrono::system_clock::now();
            durationSinceLastMove += now - lastMoveTime;
            lastMoveTime = now;
            while (durationSinceLastMove >= moveInterval) {
                durationSinceLastMove -= moveInterval;
                updateSnake(provider);
            }
        }

        void generateFoodIfTime() {
            auto now = std::chrono::system_clock::now();
            durationSinceLastFood += now - lastFoodTime;
            lastFoodTime = now;
            while (durationSinceLastFood >= foodGenerateInterval) {
                durationSinceLastFood -= foodGenerateInterval;
                generateFood();
            }
        }

        [[nodiscard]] bool notSnake(int x, int y) const {
            if (x < 0 || x >= mapSize || y < 0 || y >= mapSize) {
                return false;
            }
            return snakeMap[x][y] == nullptr || snakeMap[x][y]->isFood || snakeMap[x][y]->backState
                   == SnakePartState::NONE;
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
                updateSnakeIfTime(provider);
                generateFoodIfTime();
            }

            if (gameState == GameState::Over) {
                calculateGameScore(provider);

                int windowWidth, windowHeight;
                SDL_GetWindowSize(provider.getWindow(), &windowWidth, &windowHeight);

                // auto gameOverTaskInfos = freeTypeFontHolder->generateDetailedTask(
                //     gameOverStringProvider(), windowWidth, windowHeight / 5);
                //
                // gameOverTask = std::move(std::get<0>(gameOverTaskInfos));
                // gameOverOffset = {
                //     (windowWidth - std::get<1>(gameOverTaskInfos)) / 2,
                //     (windowHeight - std::get<2>(gameOverTaskInfos)) * 2 / 5
                // };
                // if (gameOverTexture.get())
                //     gameOverRect = SDL2_RectBuilder{provider.getWindow()}
                //             .ofRelativePosition(0.5, 0.4)
                //             .withRelativeHeight(0.2, gameOverTexture.getAspectRatio())
                //             .buildCentered();
                gameOverTask = freeTypeFontHolder->generateDetailedTask(
                    gameOverStringProvider(), windowWidth, windowHeight / 5);
                gameOverTask.offsetX = (windowWidth - gameOverTask.w) / 2;
                gameOverTask.offsetY = (windowHeight - gameOverTask.h) * 2 / 5;
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

        // Texture reminderTexture;
        // Texture gameOverTexture;
        // Texture pauseTexture;

        void calculateGameScore(const IVirtualMachineContextProvider &provider) {
            size_t length = snakeParts.size();
            if (length == mapSize * mapSize) {
                gameState = GameState::Success;
                Mix_PlayChannel(-1, mixChunks["snake/success"].get(), 0);
            } else {
                gameState = GameState::Score;
                Mix_PlayChannel(-1, mixChunks["snake/game_over"].get(), 0);
            }

            auto &&fontHolder = *openSansHolder;

            gameOverStringProvider = [string = gameState == GameState::Success
                                                   ? string{"You Win!"}
                                                   : "Your Score: {}"_fmt(length) ]() -> string_view {
                return string.c_str();
            };

            // gameOverTexture = fontHolder.getTextureBlended(constants::default_font_size,
            //                                                gameState == GameState::Success
            //                                                    ? "You Win!"
            //                                                    : "Your Score: {}"_fmt(length),
            //                                                constants::default_font_color,
            //                                                provider.getRenderer());
        }

        void renderTips(const IVirtualMachineContextProvider &provider) const {
            auto pushColor = application::colors::PushColor{
                tipsTask.fontAtlasTexture, SDL_Color{255, 255, 255, 255}
            };
            FreeTypeFontHolder::renderCenteredDetailedTask(tipsTask, provider.getRenderer());
        }

        void renderSnakeLength(const IVirtualMachineContextProvider &provider) const {
            auto pushColor = application::colors::PushColor{
                snakeLengthTask.fontAtlasTexture, SDL_Color{0, 255, 0, 255}
            };
            FreeTypeFontHolder::renderCenteredDetailedTask(snakeLengthTask, provider.getRenderer());
        }

        void render(IVirtualMachineContextProvider &provider) override {
            SDL2_RenderTextureRect(provider.getRenderer(), backGroundTexture, renderRect);
            renderFood(provider);
            renderSnake(provider);
            renderTips(provider);

            if (gameState == GameState::Running || gameState == GameState::Paused) {
                renderSnakeLength(provider);
            }

            switch (gameState) {
                case GameState::Paused: {
                    auto pushColor = application::colors::PushColor{
                        pauseTask.fontAtlasTexture, SDL_Color{255, 255, 255, 127}
                    };
                    FreeTypeFontHolder::renderCenteredDetailedTask(pauseTask, provider.getRenderer());
                    break;
                }
                case GameState::Over:
                    throw std::runtime_error("Impossible to reach here");
                case GameState::Success:
                case GameState::Score: {
                    {
                        auto pushColor = application::colors::PushColor{
                            gameOverTask.fontAtlasTexture, SDL_Color{255, 0, 255, 255}
                        };
                        FreeTypeFontHolder::renderCenteredDetailedTask(gameOverTask, provider.getRenderer());
                    } {
                        auto pushColor = application::colors::PushColor{
                            pauseTask.fontAtlasTexture, SDL_Color{255, 255, 255, 255}
                        };

                        FreeTypeFontHolder::renderCenteredDetailedTask(reminderTask, provider.getRenderer());
                    }
                    break;
                }
                default: break;
            }
        }
    };
}
