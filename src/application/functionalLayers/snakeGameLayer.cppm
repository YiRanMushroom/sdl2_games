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

namespace application {
    export class SnakeGameLayer : public application::BasicMouseHandledLayer {
        function<void()> m_onDestruct;
        shared_ptr<string> imguiWindowHolder;

        inline static bool showSnakeGameLayerWindow = true;

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

        bool gameIsOver = false;
        // IVirtualMachineContextProvider *contextProvider;

        unordered_set<Position> emptyPositions;

    public:
        ~SnakeGameLayer() override {
            if (m_onDestruct) {
                m_onDestruct();
            }
        }

        SnakeGameLayer(function<void()> onDestruct, IVirtualMachineContextProvider &) : m_onDestruct( // NOLINT
            std::move(onDestruct)) {
            imguiWindowHolder = addImGuiDisplayWindow(
                "Snake Game Layer", &showSnakeGameLayerWindow,
                [this](bool *) {
                    static auto msg = "Snake Map Size: {}"_fmt(application::snakeSizeMap);
                    ImGui::Text("%s", msg.c_str());
                    for (auto &row: mapString) {
                        ImGui::Text("%s", row.c_str());
                    }
                });

            snakeMap.resize(mapSize);
            for (auto &row: snakeMap) {
                row.resize(mapSize);
            }

            // initial length is 3;
            assert(mapSize % 2 == 1);

            for (size_t i = 0; i < mapSize; i++) {
                for (size_t j = 0; j < mapSize; j++) {
                    emptyPositions.emplace(i, j);
                }
            }

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

            lastMoveTime = std::chrono::system_clock::now();
            durationSinceLastMove = std::chrono::system_clock::duration::zero();

            lastFoodTime = std::chrono::system_clock::now();
            durationSinceLastFood = std::chrono::system_clock::duration::zero();

            updateMapString();
        }

        vector<string> mapString{};

        // for test:
        void updateMapString() {
            vector<string> result(mapSize, string(mapSize, ' '));
            for (size_t y = 0; y < mapSize; y++) {
                for (size_t x = 0; x < mapSize; x++) {
                    if (snakeMap[x][y] != nullptr) {
                        if (snakeMap[x][y]->isFood) {
                            result[y][x] = 'F';
                        } else {
                            if (snakeMap[x][y]->frontState == SnakePartState::NONE) {
                                result[y][x] = 'H';
                            } else if (snakeMap[x][y]->backState == SnakePartState::NONE) {
                                result[y][x] = 'T';
                            } else {
                                result[y][x] = 'B';
                            }
                        }
                    } else {
                        result[y][x] = '.';
                    }
                }
            }
            mapString = std::move(result);
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
            bool over = advanceSnake();
            if (over) {
                gameIsOver = true;
            }
        }

        void generateFood() {
            if (gameIsOver || emptyPositions.empty()) {
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
                updateMapString();
            }
        }

        void generateFoodIfTime() {
            auto now = std::chrono::system_clock::now();
            durationSinceLastFood += now - lastFoodTime;
            lastFoodTime = now;
            while (durationSinceLastFood >= foodInterval) {
                durationSinceLastFood -= foodInterval;
                generateFood();
                updateMapString();
            }
        }

        void tryChangeTo(Direction newDirection) {
            auto [x, y] = snakeParts.front()->position;
            switch (newDirection) {
                case Direction::LEFT:
                    if (currentDirection != Direction::RIGHT && x > 0) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::RIGHT:
                    if (currentDirection != Direction::LEFT && x < mapSize - 1) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::UP:
                    if (currentDirection != Direction::DOWN && y > 0) {
                        nextDirection = newDirection;
                    }
                    return;
                case Direction::DOWN:
                    if (currentDirection != Direction::UP && y < mapSize - 1) {
                        nextDirection = newDirection;
                    }
                    return;
            }
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

                default: return original;
            }
        }

        void postEventListening(IVirtualMachineContextProvider &) override {
            if (!gameIsOver) {
                this->updateSnakeIfTime();
                this->generateFoodIfTime();
            }
        }

        void render(IVirtualMachineContextProvider &) override {
            // TODO: render the snake game
        }
    };
}
