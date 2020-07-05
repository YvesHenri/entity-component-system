#ifndef CORE_ENTITY_ENTITYMANAGER_IMPL_H
#define CORE_ENTITY_ENTITYMANAGER_IMPL_H

#include "EntityManager.h"
#include "Entity.h"

namespace cs
{
	template <typename Component, typename... Args>
	Component EntityManager::assign(std::uint32_t entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safelyRetrieveCollection<Component>().add(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::replace(std::uint32_t entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		unsafelyRetrieveCollection<Component>().replace(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::save(std::uint32_t entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safelyRetrieveCollection<Component>().save(entityId, component);
		return component;
	}

	template <typename Component, typename... Components>
	void EntityManager::assign(std::uint32_t entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safelyRetrieveCollection<Component>().add(entityId, component);
		assign(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::replace(std::uint32_t entityId, const Component& component, const Components&... components) {
		validate(entityId);
		unsafelyRetrieveCollection<Component>().replace(entityId, component);
		replace(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::save(std::uint32_t entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safelyRetrieveCollection<Component>().save(entityId, component);
		save(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(std::uint32_t entityId) {
		validate(entityId);

		if (managed<Component>()) {
			unsafelyRetrieveCollection<Component>().reset(entityId);
		}

		reset<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(std::uint32_t entityId, const Component& unused, const Components&... unuseds) {
		reset<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(std::uint32_t entityId) {
		validate(entityId);
		unsafelyRetrieveCollection<Component>().remove(entityId);
		remove<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(std::uint32_t entityId, const Component& unused, const Components&... unuseds) {
		remove<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	bool EntityManager::has(std::uint32_t entityId) {
		validate(entityId);
		return managed<Component>() && unsafelyRetrieveCollection<Component>().contains(entityId) && has<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	bool EntityManager::has(std::uint32_t entityId, const Component& unused, const Components&... unuseds) {
		return has<Component, Components...>(entityId);
	}

	Entity EntityManager::create() {
		std::uint32_t id;

		if (available) {
			const auto entity = next;
			const auto version = entities[entity] & ~Entity::ID_MASK;

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
	Entity EntityManager::create(Args&&... componentArgs) {
		auto entity = create();
		entity.assign<Component>(componentArgs...);
		return entity;
	}

	template <typename Component>
	Component& EntityManager::component(std::uint32_t entityId) {
		validate(entityId);
		return unsafelyRetrieveCollection<Component>().get(entityId);
	}

	template <typename Component>
	std::uint32_t EntityManager::count() {
		return managed<Component>() ? unsafelyRetrieveCollection<Component>().size() : 0U;
	}

	std::uint32_t EntityManager::size() const {
		return entities.size() - available;
	}

	std::uint32_t EntityManager::version(std::uint32_t entityId) const {
		return std::uint32_t((entityId >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	std::uint32_t EntityManager::current(std::uint32_t entityId) const {
		auto index = entityId & Entity::ID_MASK;
		assert(index < entities.size());
		return std::uint32_t((entities[index] >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	void EntityManager::destroy(std::uint32_t entityId) {
		validate(entityId);

		const auto entity = entityId & Entity::ID_MASK;
		const auto version = (entityId & (~Entity::ID_MASK)) + (1U << Entity::VERSION_SHIFT);
		const auto node = (available ? next : ((entity + 1U) & Entity::ID_MASK)) | version;

		entities[entity] = node;
		next = entity;
		++available;

		for (auto& collection : collections) {
			if (collection && collection->contains(entityId)) {
				collection->remove(entityId);
			}
		}
	}

	bool EntityManager::valid(std::uint32_t entityId) const {
		auto index = entityId & Entity::ID_MASK;
		return index < entities.size() && entities[index] == entityId;
	}

	void EntityManager::validate(std::uint32_t entityId) const {
		if (!valid(entityId))
			throw new std::runtime_error("Invalid entity identifier");
	}

	template <typename Lambda>
	void EntityManager::each(Lambda&& lambda) {
		auto function = std::function<void(Entity&)>(std::move(lambda));

		if (available) {
			for (auto entityId : entities) {
				const auto clone = entityId; // Needed?
				const auto masked = clone & Entity::ID_MASK;

				if (entityId == masked) {
					auto entity = cs::Entity(this, entityId);
					function(entity);
				}
			}
		}
		else {
			for (auto entityId : entities) {
				auto entity = cs::Entity(this, entityId);
				function(entity);
			}
		}
	}

	template <typename Component, typename... Components, typename Lambda>
	void EntityManager::each(Lambda&& lambda) {
		auto function = std::function<void(cs::Entity&, Component&, Components&...)>(std::move(lambda));
		ComponentView<Component, Components...>(this, safelyRetrieveCollection<Component>(), safelyRetrieveCollection<Components>()...).each(function);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset() {
		if (managed<Component>()) {
			auto& collection = unsafelyRetrieveCollection<Component>();

			each([&collection](Entity& entity) {
				collection.reset(entity.id());
			});
		}

		reset<Components...>();
	}

	void EntityManager::reset() {
		each([](Entity& entity) {
			entity.destroy();
		});
	}

	template <typename Component>
	bool EntityManager::managed() const {
		auto uid = ComponentFamily::uid<Component>();
		return uid < collections.size() && collections[uid];
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::unsafelyRetrieveCollection() {
		auto& collection = collections[ComponentFamily::uid<Component>()];
		return static_cast<ComponentCollection<Component>&>(*collection);
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::safelyRetrieveCollection() {
		auto uid = ComponentFamily::uid<Component>();

		if (uid >= collections.size()) {
			collections.resize(uid + 1U);
		}

		if (!collections[uid]) {
			collections[uid] = std::make_unique<ComponentCollection<Component>>();
		}

		return unsafelyRetrieveCollection<Component>();
	}
}

#endif