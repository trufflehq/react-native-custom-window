#pragma once

#include "pch.h"
#include "NativeModules.h"

#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.UI.h"
#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.Xaml.Navigation.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::ApplicationModel::Core;


namespace RN = winrt::Microsoft::ReactNative;

namespace WindowRN {
    REACT_MODULE(WindowRNModule, L"window");
    struct WindowRNModule final {

        RN::ReactContext context;
        
        bool m_isPointerExitedListenerActive = false;
        bool m_isPointerEnteredListenerActive = false;

        REACT_INIT(Initialize, L"init");
        void Initialize(const RN::ReactContext& reactContext) noexcept
        {
            context = reactContext;
        }

        REACT_METHOD(Fullscreen, L"fullscreen")
        void Fullscreen(RN::ReactPromise<void> promise) noexcept
        {
            try {
                auto jsDispatcher = context.JSDispatcher();

                context.UIDispatcher().Post([promise, jsDispatcher] {
                    ApplicationView view = ApplicationView::GetForCurrentView();
                    bool isFullScreen = view.IsFullScreenMode();
                    if (isFullScreen)
                    {
                        view.ExitFullScreenMode();
                        jsDispatcher.Post([promise] { promise.Resolve(); });
                    }
                    else
                    {
                        view.TryEnterFullScreenMode();
                        jsDispatcher.Post([promise] { promise.Resolve(); });
                    }
                });
            }
            catch (const hresult_error& ex) {
                promise.Reject(RN::ReactError{ winrt::to_string(ex.message()).c_str() });
            }
        }

        REACT_METHOD(IsFullScreen, L"isFullScreen")
        void IsFullScreen(RN::ReactPromise<bool> promise) noexcept
        {
            try {
                auto jsDispatcher = context.JSDispatcher();
                context.UIDispatcher().Post([jsDispatcher,promise] {
                    ApplicationView view = ApplicationView::GetForCurrentView();
                    bool isFullScreen = view.IsFullScreenMode();
                    jsDispatcher.Post([promise, isFullScreen] { promise.Resolve(isFullScreen); });
                });
            }
            catch (const hresult_error& ex) {
                promise.Reject(RN::ReactError{ winrt::to_string(ex.message()).c_str() });
            }
        }

        REACT_METHOD(EnterFullscreen, L"enterFullscreen")
        void EnterFullscreen(RN::ReactPromise<void> promise) noexcept
        {
            try {
                auto jsDispatcher = context.JSDispatcher();

                context.UIDispatcher().Post([jsDispatcher, promise] {
                    ApplicationView view = ApplicationView::GetForCurrentView();
                    view.TryEnterFullScreenMode();
                    jsDispatcher.Post([promise] { promise.Resolve(); });
                });
            }
            catch (const hresult_error& ex) {
                promise.Reject(RN::ReactError{ winrt::to_string(ex.message()).c_str() });
            }
        }

        REACT_METHOD(LeaveFullscreen, L"leaveFullscreen")
        void LeaveFullscreen(RN::ReactPromise<void> promise) noexcept
        {
            try {
                auto jsDispatcher = context.JSDispatcher();
                context.UIDispatcher().Post([jsDispatcher, promise] {
                    ApplicationView view = ApplicationView::GetForCurrentView();
                    view.ExitFullScreenMode();
                    jsDispatcher.Post([promise] { promise.Resolve(); });
                });
            }
            catch (const hresult_error& ex) {
                promise.Reject(RN::ReactError{ winrt::to_string(ex.message()).c_str() });
            }
        }

        REACT_METHOD(SetSize, L"setSize")
        void SetSize(JSValueObject size, RN::ReactPromise<void> promise) noexcept
        {
            try {
                auto jsDispatcher = context.JSDispatcher();

                float width = size["width"].AsSingle();
                float height = size["height"].AsSingle();
                context.UIDispatcher().Post([jsDispatcher, width, height, promise] {

                    ApplicationView view = ApplicationView::GetForCurrentView();
                    view.TryResizeView({ width,height });
                    jsDispatcher.Post([promise] { promise.Resolve(); });
                });
            }
            catch (const hresult_error& ex) {
                promise.Reject(RN::ReactError{ winrt::to_string(ex.message()).c_str() });
            }
        };
        
        REACT_EVENT(OnPointerExited, L"onPointerExited")
        std::function<void(winrt::Microsoft::ReactNative::JSValueObject)> OnPointerExited;
        
        // onPointerExited returns dispose function
        REACT_METHOD(ListenPointerExited, L"listenPointerExited")
        void ListenPointerExited() noexcept
        {          
            context.UIDispatcher().Post([this] {
                auto coreWindow = CoreWindow::GetForCurrentThread();
                if (coreWindow != nullptr)
                {
                    if (!m_isPointerExitedListenerActive)
                    {
                        coreWindow.PointerExited({ this, &WindowRNModule::OnPointerExitedHandler });
                        m_isPointerExitedListenerActive = true;
                    }
                }
            });
        }
        
        void OnPointerExitedHandler(CoreWindow const& sender, PointerEventArgs const& args) {
            // get the current position of the pointer
            auto pointerPosition = args.CurrentPoint().Position();
            // get the current bounds of the window
            auto windowBounds = Window::Current().Bounds();
                    
            winrt::Microsoft::ReactNative::JSValueObject params;
            params["x"] = pointerPosition.X;
            params["y"] = pointerPosition.Y;
            params["windowWidth"] = windowBounds.Width;
            params["windowHeight"] = windowBounds.Height;
            
            OnPointerExited(std::move(params));
        }
        
        REACT_EVENT(OnPointerEntered, L"onPointerEntered")
        std::function<void(winrt::Microsoft::ReactNative::JSValueObject)> OnPointerEntered;
        
        // onPointerEntered returns dispose function
        REACT_METHOD(ListenPointerEntered, L"listenPointerEntered")
        void ListenPointerEntered() noexcept
        {          
            context.UIDispatcher().Post([this] {
                auto coreWindow = CoreWindow::GetForCurrentThread();
                if (coreWindow != nullptr)
                {
                    if (!m_isPointerEnteredListenerActive)
                    {
                        coreWindow.PointerEntered({ this, &WindowRNModule::OnPointerEnteredHandler });
                        m_isPointerEnteredListenerActive = true;
                    }
                }
            });
        }
        
        void OnPointerEnteredHandler(CoreWindow const& sender, PointerEventArgs const& args) {
            // get the current position of the pointer
            auto pointerPosition = args.CurrentPoint().Position();
            // get the current bounds of the window
            auto windowBounds = Window::Current().Bounds();
                    
            winrt::Microsoft::ReactNative::JSValueObject params;
            params["x"] = pointerPosition.X;
            params["y"] = pointerPosition.Y;
            params["windowWidth"] = windowBounds.Width;
            params["windowHeight"] = windowBounds.Height;
            
            OnPointerEntered(std::move(params));
        }
    };
}
