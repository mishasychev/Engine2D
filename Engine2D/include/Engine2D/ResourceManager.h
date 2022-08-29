#pragma once

#include "common.h"

#include <type_traits>
#include <string_view>
#include <unordered_map>
#include <ranges>
#include <d2d1.h>

namespace e2d
{
	template<typename T>
	concept ResourceT = std::is_base_of_v<ID2D1Resource, T>;

	class ResourceManager
	{
		ResourceManager() = default;

	public:
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		~ResourceManager();

		static ResourceManager& Instance();

		void Store(std::string_view name, ID2D1Resource* resource);

		template<ResourceT T>
		T* Get(std::string_view name) const;

	private:
		std::unordered_map<std::string_view, ID2D1Resource*> data_;
	};

	__forceinline ResourceManager::~ResourceManager()
	{
		for (auto* ptr : data_ | std::views::values)
			safeRelease(ptr);
	}

	template <ResourceT T>
	__forceinline T* ResourceManager::Get(std::string_view name) const
	{
		const auto it = data_.find(name);
		if (it == data_.end())
			return nullptr;

		return static_cast<T*>(it->second);
	}

	__forceinline ResourceManager& ResourceManager::Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	__forceinline void ResourceManager::Store(std::string_view name, ID2D1Resource* resource)
	{
		data_.insert(std::make_pair(name, resource));
	}
}
