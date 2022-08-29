#pragma once

// Windows Header Files:
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d2d1.h>
#include <dwrite.h>

#include "common.h"
#include "Input.h"
#include "Vector2D.h"

#include <cstdint>
#include <chrono>
#include <array>

namespace e2d
{
	class Engine2D
	{
	public:
		Engine2D();
		virtual ~Engine2D();

		bool Initialize(int32_t width, int32_t height, bool vSync = true);

		void Run();

		virtual void OnInitialize();
		virtual void OnDestroy();

		virtual void OnTick(float deltaTime);

		virtual void OnDraw(ID2D1HwndRenderTarget* renderTarget);

		virtual void OnButtonPressed(Button button);
		virtual void OnButtonHeld(Button button);
		virtual void OnButtonReleased(Button button);

		virtual void OnMouseScroll(int32_t delta);

		void SetWindowTitle(std::wstring_view newTitle) const;

	private:
		bool CreateWindow_();

		void EngineLoop_();
		void StopEngineThread_();

		bool InitializeDeviceResources_();
		void DiscardDeviceResources_() const;

		static LRESULT CALLBACK WndProc_(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam
		);

		void UpdateWin32Events_() const;

		void UpdateDeltaTime_();
		void UpdateDrawing_();

		void UpdateButtonState_(Button button, bool bState);
		void UpdateInput_();

	private:
		//Size
		int32_t width_{ 0 };
		int32_t height_{ 0 };

		bool bVSync_{true};

		//Engine loop
		bool bShouldStop_{ false };

		//Delta time
		std::chrono::time_point<std::chrono::high_resolution_clock> timePoint1_{ std::chrono::high_resolution_clock::now() };
		std::chrono::time_point<std::chrono::high_resolution_clock> timePoint2_;

		float deltaTime_{ 0.0f };

		//Drawing stuff
		HWND hwnd_{ nullptr };
		ID2D1Factory* factory_{ nullptr };
		ID2D1HwndRenderTarget* renderTarget_{ nullptr };
		IDWriteFactory* writeFactory_{ nullptr };

		//Input
		std::array<bool, static_cast<size_t>(Button::INPUT_MAX)> oldButtonInput_;
		std::array<bool, static_cast<size_t>(Button::INPUT_MAX)> newButtonInput_;
		std::array<ButtonState, static_cast<size_t>(Button::INPUT_MAX)> buttonStates_;

	public:
		__forceinline int32_t GetWidth() const { return width_; }
		__forceinline int32_t GetHeight() const { return height_; }

		__forceinline HWND GetHWND() const { return hwnd_; }
		__forceinline ID2D1Factory* GetD2D1Factory() const { return factory_; }
		__forceinline ID2D1HwndRenderTarget* GetD2D1RenderTarget() const { return renderTarget_; }
		__forceinline IDWriteFactory* GetDWriteFactory() const { return writeFactory_; }

		__forceinline ButtonState GetButtonState(const Button button) const { return buttonStates_[static_cast<size_t>(button)]; }

		Vector2D GetMousePosition() const;
	};
}
