#pragma once

namespace e2d
{
	template <typename T>
	concept Releasable = requires(T * object)
	{
		object->Release();
	};

	template <Releasable T>
	__forceinline void safeRelease(T* object)
	{
		if (object != nullptr)
			object->Release();
	}
}