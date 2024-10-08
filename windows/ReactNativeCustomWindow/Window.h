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

        REACT_METHOD(SetCursor, L"setCursor")
        void SetCursor(hstring cursorName) noexcept
        {
            context.UIDispatcher().Post([cursorName] {
                winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(
                        winrt::Windows::UI::Core::CoreCursor(
                            GetCursorType(cursorName),
                            0
                        )
                    );
            });
        }

        static CoreCursorType GetCursorType(hstring cursorName) noexcept
        {
            if (cursorName == L"arrow") return CoreCursorType::Arrow;
            if (cursorName == L"cross") return CoreCursorType::Cross;
            if (cursorName == L"hand") return CoreCursorType::Hand;
            if (cursorName == L"help") return CoreCursorType::Help;
            if (cursorName == L"ibeam") return CoreCursorType::IBeam;
            if (cursorName == L"sizeAll") return CoreCursorType::SizeAll;
            if (cursorName == L"sizeNortheastSouthwest") return CoreCursorType::SizeNortheastSouthwest;
            if (cursorName == L"sizeNorthSouth") return CoreCursorType::SizeNorthSouth;
            if (cursorName == L"sizeNorthwestSoutheast") return CoreCursorType::SizeNorthwestSoutheast;
            if (cursorName == L"sizeWestEast") return CoreCursorType::SizeWestEast;
            if (cursorName == L"universalNo") return CoreCursorType::UniversalNo;
            if (cursorName == L"upArrow") return CoreCursorType::UpArrow;
            if (cursorName == L"wait") return CoreCursorType::Wait;
            return CoreCursorType::Arrow;
        }
        
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
            try {
                // Check if args is valid
                if (!args) {
                    throw std::runtime_error("PointerEventArgs is null");
                }

                // Get the current point and check if it is valid
                auto currentPoint = args.CurrentPoint();
                if (!currentPoint) {
                    throw std::runtime_error("CurrentPoint is null");
                }

                // Get the current position of the pointer
                auto pointerPosition = currentPoint.Position();

                // Get the current bounds of the window
                auto windowBounds = Window::Current().Bounds();

                winrt::Microsoft::ReactNative::JSValueObject params;
                params["x"] = pointerPosition.X;
                params["y"] = pointerPosition.Y;
                params["windowWidth"] = windowBounds.Width;
                params["windowHeight"] = windowBounds.Height;

                // Only call the event if it is defined, otherwise it will throw an exception
                if (OnPointerEntered) {
                    OnPointerEntered(std::move(params));
                }
            } catch (const std::exception& ex) {
                // Log the exception message
                OutputDebugStringA(ex.what());
            } catch (...) {
                // Catch any other exceptions
                OutputDebugStringA("An unknown error occurred in OnPointerEnteredHandler");
            }
        }
    };
}
