#include "Engine2D/Engine2D.h"

#include <cmath>

namespace e2d
{
    Engine2D::Engine2D()
    {
        oldButtonInput_.fill(false);
        newButtonInput_.fill(false);
    }

    Engine2D::~Engine2D()
    {
        StopEngineThread_();

        DiscardDeviceResources_();
    }

    bool Engine2D::Initialize(int32_t width, int32_t height, bool vSync)
    {
        width_ = width;
        height_ = height;
        bVSync_ = vSync;

        if (!CreateWindow_())
            return false;

        return true;
    }

    void Engine2D::Run()
    {
        OnInitialize();

        EngineLoop_();

        OnDestroy();
    }

    void Engine2D::OnInitialize()
    {
    }

    void Engine2D::OnDestroy()
    {
    }

    void Engine2D::OnTick(float deltaTime)
    {
    }

    void Engine2D::OnDraw(ID2D1HwndRenderTarget* renderTarget)
    {
    }

    void Engine2D::OnButtonPressed(Button button)
    {
    }

    void Engine2D::OnButtonHeld(Button button)
    {
    }

    void Engine2D::OnButtonReleased(Button button)
    {
    }

    void Engine2D::OnMouseScroll(int32_t delta)
    {
    }

    void Engine2D::SetWindowTitle(std::wstring_view newTitle) const
    {
        SetWindowText(hwnd_, newTitle.data());
    }

    bool Engine2D::CreateWindow_()
    {
        // Register the window class.
        const WNDCLASSEX wndClassEx = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = Engine2D::WndProc_,
            .cbClsExtra = 0,
            .cbWndExtra = sizeof(LONG_PTR),
            .hInstance = nullptr,
            .hCursor = LoadCursor(nullptr, IDI_APPLICATION),
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = TEXT("Engine2D")
        };

        RegisterClassEx(&wndClassEx);

        // In terms of using the correct DPI, to create a window at a specific size
        // like this, the procedure is to first create the window hidden. Then we get
        // the actual DPI from the HWND (which will be assigned by whichever monitor
        // the window is created on). Then we use SetWindowPos to resize it to the
        // correct DPI-scaled size, then we use ShowWindow to show it.
        hwnd_ = CreateWindow(
            TEXT("Engine2D"),
            TEXT("Engine2D"),
            WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            width_,
            height_,
            nullptr,
            nullptr,
            nullptr,
            this
        );

        if (hwnd_ == nullptr)
            return false;

        RECT rcClient, rcWind;
        GetClientRect(GetHWND(), &rcClient);
        GetWindowRect(GetHWND(), &rcWind);
        const int32_t clientWidth = width_ + (rcWind.right - rcWind.left) - rcClient.right;
        const int32_t clientHeight = height_ + (rcWind.bottom - rcWind.top) - rcClient.bottom;

        // Because the SetWindowPos function takes its size in pixels, we
        // obtain the window's DPI, and use it to scale the window size.
        const auto dpi = static_cast<float>(GetDpiForWindow(hwnd_));

        const auto cX = static_cast<int32_t>(std::ceil(static_cast<float>(clientWidth) * dpi / 96.0f));
        const auto cY = static_cast<int32_t>(std::ceil(static_cast<float>(clientHeight) * dpi / 96.0f));
        SetWindowPos(
            hwnd_,
            nullptr,
            0,
            0,
            cX,
            cY,
            SWP_NOMOVE
        );

        if (!InitializeDeviceResources_())
            return false;

        ShowWindow(hwnd_, SW_SHOWNORMAL);
        UpdateWindow(hwnd_);

        return true;
    }

    void Engine2D::EngineLoop_()
    {
        while (!bShouldStop_)
        {
            //Calculate delta time
            UpdateDeltaTime_();

            //Win32 events
            UpdateWin32Events_();

            //Input
            UpdateInput_();

            //Tick
            OnTick(deltaTime_);

            //Drawing
            UpdateDrawing_();
        }
    }

    void Engine2D::StopEngineThread_()
    {
        bShouldStop_ = true;
    }

    bool Engine2D::InitializeDeviceResources_()
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory_);
        if (factory_ == nullptr)
            return false;

        RECT clientRect;
        GetClientRect(hwnd_, &clientRect);

        const D2D1_SIZE_U size = {
            static_cast<UINT32>(clientRect.right - clientRect.left),
            static_cast<UINT32>(clientRect.bottom - clientRect.top)
        };

        // Create a Direct2D render target.
        factory_->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hwnd_,
                size,
                bVSync_ ? D2D1_PRESENT_OPTIONS_NONE : D2D1_PRESENT_OPTIONS_IMMEDIATELY
            ),
            &renderTarget_);

        if (renderTarget_ == nullptr)
            return false;

        renderTarget_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        renderTarget_->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        //Create a DWriteFactory
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&writeFactory_));

        if (writeFactory_ == nullptr)
            return false;

        return true;
    }

    void Engine2D::DiscardDeviceResources_() const
    {
        safeRelease(factory_);
        safeRelease(renderTarget_);
    }

    LRESULT Engine2D::WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CREATE)
        {
            const auto pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            auto* engine = static_cast<Engine2D*>(pcs->lpCreateParams);
            SetWindowLongPtr(
                hwnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(engine)
            );

            return 0;
        }

        const auto engine = reinterpret_cast<Engine2D*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (engine == nullptr)
            return DefWindowProc(hwnd, message, wParam, lParam);

        switch (message)
        {
        case WM_CLOSE:
        {
            engine->StopEngineThread_();
            PostMessage(hwnd, WM_DESTROY, 0, 0);

            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            DestroyWindow(hwnd);

            return 0;
        }
        case WM_DISPLAYCHANGE: { InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            engine->UpdateButtonState_(static_cast<Button>(wParam), true);
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            engine->UpdateButtonState_(static_cast<Button>(wParam), false);
            return 0;
        }
        case WM_LBUTTONDOWN: { engine->UpdateButtonState_(Button::MOUSE_LEFT, true); return 0; }
        case WM_LBUTTONUP: { engine->UpdateButtonState_(Button::MOUSE_LEFT, false); return 0; }
        case WM_MBUTTONDOWN: { engine->UpdateButtonState_(Button::MOUSE_MIDDLE, true); return 0; }
        case WM_MBUTTONUP: { engine->UpdateButtonState_(Button::MOUSE_MIDDLE, false); return 0; }
        case WM_RBUTTONDOWN: { engine->UpdateButtonState_(Button::MOUSE_RIGHT, true); return 0; }
        case WM_RBUTTONUP: { engine->UpdateButtonState_(Button::MOUSE_RIGHT, false); return 0; }
        case WM_MOUSEWHEEL: { engine->OnMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam)); return 0; }
        }

        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    void Engine2D::UpdateWin32Events_() const
    {
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Engine2D::UpdateDeltaTime_()
    {
        timePoint2_ = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = timePoint2_ - timePoint1_;
        timePoint1_ = timePoint2_;

        deltaTime_ = duration.count();
    }

    void Engine2D::UpdateDrawing_()
    {
        constexpr D2D1_COLOR_F clearColor =
        {
            .r = 0.0f,
            .g = 0.0f,
            .b = 0.0f,
            .a = 1.0f
        };

        renderTarget_->BeginDraw();
        renderTarget_->SetTransform(D2D1::Matrix3x2F::Identity());
        renderTarget_->Clear(clearColor);

        OnDraw(renderTarget_);

        renderTarget_->EndDraw();
    }

    void Engine2D::UpdateButtonState_(Button button, bool bState)
    {
        newButtonInput_[static_cast<size_t>(button)] = bState;
    }

    void Engine2D::UpdateInput_()
    {
        constexpr auto buttonsCount = static_cast<size_t>(Button::INPUT_MAX);

        for (size_t i = 0; i < buttonsCount; i++)
        {
            auto& buttonState = buttonStates_[i];
            buttonState.Reset();

            const auto oldInput = oldButtonInput_[i];
            const auto newInput = newButtonInput_[i];

            const auto button = static_cast<Button>(i);

            if (newInput)
            {
                if (oldInput)
                {
                    buttonState.bHeld = true;
                    OnButtonHeld(button);
                }
                else
                {
                    buttonState.bPressed = true;
                    OnButtonPressed(button);
                }
            }
            else if (oldInput)
            {
                buttonState.bReleased = true;
                OnButtonReleased(button);
            }

            oldButtonInput_[i] = newInput;
        }
    }

    Vector2D Engine2D::GetMousePosition() const
    {
        POINT mousePosition;
        GetCursorPos(&mousePosition);
        ScreenToClient(hwnd_, &mousePosition);

        return { static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y) };
    }
}
