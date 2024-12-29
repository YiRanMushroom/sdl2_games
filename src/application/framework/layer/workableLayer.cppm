module;

#include <AppMacros.h>
#include <SDL2/SDL.h>
#include <list>
#include <queue>

export module workableLayer;

import std_essentials;

namespace application {
    export class WorkableLayer;
}

export {
    using LayerList = std::list<unique_ptr<application::WorkableLayer> >;
    using LayerListIterator = LayerList::iterator;
    using LayerListReverseIterator = LayerList::reverse_iterator;
}

namespace application {
    export class IVirtualMachineContextProvider {
    public:
        virtual ~IVirtualMachineContextProvider() = default;

        [[nodiscard]] virtual SDL_Window *getWindow() const = 0;

        [[nodiscard]] virtual SDL_Renderer *getRenderer() const = 0;

        virtual std::queue<function<void()> > &getDeferredTasks() = 0;

        virtual LayerList &getLayers() = 0;

        virtual LayerListIterator &getCurrentIterator() = 0;

        virtual LayerListReverseIterator &getCurrentReverseIterator() = 0;
    };

    export class ContextEventListener {
    protected:
        virtual bool onFirstEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onQuit(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAppTerminating(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAppLowMemory(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAppWillEnterBackground(const SDL_Event &, bool original,
                                              IVirtualMachineContextProvider &provider) { return original; }

        virtual bool
        onAppDidEnterBackground(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAppWillEnterForeground(const SDL_Event &, bool original,
                                              IVirtualMachineContextProvider &provider) { return original; }

        virtual bool
        onAppDidEnterForeground(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onLocaleChanged(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDisplayEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onWindowEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onSysWMEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onKeyDown(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onKeyUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onTextEditing(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onTextInput(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onKeyMapChanged(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onTextEditingExt(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onMouseMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onMouseButtonDown(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onMouseButtonUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onMouseWheel(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyAxisMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyBallMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyHatMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyButtonDown(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyButtonUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyDeviceAdded(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyDeviceRemoved(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onJoyBatteryUpdated(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool
        onControllerAxisMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool
        onControllerButtonDown(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onControllerButtonUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool
        onControllerDeviceAdded(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onControllerDeviceRemoved(const SDL_Event &, bool original,
                                               IVirtualMachineContextProvider &provider) { return original; }

        virtual bool onControllerDeviceRemapped(const SDL_Event &, bool original,
                                                IVirtualMachineContextProvider &provider) { return original; }

        virtual bool onControllerTouchPadDown(const SDL_Event &, bool original,
                                              IVirtualMachineContextProvider &provider) { return original; }

        virtual bool onControllerTouchPadMotion(const SDL_Event &, bool original,
                                                IVirtualMachineContextProvider &provider) { return original; }

        virtual bool
        onControllerTouchPadUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onControllerSensorUpdate(const SDL_Event &, bool original,
                                              IVirtualMachineContextProvider &provider) { return original; }

        virtual bool onControllerUpdateCompleteReservedForSDL3(const SDL_Event &, bool original,
                                                               IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onControllerSteamHandleUpdated(const SDL_Event &, bool original,
                                                    IVirtualMachineContextProvider &provider) { return original; }

        virtual bool onFingerDown(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onFingerUp(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onFingerMotion(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDollarGesture(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDollarRecord(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onMultiGesture(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onClipboardUpdate(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDropFile(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDropText(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDropBegin(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onDropComplete(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAudioDeviceAdded(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onAudioDeviceRemoved(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onSensorUpdate(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onRenderTargetsReset(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onRenderDeviceReset(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onPollSentinel(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onUserEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

        virtual bool onLastEvent(const SDL_Event &, bool original, IVirtualMachineContextProvider &provider) {
            return original;
        }

    public:
        virtual ~ContextEventListener() = default;

        virtual bool handle(const SDL_Event &event, bool prevHandled, IVirtualMachineContextProvider &provider) {
            switch (event.type) {
                case SDL_FIRSTEVENT: return onFirstEvent(event, prevHandled, provider);
                case SDL_QUIT: return onQuit(event, prevHandled, provider);
                case SDL_APP_TERMINATING: return onAppTerminating(event, prevHandled, provider);
                case SDL_APP_LOWMEMORY: return onAppLowMemory(event, prevHandled, provider);
                case SDL_APP_WILLENTERBACKGROUND: return onAppWillEnterBackground(event, prevHandled, provider);
                case SDL_APP_DIDENTERBACKGROUND: return onAppDidEnterBackground(event, prevHandled, provider);
                case SDL_APP_WILLENTERFOREGROUND: return onAppWillEnterForeground(event, prevHandled, provider);
                case SDL_APP_DIDENTERFOREGROUND: return onAppDidEnterForeground(event, prevHandled, provider);
                case SDL_LOCALECHANGED: return onLocaleChanged(event, prevHandled, provider);
                case SDL_DISPLAYEVENT: return onDisplayEvent(event, prevHandled, provider);
                case SDL_WINDOWEVENT: return onWindowEvent(event, prevHandled, provider);
                case SDL_SYSWMEVENT: return onSysWMEvent(event, prevHandled, provider);
                case SDL_KEYDOWN: return onKeyDown(event, prevHandled, provider);
                case SDL_KEYUP: return onKeyUp(event, prevHandled, provider);
                case SDL_TEXTEDITING: return onTextEditing(event, prevHandled, provider);
                case SDL_TEXTINPUT: return onTextInput(event, prevHandled, provider);
                case SDL_KEYMAPCHANGED: return onKeyMapChanged(event, prevHandled, provider);
                case SDL_TEXTEDITING_EXT: return onTextEditingExt(event, prevHandled, provider);
                case SDL_MOUSEMOTION: return onMouseMotion(event, prevHandled, provider);
                case SDL_MOUSEBUTTONDOWN: return onMouseButtonDown(event, prevHandled, provider);
                case SDL_MOUSEBUTTONUP: return onMouseButtonUp(event, prevHandled, provider);
                case SDL_MOUSEWHEEL: return onMouseWheel(event, prevHandled, provider);
                case SDL_JOYAXISMOTION: return onJoyAxisMotion(event, prevHandled, provider);
                case SDL_JOYBALLMOTION: return onJoyBallMotion(event, prevHandled, provider);
                case SDL_JOYHATMOTION: return onJoyHatMotion(event, prevHandled, provider);
                case SDL_JOYBUTTONDOWN: return onJoyButtonDown(event, prevHandled, provider);
                case SDL_JOYBUTTONUP: return onJoyButtonUp(event, prevHandled, provider);
                case SDL_JOYDEVICEADDED: return onJoyDeviceAdded(event, prevHandled, provider);
                case SDL_JOYDEVICEREMOVED: return onJoyDeviceRemoved(event, prevHandled, provider);
                case SDL_JOYBATTERYUPDATED: return onJoyBatteryUpdated(event, prevHandled, provider);
                case SDL_CONTROLLERAXISMOTION: return onControllerAxisMotion(event, prevHandled, provider);
                case SDL_CONTROLLERBUTTONDOWN: return onControllerButtonDown(event, prevHandled, provider);
                case SDL_CONTROLLERBUTTONUP: return onControllerButtonUp(event, prevHandled, provider);
                case SDL_CONTROLLERDEVICEADDED: return onControllerDeviceAdded(event, prevHandled, provider);
                case SDL_CONTROLLERDEVICEREMOVED: return onControllerDeviceRemoved(event, prevHandled, provider);
                case SDL_CONTROLLERDEVICEREMAPPED: return onControllerDeviceRemapped(event, prevHandled, provider);
                case SDL_CONTROLLERTOUCHPADDOWN: return onControllerTouchPadDown(event, prevHandled, provider);
                case SDL_CONTROLLERTOUCHPADMOTION: return onControllerTouchPadMotion(event, prevHandled, provider);
                case SDL_CONTROLLERTOUCHPADUP: return onControllerTouchPadUp(event, prevHandled, provider);
                case SDL_CONTROLLERSENSORUPDATE: return onControllerSensorUpdate(event, prevHandled, provider);
                case SDL_CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3: return
                            onControllerUpdateCompleteReservedForSDL3(event, prevHandled, provider);
                case SDL_CONTROLLERSTEAMHANDLEUPDATED: return onControllerSteamHandleUpdated(
                        event, prevHandled, provider);
                case SDL_FINGERDOWN: return onFingerDown(event, prevHandled, provider);
                case SDL_FINGERUP: return onFingerUp(event, prevHandled, provider);
                case SDL_FINGERMOTION: return onFingerMotion(event, prevHandled, provider);
                case SDL_DOLLARGESTURE: return onDollarGesture(event, prevHandled, provider);
                case SDL_DOLLARRECORD: return onDollarRecord(event, prevHandled, provider);
                case SDL_MULTIGESTURE: return onMultiGesture(event, prevHandled, provider);
                case SDL_CLIPBOARDUPDATE: return onClipboardUpdate(event, prevHandled, provider);
                case SDL_DROPFILE: return onDropFile(event, prevHandled, provider);
                case SDL_DROPTEXT: return onDropText(event, prevHandled, provider);
                case SDL_DROPBEGIN: return onDropBegin(event, prevHandled, provider);
                case SDL_DROPCOMPLETE: return onDropComplete(event, prevHandled, provider);
                case SDL_AUDIODEVICEADDED: return onAudioDeviceAdded(event, prevHandled, provider);
                case SDL_AUDIODEVICEREMOVED: return onAudioDeviceRemoved(event, prevHandled, provider);
                case SDL_SENSORUPDATE: return onSensorUpdate(event, prevHandled, provider);
                case SDL_RENDER_TARGETS_RESET: return onRenderTargetsReset(event, prevHandled, provider);
                case SDL_RENDER_DEVICE_RESET: return onRenderDeviceReset(event, prevHandled, provider);
                case SDL_POLLSENTINEL: return onPollSentinel(event, prevHandled, provider);
                case SDL_USEREVENT: return onUserEvent(event, prevHandled, provider);
                case SDL_LASTEVENT: return onLastEvent(event, prevHandled, provider);
                default: throw runtime_error("Unknown event type");
            }
        }
    };
}

namespace application {
    // shared_ptr<SDL2_EventListener> identityEventListener = make_shared<IdentitySDL2_EventListener>();

    enum class LayerState : int8_t {
        enabled,
        disabled,
    };

    // A Layer is a workable unit that can be enabled or disabled
    // It needs to provide some workable interface
    // 1. before listening to events
    // 2. event listener
    // 3. after listening to events
    // 4. render
    // 5. the state of the layer

    export class WorkableLayer extends public ContextEventListener {
    protected:
        LayerState m_state = LayerState::enabled;

    public:
        WorkableLayer() = default;

        WorkableLayer(const WorkableLayer &) = delete;

        WorkableLayer &operator=(const WorkableLayer &) = delete;

        [[nodiscard]] virtual LayerState state() const {
            return m_state;
        }

        void virtual preEventListening(IVirtualMachineContextProvider&) {}

        void virtual postEventListening(IVirtualMachineContextProvider&) {}

        void virtual render(IVirtualMachineContextProvider&) {}

        operator bool() const { // NOLINT(*-explicit-constructor)
            return m_state == LayerState::enabled;
        }
    };
}