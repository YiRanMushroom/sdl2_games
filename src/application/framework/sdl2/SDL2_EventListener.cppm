module;

#include <SDL2/SDL.h>

export module SDL2_EventListener;

import std_essentials;

// export class SDL2_EventListener {
// protected:
//     virtual bool onFirstEvent(const SDL_Event &, bool original) { return original; }
//     virtual bool onQuit(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppTerminating(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppLowMemory(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppWillEnterBackground(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppDidEnterBackground(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppWillEnterForeground(const SDL_Event &, bool original) { return original; }
//     virtual bool onAppDidEnterForeground(const SDL_Event &, bool original) { return original; }
//     virtual bool onLocaleChanged(const SDL_Event &, bool original) { return original; }
//     virtual bool onDisplayEvent(const SDL_Event &, bool original) { return original; }
//     virtual bool onWindowEvent(const SDL_Event &, bool original) { return original; }
//     virtual bool onSysWMEvent(const SDL_Event &, bool original) { return original; }
//     virtual bool onKeyDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onKeyUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onTextEditing(const SDL_Event &, bool original) { return original; }
//     virtual bool onTextInput(const SDL_Event &, bool original) { return original; }
//     virtual bool onKeyMapChanged(const SDL_Event &, bool original) { return original; }
//     virtual bool onTextEditingExt(const SDL_Event &, bool original) { return original; }
//     virtual bool onMouseMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onMouseButtonDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onMouseButtonUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onMouseWheel(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyAxisMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyBallMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyHatMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyButtonDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyButtonUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyDeviceAdded(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyDeviceRemoved(const SDL_Event &, bool original) { return original; }
//     virtual bool onJoyBatteryUpdated(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerAxisMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerButtonDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerButtonUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerDeviceAdded(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerDeviceRemoved(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerDeviceRemapped(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerTouchPadDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerTouchPadMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerTouchPadUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerSensorUpdate(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerUpdateCompleteReservedForSDL3(const SDL_Event &, bool original) { return original; }
//     virtual bool onControllerSteamHandleUpdated(const SDL_Event &, bool original) { return original; }
//     virtual bool onFingerDown(const SDL_Event &, bool original) { return original; }
//     virtual bool onFingerUp(const SDL_Event &, bool original) { return original; }
//     virtual bool onFingerMotion(const SDL_Event &, bool original) { return original; }
//     virtual bool onDollarGesture(const SDL_Event &, bool original) { return original; }
//     virtual bool onDollarRecord(const SDL_Event &, bool original) { return original; }
//     virtual bool onMultiGesture(const SDL_Event &, bool original) { return original; }
//     virtual bool onClipboardUpdate(const SDL_Event &, bool original) { return original; }
//     virtual bool onDropFile(const SDL_Event &, bool original) { return original; }
//     virtual bool onDropText(const SDL_Event &, bool original) { return original; }
//     virtual bool onDropBegin(const SDL_Event &, bool original) { return original; }
//     virtual bool onDropComplete(const SDL_Event &, bool original) { return original; }
//     virtual bool onAudioDeviceAdded(const SDL_Event &, bool original) { return original; }
//     virtual bool onAudioDeviceRemoved(const SDL_Event &, bool original) { return original; }
//     virtual bool onSensorUpdate(const SDL_Event &, bool original) { return original; }
//     virtual bool onRenderTargetsReset(const SDL_Event &, bool original) { return original; }
//     virtual bool onRenderDeviceReset(const SDL_Event &, bool original) { return original; }
//     virtual bool onPollSentinel(const SDL_Event &, bool original) { return original; }
//     virtual bool onUserEvent(const SDL_Event &, bool original) { return original; }
//     virtual bool onLastEvent(const SDL_Event &, bool original) { return original; }
//
// public:
//     virtual ~SDL2_EventListener() = default;
//
//     virtual bool handle(const SDL_Event &event, bool prevHandled) {
//         switch (event.type) {
//             case SDL_FIRSTEVENT: return onFirstEvent(event, prevHandled);
//             case SDL_QUIT: return onQuit(event, prevHandled);
//             case SDL_APP_TERMINATING: return onAppTerminating(event, prevHandled);
//             case SDL_APP_LOWMEMORY: return onAppLowMemory(event, prevHandled);
//             case SDL_APP_WILLENTERBACKGROUND: return onAppWillEnterBackground(event, prevHandled);
//             case SDL_APP_DIDENTERBACKGROUND: return onAppDidEnterBackground(event, prevHandled);
//             case SDL_APP_WILLENTERFOREGROUND: return onAppWillEnterForeground(event, prevHandled);
//             case SDL_APP_DIDENTERFOREGROUND: return onAppDidEnterForeground(event, prevHandled);
//             case SDL_LOCALECHANGED: return onLocaleChanged(event, prevHandled);
//             case SDL_DISPLAYEVENT: return onDisplayEvent(event, prevHandled);
//             case SDL_WINDOWEVENT: return onWindowEvent(event, prevHandled);
//             case SDL_SYSWMEVENT: return onSysWMEvent(event, prevHandled);
//             case SDL_KEYDOWN: return onKeyDown(event, prevHandled);
//             case SDL_KEYUP: return onKeyUp(event, prevHandled);
//             case SDL_TEXTEDITING: return onTextEditing(event, prevHandled);
//             case SDL_TEXTINPUT: return onTextInput(event, prevHandled);
//             case SDL_KEYMAPCHANGED: return onKeyMapChanged(event, prevHandled);
//             case SDL_TEXTEDITING_EXT: return onTextEditingExt(event, prevHandled);
//             case SDL_MOUSEMOTION: return onMouseMotion(event, prevHandled);
//             case SDL_MOUSEBUTTONDOWN: return onMouseButtonDown(event, prevHandled);
//             case SDL_MOUSEBUTTONUP: return onMouseButtonUp(event, prevHandled);
//             case SDL_MOUSEWHEEL: return onMouseWheel(event, prevHandled);
//             case SDL_JOYAXISMOTION: return onJoyAxisMotion(event, prevHandled);
//             case SDL_JOYBALLMOTION: return onJoyBallMotion(event, prevHandled);
//             case SDL_JOYHATMOTION: return onJoyHatMotion(event, prevHandled);
//             case SDL_JOYBUTTONDOWN: return onJoyButtonDown(event, prevHandled);
//             case SDL_JOYBUTTONUP: return onJoyButtonUp(event, prevHandled);
//             case SDL_JOYDEVICEADDED: return onJoyDeviceAdded(event, prevHandled);
//             case SDL_JOYDEVICEREMOVED: return onJoyDeviceRemoved(event, prevHandled);
//             case SDL_JOYBATTERYUPDATED: return onJoyBatteryUpdated(event, prevHandled);
//             case SDL_CONTROLLERAXISMOTION: return onControllerAxisMotion(event, prevHandled);
//             case SDL_CONTROLLERBUTTONDOWN: return onControllerButtonDown(event, prevHandled);
//             case SDL_CONTROLLERBUTTONUP: return onControllerButtonUp(event, prevHandled);
//             case SDL_CONTROLLERDEVICEADDED: return onControllerDeviceAdded(event, prevHandled);
//             case SDL_CONTROLLERDEVICEREMOVED: return onControllerDeviceRemoved(event, prevHandled);
//             case SDL_CONTROLLERDEVICEREMAPPED: return onControllerDeviceRemapped(event, prevHandled);
//             case SDL_CONTROLLERTOUCHPADDOWN: return onControllerTouchPadDown(event, prevHandled);
//             case SDL_CONTROLLERTOUCHPADMOTION: return onControllerTouchPadMotion(event, prevHandled);
//             case SDL_CONTROLLERTOUCHPADUP: return onControllerTouchPadUp(event, prevHandled);
//             case SDL_CONTROLLERSENSORUPDATE: return onControllerSensorUpdate(event, prevHandled);
//             case SDL_CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3: return
//                         onControllerUpdateCompleteReservedForSDL3(event, prevHandled);
//             case SDL_CONTROLLERSTEAMHANDLEUPDATED: return onControllerSteamHandleUpdated(event, prevHandled);
//             case SDL_FINGERDOWN: return onFingerDown(event, prevHandled);
//             case SDL_FINGERUP: return onFingerUp(event, prevHandled);
//             case SDL_FINGERMOTION: return onFingerMotion(event, prevHandled);
//             case SDL_DOLLARGESTURE: return onDollarGesture(event, prevHandled);
//             case SDL_DOLLARRECORD: return onDollarRecord(event, prevHandled);
//             case SDL_MULTIGESTURE: return onMultiGesture(event, prevHandled);
//             case SDL_CLIPBOARDUPDATE: return onClipboardUpdate(event, prevHandled);
//             case SDL_DROPFILE: return onDropFile(event, prevHandled);
//             case SDL_DROPTEXT: return onDropText(event, prevHandled);
//             case SDL_DROPBEGIN: return onDropBegin(event, prevHandled);
//             case SDL_DROPCOMPLETE: return onDropComplete(event, prevHandled);
//             case SDL_AUDIODEVICEADDED: return onAudioDeviceAdded(event, prevHandled);
//             case SDL_AUDIODEVICEREMOVED: return onAudioDeviceRemoved(event, prevHandled);
//             case SDL_SENSORUPDATE: return onSensorUpdate(event, prevHandled);
//             case SDL_RENDER_TARGETS_RESET: return onRenderTargetsReset(event, prevHandled);
//             case SDL_RENDER_DEVICE_RESET: return onRenderDeviceReset(event, prevHandled);
//             case SDL_POLLSENTINEL: return onPollSentinel(event, prevHandled);
//             case SDL_USEREVENT: return onUserEvent(event, prevHandled);
//             case SDL_LASTEVENT: return onLastEvent(event, prevHandled);
//             default: throw runtime_error("Unknown event type");
//         }
//     }
// };

// export class IdentitySDL2_EventListener : public SDL2_EventListener {
//     virtual bool handle(const SDL_Event &event, bool prevHandled) override {
//         return prevHandled;
//     }
// };
