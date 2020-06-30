#ifndef CORE_ENTITY_ENTITYMANAGER_IMPL_H
#define CORE_ENTITY_ENTITYMANAGER_IMPL_H

#include "EntityManager.h"
#include "Entity.h"

namespace cs
{
	template <typename Component, typename... Args>
	Component EntityManager::assign(std::uint32_t id, Args&&... args) {
		validate(id);
		auto component = Component(std::forward<Args>(args)...);
		ensure<Component>().add(id, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::replace(std::uint32_t id, Args&&... args) {
		validate(id);
		auto component = Component(std::forward<Args>(args)...);
		set<Component>().update(id, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::accomodate(std::uint32_t id, Args&&... args) {
		validate(id);
		auto component = Component(std::forward<Args>(args)...);
		ensure<Component>().accomodate(id, component);
		return component;
	}

	template <typename Component, typename... Components>
	void EntityManager::assign(std::uint32_t id, const Component& component, const Components&... components) {
		validate(id);
		ensure<Component>().add(id, component);
		assign(id, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::replace(std::uint32_t id, const Component& component, const Components&... components) {
		validate(id);
		ensure<Component>().update(id, component);
		replace(id, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::accomodate(std::uint32_t id, const Component& component, const Components&... components) {
		validate(id);
		ensure<Component>().accomodate(id, component);
		accomodate(id, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(std::uint32_t id) {
		validate(id);

		if (managed<Component>()) {
			set<Component>().reset(id);
		}

		reset<Components...>(id);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(std::uint32_t id, const Component& unused, const Components&... unuseds) {
		reset<Component, Components...>(id);
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(std::uint32_t id) {
		validate(id);
		set<Component>().remove(id);
		remove<Components...>(id);
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(std::uint32_t id, const Component& unused, const Components&... unuseds) {
		remove<Component, Components...>(id);
	}

	template <typename Component, typename... Components>
	bool EntityManager::has(std::uint32_t id) {
		validate(id);
		return managed<Component>() && set<Component>().contains(id) && has<Components...>(id);
	}

	template <typename Component, typename... Components>
	bool EntityManager::has(std::uint32_t id, const Component& unused, const Components&... unuseds) {
		return has<Component, Components...>(id);
	}

	Entity EntityManager::create() {
		std::uint32_t id;

		if (available) {
			const auto entity = next;
			const auto version = entities[entity] & (~Entity::ID_MASK);

			id = entity | version;
			next = entities[entity] & Entity::ID_MASK;
			entities[entity] = id;
			--available;
		}
		else {
			id = std::uint32_t(entities.size());
			assert(id < Entity::ID_MASK);
			entities.push_back(id);
		}

		return Entity(this, id);
	}

	template <typename Component, typename... Components>
	Entity EntityManager::create(const Component& component, const Components&... components) {
		auto entity = create();
		entity.assign(component, components...);
		return entity;
	}

	template <typename Component, typename... Args>
	Entity EntityManager::create(Args&&... args) {
		auto entity = create();
		entity.assign<Component>(args...);
		return entity;
	}

	template <typename Component>
	Component& EntityManager::component(std::uint32_t id) {
		validate(id);
		return set<Component>().get(id);
	}

	template <typename... Components>
	std::tuple<Components&...> EntityManager::components(std::uint32_t id) {
		return std::tuple<Components&...>(component<Components>(id)...);
	}

	template <typename Component>
	std::uint32_t EntityManager::count() {
		return managed<Component>() ? set<Component>().size() : std::uint32_t();
	}

	std::uint32_t EntityManager::size() const {
		return entities.size() - available;
	}

	std::uint32_t EntityManager::capacity() const {
		return entities.size();
	}

	template <typename Component>
	void EntityManager::reserve(std::uint32_t capacity) {
		ensure<Component>().reserve(capacity);
	}

	void EntityManager::reserve(std::uint32_t capacity) {
		entities.reserve(capacity);
	}

	template <typename Component, typename... Components>
	bool EntityManager::empty() {
		return managed<Component>() ? (set<Component>().empty() ? true : empty<Components...>()) : true;
	}

	bool EntityManager::empty() const {
		return entities.size() == available;
	}

	std::uint32_t EntityManager::version(std::uint32_t id) const {
		return std::uint32_t((id >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	std::uint32_t EntityManager::current(std::uint32_t id) const {
		auto index = id & Entity::ID_MASK;
		assert(index < entities.size());
		return std::uint32_t((entities[index] >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	void EntityManager::destroy(std::uint32_t id) {
		validate(id);

		const auto entity = id & Entity::ID_MASK;
		const auto version = (id & (~Entity::ID_MASK)) + (1U << Entity::VERSION_SHIFT);
		const auto node = (available ? next : ((entity + 1U) & Entity::ID_MASK)) | version;

		entities[entity] = node;
		next = entity;
		++available;

		for (auto&& cet : sets) {
			if (cet && cet->contains(id)) {
				cet->remove(id);
			}
		}
	}

	bool EntityManager::valid(std::uint32_t id) const {
		auto index = id & Entity::ID_MASK;
		return (index < entities.size() && entities[index] == id);
	}

	void EntityManager::validate(std::uint32_t id) const {
		if (!valid(id))
			throw new std::runtime_error("Invalid id");
	}

	template <typename Function>
	void EntityManager::each(Function& function) {
		if (available) {
			for (auto id : entities) {
				const auto clone = id; // Needed?
				const auto masked = clone & Entity::ID_MASK;

				if (id == masked) {
					function(Entity(this, id));
				}
			}
		}
		else {
			for (auto id : entities) {
				function(Entity(this, id));
			}
		}
	}

	template <typename Component, typename... Components, typename Function>
	void EntityManager::each(Function& function) {
		ComponentView<Component, Components...>(this, ensure<Component>(), ensure<Components>()...).each(function);
		//View<Component, Components...>(this, ensure<Component>(), ensure<Components>()...).each(function);
	}

	template <typename Component, typename... Components, typename Function>
	void EntityManager::every(Function& function) {
		//PersistentView<Components...>(this, handler<Components...>(), set<Components>()...).each(function);
	}

	template <typename Component, typename Compare>
	void EntityManager::sort(Compare compare) {
		ensure<Component>().sort(std::move(compare));
	}

	template <typename To, typename From>
	void EntityManager::sort() {
		ensure<To>().respect(ensure<From>());
	}

	template <typename Component, typename... Components>
	void EntityManager::reset() {
		if (managed<Component>()) {
			auto& cet = set<Component>();

			each([&cet](Entity entity) {
				cet.reset(entity.id());
			});
		}

		reset<Components...>();
	}

	void EntityManager::reset() {
		each([](Entity entity) {
			entity.destroy();
		});
	}

	template <typename Component>
	bool EntityManager::managed() const {
		auto uid = ComponentFamily::uid<Component>();
		return uid < sets.size() && sets[uid];
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::set() {
		assert(managed<Component>());
		return static_cast<ComponentCollection<Component>&>(*sets[ComponentFamily::uid<Component>()]);
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::ensure() {
		auto uid = ComponentFamily::uid<Component>();

		if (uid >= sets.size()) {
			sets.resize(uid + 1);
		}

		if (!sets[uid]) {
			sets[uid] = std::make_unique<ComponentCollection<Component>>();
		}

		return set<Component>();
	}

	template <typename... Components>
	Collection& EntityManager::handler() {
		//static_assert(sizeof...(Components) > 1, "!");
		const auto uid = ViewFamily::uid<Components...>();

		if (uid >= handlers.size()) {
			handlers.resize(uid + 1);
		}

		if (!handlers[uid]) {
			auto sparse = std::make_unique<Collection>();

			for (auto id : view<Components...>()) {
				sparse->construct(id);
			}

			auto accumulator = {
				(ensure<Components>().append(sparse.get(), &EntityManager::contains<Components...>), 0)...
			};

			handlers[uid] = std::move(set);
		}

		return *handlers[uid];
	}
}

#endif