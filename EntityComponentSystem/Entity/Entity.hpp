#ifndef CORE_ENTITY_ENTITY_IMPL_H
#define CORE_ENTITY_ENTITY_IMPL_H

#include "Entity.h"
#include "EntityManager.h"

namespace cs
{
	Entity::Entity(EntityManager* manager, std::uint32_t id)
		: manager(manager)
		, identifier(id)
	{}

	/**
	* @brief Assigns the given component to this entity.
	*
	* A new instance of the given component is created and initialized with the
	* arguments provided (the component must have a proper constructor or be of
	* aggregate type). Then the component is assigned to the given entity.
	*
	* @warning
	* Attempting to use an invalid entity or to assign a component to an entity
	* that already owns it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity already owns an instance of the given component.
	*
	* @tparam Component Type of component to create.
	* @tparam Args Types of arguments to use to construct the component.
	* @param args Parameters to use to initialize the component.
	* @return A reference to the newly created component.
	*/
	template <typename Component, typename... Args>
	Component Entity::assign(Args&&... args) {
		return manager->assign<Component>(identifier, std::forward<Args>(args)...);
	}

	/**
	* @brief Assigns the given components to this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to assign a component to an entity
	* that already owns it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity already owns an instance of the given component.
	*/
	template <typename Component, typename... Components>
	void Entity::assign(const Component& component, const Components&... components) {
		manager->assign(identifier, component, components...);
	}

	/**
	* @brief Replaces the given component.
	*
	* A new instance of the given component is created and initialized with the
	* arguments provided (the component must have a proper constructor or be of
	* aggregate type). Then the component is assigned to this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to replace a component of an
	* entity that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*
	* @tparam Component Type of component to replace.
	* @tparam Args Types of arguments to use to construct the component.
	* @param args Parameters to use to initialize the component.
	* @return A reference to the newly created component.
	*/
	template <typename Component, typename... Args>
	Component Entity::replace(Args&&... args) {
		return manager->replace<Component>(identifier, std::forward<Args>(args)...);
	}

	/**
	* @brief Replaces the given components.
	*
	* @warning
	* Attempting to use an invalid entity or to replace a component of an
	* entity that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*/
	template <typename Component, typename... Components>
	void Entity::replace(const Component& component, const Components&... components) {
		manager->replace(identifier, component, components...);
	}

	/**
	* @brief Assigns or replaces the given components for this entity.
	* Prefer this function anyway because it has slighlty better performance.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*
	* @tparam Component Type of component to assign or replace.
	* @tparam Args Types of arguments to use to construct the component.
	* @param args Parameters to use to initialize the component.
	* @return A reference to the newly created component.
	*/
	template <typename Component, typename... Args>
	Component Entity::accomodate(Args&&... args) {
		return manager->accomodate<Component>(identifier, std::forward<Args>(args)...);
	}

	/**
	* @brief Assigns or replaces the given components for this entity.
	* Prefer this function anyway because it has slighlty better performance.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*/
	template <typename Component, typename... Components>
	void Entity::accomodate(const Component& component, const Components&... components) {
		manager->accomodate(identifier, component, components...);
	}

	/**
	* @brief Checks if this entity has all the given components.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*
	* @tparam Component Components for which to perform the check.
	* @return True if this entity has all the components, false otherwise.
	*/
	template <typename Component, typename... Components>
	bool Entity::has() const {
		return manager->has<Component, Components...>(identifier);
	}

	/**
	* @brief Checks if this entity has all the given components.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*/
	template <typename Component, typename... Components>
	bool Entity::has(const Component& component, const Components&... components) const {
		return manager->has(identifier, component, components...);
	}
	
	/**
	* @brief Returns the entity identifier.
	*/
	const std::uint32_t Entity::id() const {
		return identifier;
	}

	/**
	* @brief Returns the version of this entity.
	*
	* @warning
	* Attempting to use an entity that doesn't belong to the manager results
	* in undefined behavior. An entity belongs to the manager even if it has
	* been previously destroyed and/or recycled.<br/>
	* An assertion will abort the execution at runtime in debug mode if the
	* manager doesn't own the given entity.
	*
	* @return Actual version for this entity.
	*/
	const std::uint32_t Entity::version() const {
		return manager->current(identifier);
	}

	/**
	* @brief Verifies if this entity still refers to a valid state.
	*
	* In case entities are stored around, this function can be used to know if they
	* are still valid or the entity has been destroyed and potentially recycled.
	*
	* @return True if the identifier is still valid, false otherwise.
	*/
	bool Entity::valid() const {
		return manager->valid(identifier);
	}

	/**
	* @brief Destroys this entity and lets the manager recycle its identifier.
	*
	* When an entity is destroyed, its version is updated and the identifier
	* can be recycled at any time. In case entity identifers are stored around,
	* the `current` member function can be used to know if they are still valid
	* or the entity has been destroyed and potentially recycled.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*/
	void Entity::destroy() {
		manager->destroy(identifier);
	}

	/**
	* @brief Returns a reference to the given component for this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to get a component from an entity
	* that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*
	* @tparam Component Type of component to get.
	* @return A reference to the component owned by the entity.
	*/
	template <typename Component>
	Component& Entity::component() {
		return const_cast<Component&>(const_cast<const Entity*>(this)->component<Component>());
	}

	/**
	* @brief Returns a reference to the given component for this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to get a component from an entity
	* that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*
	* @tparam Component Type of component to get.
	* @return A reference to the component owned by the entity.
	*/
	template <typename Component>
	const Component& Entity::component() const {
		return manager->component<Component>(identifier);
	}

	/**
	* @brief Returns a reference to the given components for this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to get components from an entity
	* that doesn't own them results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own instances of the given
	* components.
	*
	* @tparam Component Type of components to get.
	* @return References to the components owned by the entity.
	*/
	template <typename... Components>
	std::tuple<Components&...> Entity::components() {
		return manager->components<Components...>(identifier);
	}

	/**
	* @brief Returns a reference to the given components for this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to get components from an entity
	* that doesn't own them results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own instances of the given
	* components.
	*
	* @tparam Component Type of components to get.
	* @return References to the components owned by the entity.
	*/
	template <typename... Components>
	std::tuple<const Components&...> Entity::components() const {
		return manager->components<Components...>(identifier);
	}

	/**
	* @brief Resets the given components.
	*
	* If the entity has instances of the components, this function
	* removes them from this entity. Otherwise it does nothing.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*
	* @tparam Component Type of component to reset.
	*/
	template <typename Component, typename... Components>
	void Entity::reset() {
		manager->reset<Component, Components...>(identifier);
	}

	/**
	* @brief Resets the given components from this entity by deducting their types.
	*
	* If the entity has instances of the components, this function
	* removes them from this entity. Otherwise it does nothing.
	*
	* @warning
	* Attempting to use an invalid entity results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity.
	*
	* @tparam Component Type of component to reset.
	*/
	template <typename Component, typename... Components>
	void Entity::reset(const Component& component, const Components&... components) {
		manager->reset(identifier, component, components...);
	}

	/**
	* @brief Removes the given components from this entity.
	*
	* @warning
	* Attempting to use an invalid entity or to remove a component from an
	* entity that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*
	* @tparam Component Type of component to remove.
	*/
	template <typename Component, typename... Components>
	void Entity::remove() {
		manager->remove<Component, Components...>(identifier);
	}

	/**
	* @brief Removes the given components from this entity by deducting their types.
	*
	* @warning
	* Attempting to use an invalid entity or to remove a component from an
	* entity that doesn't own it results in undefined behavior.<br/>
	* An assertion will abort the execution at runtime in debug mode in case of
	* invalid entity or if the entity doesn't own an instance of the given component.
	*
	* @tparam Component Type of component to remove.
	*/
	template <typename Component, typename... Components>
	void Entity::remove(const Component& component, const Components&... components) {
		manager->remove(identifier, component, components...);
	}

	/**
	* @brief Verifies if this entitys identifier still refers to a valid value.
	* @return True if the identifier is still valid, false otherwise.
	*/
	Entity::operator bool() const {
		return valid();
	}

	bool Entity::operator==(const Entity& other) const {
		return identifier == other.identifier;
	}

	bool Entity::operator!=(const Entity& other) const {
		return identifier != other.identifier;
	}

	bool Entity::operator<(const Entity& other) const {
		return identifier < other.identifier;
	}
}

#endif