#ifndef CORE_ENTITY_ENTITYMANAGER_H
#define CORE_ENTITY_ENTITYMANAGER_H

#include <tuple>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include "../Type/Family.h"
#include "../Entity/Entity.h"
#include "../Component/Container/ComponentCollection.h"
#include "../Component/View/View.h"
#include "../Component/View/PersistentView.h"
#include "../Component/View/ComponentView.h"

namespace cs
{
	class EntityManager
	{
	public:
		EntityManager() = default;
		EntityManager(const EntityManager&) = delete;
		EntityManager(EntityManager&&) = default;

		EntityManager& operator=(const EntityManager&) = delete;
		EntityManager& operator=(EntityManager&&) = default;

		Entity create();

		template <typename Component, typename... Args>
		Entity create(Args&&... args);

		template <typename Component, typename... Components>
		Entity create(const Component& component, const Components&... components);

		template <typename Component, typename... Args>
		Component assign(std::uint32_t id, Args&&... args);

		template <typename Component, typename... Args>
		Component replace(std::uint32_t id, Args&&... args);

		template <typename Component, typename... Args>
		Component accomodate(std::uint32_t id, Args&&... args);

		template <typename Component, typename... Components>
		void assign(std::uint32_t id, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void replace(std::uint32_t id, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void accomodate(std::uint32_t id, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void reset(std::uint32_t id);

		template <typename Component, typename... Components>
		void reset(std::uint32_t id, const Component& unused, const Components&... unuseds);

		template <typename Component, typename... Components>
		void remove(std::uint32_t id);

		template <typename Component, typename... Components>
		void remove(std::uint32_t id, const Component& unused, const Components&... unuseds);

		template <typename Component, typename... Components>
		bool has(std::uint32_t id);

		template <typename Component, typename... Components>
		bool has(std::uint32_t id, const Component& unused, const Components&... unuseds);

		template <typename Component>
		Component& component(std::uint32_t id);

		template <typename... Components>
		std::tuple<Components&...> components(std::uint32_t id);

		template <typename Component>
		std::uint32_t count();
		std::uint32_t size() const;
		std::uint32_t capacity() const;

		template <typename Component>
		void reserve(std::uint32_t capacity);
		void reserve(std::uint32_t capacity);

		template <typename Component, typename... Components>
		bool empty();
		bool empty() const;
		
		std::uint32_t version(std::uint32_t id) const;
		std::uint32_t current(std::uint32_t id) const;

		void destroy(std::uint32_t id);
		bool valid(std::uint32_t id) const;
		void validate(std::uint32_t id) const;

		template <typename Component, typename Compare>
		void sort(Compare compare);

		template <typename To, typename From>
		void sort();

		template <typename Component, typename... Components>
		void reset();
		void reset();

		template <typename Function>
		void each(Function& function);

		template <typename Component, typename... Components, typename Function>
		void each(Function& function);

		template <typename Component, typename... Components, typename Function>
		void every(Function& function);

	protected:
		template <typename Component>
		bool managed() const;
		
		template <typename... Components>
		Collection& handler();

		template <typename Component>
		ComponentCollection<Component>& set();

		template <typename Component>
		ComponentCollection<Component>& ensure();

	private:
		#pragma region Fallbacks
		
		// Fallback blank function for recursion
		void assign(std::uint32_t id) {}

		// Fallback blank function for recursion
		void replace(std::uint32_t id) {}

		// Fallback blank function for recursion
		void accomodate(std::uint32_t id) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void reset() {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void remove(std::uint32_t id) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void reset(std::uint32_t id) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		bool has(std::uint32_t id) { return true; }

		// Fallback blank function for recursion
		template <bool expand = true>
		bool empty() const { return false; }

		#pragma endregion

	private:
		std::uint32_t next = 0U;
		std::uint32_t available = 0U;
		std::vector<std::uint32_t> entities;
		std::vector<std::unique_ptr<Collection>> sets;
		std::vector<std::unique_ptr<Collection>> handlers;
	};
}

#endif