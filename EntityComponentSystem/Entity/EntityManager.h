#ifndef CORE_ENTITY_ENTITYMANAGER_H
#define CORE_ENTITY_ENTITYMANAGER_H

#include <memory>
#include <utility>
#include <cassert>
#include <algorithm>
#include <stdexcept>

#include "../Type/Family.h"
#include "../Entity/Entity.h"
#include "../Component/Container/ComponentView.hpp"

namespace cs
{
	class EntityManager final
	{
	public:
		EntityManager() = default;
		EntityManager(const EntityManager&) = delete;
		EntityManager(EntityManager&&) = default;

		EntityManager& operator=(const EntityManager&) = delete;
		EntityManager& operator=(EntityManager&&) = default;

		Entity create();

		template <typename Component, typename... Args>
		Entity create(Args&&... componentArgs);

		template <typename Component, typename... Components>
		Entity create(const Component& component, const Components&... components);

		template <typename Component, typename... Args>
		Component assign(std::uint32_t entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component replace(std::uint32_t entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component save(std::uint32_t entityId, Args&&... componentArgs);

		template <typename Component>
		Component& component(std::uint32_t entityId);

		template <typename Component, typename... Components>
		void assign(std::uint32_t entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void replace(std::uint32_t entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void save(std::uint32_t entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void reset(std::uint32_t entityId);

		template <typename Component, typename... Components>
		void reset(std::uint32_t entityId, const Component& unused, const Components&... unuseds); // Uses type deduction

		template <typename Component, typename... Components>
		void remove(std::uint32_t entityId);

		template <typename Component, typename... Components>
		void remove(std::uint32_t entityId, const Component& unused, const Components&... unuseds); // Uses type deduction

		template <typename Component, typename... Components>
		bool has(std::uint32_t entityId);

		template <typename Component, typename... Components>
		bool has(std::uint32_t entityId, const Component& unused, const Components&... unuseds); // Uses type deduction

		template <typename Component>
		std::uint32_t count();
		std::uint32_t size() const;

		std::uint32_t version(std::uint32_t entityId) const;
		std::uint32_t current(std::uint32_t entityId) const;

		void destroy(std::uint32_t entityId);
		bool valid(std::uint32_t entityId) const;

		template <typename Component, typename... Components>
		void reset();
		void reset();

		template <typename Component, typename... Components, typename Lambda>
		void each(Lambda&& lambda);

		template <typename Lambda>
		void each(Lambda&& lambda);

	protected:
		template <typename Component>
		bool managed() const;

		template <typename Component>
		ComponentCollection<Component>& safelyRetrieveCollection();

		template <typename Component>
		ComponentCollection<Component>& unsafelyRetrieveCollection();

	private:
		void validate(std::uint32_t entityId) const;

		// Fallback blank function for recursion
		void assign(std::uint32_t entityId) {}

		// Fallback blank function for recursion
		void replace(std::uint32_t entityId) {}

		// Fallback blank function for recursion
		void save(std::uint32_t entityId) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void reset() {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void remove(std::uint32_t entityId) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		void reset(std::uint32_t entityId) {}

		// Fallback blank function for recursion
		template <bool expand = true>
		bool has(std::uint32_t entityId) { return true; }

		// Fallback blank function for recursion
		template <bool expand = true>
		bool empty() const { return false; }

	private:
		std::uint32_t next = 0U;
		std::uint32_t available = 0U;
		std::vector<std::uint32_t> entities;
		std::vector<std::unique_ptr<Collection>> collections;
	};
}

#endif