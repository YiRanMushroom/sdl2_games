module;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <iostream>
#include <list>
#include <queue>

#include <AppMacros.h>

export module SDL2_ImGui_VM;

import SDL2_EventListener;
import std_essentials;
import workableLayer;
import applicationSharedState;
import imguiUtilities;

struct SDL2_Context_Holder {
    SDL2_Context_Holder() {
        std::cout << "Initializing SDL2" << std::endl;

        try {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
                throw runtime_error(SDL_GetError());
            }

            if (TTF_Init() != 0) {
                throw runtime_error(TTF_GetError());
            }
        } catch (const runtime_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            throw e;
        }

        // std::cout << "SDL2 Initialized" << std::endl;
    }

    ~SDL2_Context_Holder() {
        std::cout << "SDL2 Quitting" << std::endl;
        TTF_Quit();
        SDL_Quit();
    }

    SDL2_Context_Holder(const SDL2_Context_Holder &) = delete;

    SDL2_Context_Holder &operator=(const SDL2_Context_Holder &) = delete;
};

export struct SDL2_Window_Destructor {
    static void operator()(SDL_Window *window) {
        SDL_DestroyWindow(window);
    }
};

export struct SDL2_Renderer_Destructor {
    static void operator()(SDL_Renderer *renderer) {
        SDL_DestroyRenderer(renderer);
    }
};

namespace application {
    export class SDL2_ImGUi_VM implements public application::IVirtualMachineContextProvider {
    private:
        // order is sensitive
        // contextHolder must be destroyed last
        unique_ptr<SDL2_Context_Holder> contextHolder; // NOLINT
        unique_ptr<SDL_Window, SDL2_Window_Destructor> window;
        unique_ptr<SDL_Renderer, SDL2_Renderer_Destructor> renderer;
        std::queue<function<void()> > deferredTasks{};

    public:
        LayerList layers = LayerList{};
        LayerListIterator currentIterator{};
        LayerListReverseIterator currentReverseIterator{};

    public:
        inline static SDL2_ImGUi_VM *instance = nullptr;

    public:
        operator SDL_Window *() const {
            return window.get();
        }

        operator SDL_Renderer *() const {
            return renderer.get();
        }

        [[nodiscard]] SDL_Window *getWindow() const override {
            return window.get();
        }

        [[nodiscard]] SDL_Renderer *getRenderer() const override {
            return renderer.get();
        }

        void setWindow(unique_ptr<SDL_Window, SDL2_Window_Destructor> window) {
            this->window = std::move(window);
        }

        void setRenderer(unique_ptr<SDL_Renderer, SDL2_Renderer_Destructor> renderer) {
            this->renderer = std::move(renderer);
        }

        SDL2_ImGUi_VM(unique_ptr<SDL2_Context_Holder> contextHolder,
                      unique_ptr<SDL_Window, SDL2_Window_Destructor> window,
                      unique_ptr<SDL_Renderer, SDL2_Renderer_Destructor> renderer)
            : contextHolder(std::move(contextHolder)),
              window(std::move(window)),
              renderer(std::move(renderer)) {
            if (instance) {
                throw runtime_error("SDL2_ImGUi_VM instance already exists");
            }
            instance = this;
            if (!this->window) {
                std::cerr << "Error: " << SDL_GetError() << std::endl;
                throw runtime_error(SDL_GetError());
            }

            if (!this->renderer) {
                std::cerr << "Error: " << SDL_GetError() << std::endl;
                throw runtime_error(SDL_GetError());
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void) io;
            ImGui::StyleColorsClassic();

            ImGui_ImplSDL2_InitForSDLRenderer(this->window.get(), this->renderer.get());
            ImGui_ImplSDLRenderer2_Init(this->renderer.get());
        }

        void preEventListening() {
            for (currentIterator = layers.begin(); currentIterator != layers.end(); ++currentIterator) {
                if ((*currentIterator)->enabled())
                    (*currentIterator)->preEventListening(*this);
            }
        }

        void handleEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                bool handled = false;

                for (currentIterator = layers.begin(); currentIterator != layers.end(); ++currentIterator) {
                    if ((*currentIterator)->enabled())
                        handled = (*currentIterator)->handle(event, handled, *this);
                }
            }
        }

        void postEventListening() {
            for (currentIterator = layers.begin(); currentIterator != layers.end(); ++currentIterator) {
                if ((*currentIterator)->enabled())
                    (*currentIterator)->postEventListening(*this);
            }
        }

        void render() {
            for (currentReverseIterator = layers.rbegin(); currentReverseIterator != layers.rend();
                 ++currentReverseIterator) {
                if ((*currentReverseIterator)->enabled())
                    (*currentReverseIterator)->render(*this);
            }
        }

        void handleDeferredTasks() {
            while (!deferredTasks.empty()) {
                deferredTasks.front()();
                deferredTasks.pop();
            }
        }

        ~SDL2_ImGUi_VM() {
            ImGui_ImplSDLRenderer2_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();

            instance = nullptr;
        }

        LayerList &getLayers() override {
            return layers;
        }

        LayerListIterator &getCurrentIterator() override {
            return currentIterator;
        }

        LayerListReverseIterator &getCurrentReverseIterator() override {
            return currentReverseIterator;
        }

        std::queue<function<void()> > &getDeferredTasks() override {
            return deferredTasks;
        }
    };

    export class SDL2_ImGUi_VM_Factory {
        optional<const char *> m_title;
        optional<int> m_x = SDL_WINDOWPOS_CENTERED;
        optional<int> m_y = SDL_WINDOWPOS_CENTERED;
        optional<int> m_width = 1280;
        optional<int> m_height = 720;
        optional<Uint32> m_windowFlags = SDL_WINDOW_SHOWN;

        optional<int> m_rendererIndex = -1;
        optional<Uint32> m_rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

        // shared_ptr<SDL2_EventListener> m_eventListener = make_shared<SDL2_EventListener>();

    public:
        SDL2_ImGUi_VM_Factory() = default;

        decltype(auto) title(this auto &&self, const char *title) {
            self.m_title = title;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) x(this auto &&self, int x) {
            self.m_x = x;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) y(this auto &&self, int y) {
            self.m_y = y;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) position(this auto &&self, int x, int y) {
            self.m_x = x;
            self.m_y = y;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) width(this auto &&self, int width) {
            self.m_width = width;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) height(this auto &&self, int height) {
            self.m_height = height;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) size(this auto &&self, int width, int height) {
            self.m_width = width;
            self.m_height = height;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) windowFlags(this auto &&self, Uint32 windowFlags) {
            self.m_windowFlags = windowFlags;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) rendererIndex(this auto &&self, int rendererIndex) {
            self.m_rendererIndex = rendererIndex;
            return std::forward<decltype(self)>(self);
        }

        decltype(auto) rendererFlags(this auto &&self, Uint32 rendererFlags) {
            self.m_rendererFlags = rendererFlags;
            return std::forward<decltype(self)>(self);
        }

        // decltype(auto) eventListener(this auto &&self, shared_ptr<SDL2_EventListener> eventListener) {
        //     self.m_eventListener = std::move(eventListener);
        //     return std::forward<decltype(self)>(self);
        // }

        SDL2_ImGUi_VM build() {
            unique_ptr<SDL2_Context_Holder> contextHolder = make_unique<SDL2_Context_Holder>();

            unique_ptr<SDL_Window, SDL2_Window_Destructor> window(
                SDL_CreateWindow(m_title.value(), m_x.value(), m_y.value(), m_width.value(), m_height.value(),
                                 m_windowFlags.value())
            );

            unique_ptr<SDL_Renderer, SDL2_Renderer_Destructor> renderer(
                SDL_CreateRenderer(window.get(), m_rendererIndex.value(), m_rendererFlags.value())
            );

            return {
                std::move(contextHolder), std::move(window), std::move(renderer)
            };
        }
    };
}
