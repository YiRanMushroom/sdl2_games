module;

#include <AppMacros.h>

export module IWindowSizeChangedListener;

// namespace application {
//     export interface IWindowSizeChangedListener;
//
//     export interface IWindowSizeChangedNotifier;
//
//     export interface IWindowSizeChangedNotifier {
//         virtual void addListener(IWindowSizeChangedListener *listener) = 0;
//
//         virtual void removeListener(IWindowSizeChangedListener *listener) = 0;
//
//         IWindowSizeChangedNotifier() = default;
//
//         virtual ~IWindowSizeChangedNotifier() = default;
//     };
//
//     export interface IWindowSizeChangedListener {
//         virtual void onWindowSizeChanged(int width, int height) = 0;
//
//         IWindowSizeChangedNotifier* notifier = nullptr;
//
//         IWindowSizeChangedListener(const IWindowSizeChangedListener &) = delete;
//
//         IWindowSizeChangedListener &operator=(const IWindowSizeChangedListener &) = delete;
//
//         IWindowSizeChangedListener(IWindowSizeChangedNotifier &notifier) : notifier(&notifier) {
//             notifier.addListener(this);
//         }
//
//         virtual ~IWindowSizeChangedListener() {
//             if (notifier) {
//                 notifier->removeListener(this);
//             }
//         }
//     };
// }
