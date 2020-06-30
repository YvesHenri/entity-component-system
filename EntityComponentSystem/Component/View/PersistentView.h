#pragma once

#include <array>
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>
#include "../../Entity/Entity.h"
#include "../../Component/Container/ComponentCollection.hpp"

namespace cs
{
	class EntityManager;

	/**
	* @brief Persistent set.
	*
	* A persistent set returns all the entities and only the entities that have
	* at least the given components. Moreover, it's guaranteed that the entity list
	* is thightly packed in memory for fast iterations.<br/>
	* In general, persistent views don't stay true to the order of any set of
	* components unless users explicitly sort them.
	*
	* @b Important
	*
	* Iterators aren't invalidated if:
	*
	* * New instances of the given components are created and assigned to entities.
	* * The entity currently pointed is modified (as an example, if one of the
	* given components is removed from the entity to which the iterator points).
	*
	* In all the other cases, modify the sets of the given components somehow
	* invalidates all the iterators and using them results in undefined behavior.
	*
	* @note
	* Views share references to the underlying data structures with the Registry
	* that generated them. Therefore any change to the entities and to the
	* components made by means of the registry are immediately reflected by
	* views.<br/>
	* Moreover, sorting a persistent set affects all the other views of the same
	* type (it means that users don't have to call `sort` on each set to sort all
	* of them because they share the set of entities).
	*
	* @warning
	* Lifetime of a set must overcome the one of the registry that generated it.
	* In any other case, attempting to use a set results in undefined behavior.
	*
	* @sa View
	* @sa View<EntityIdentifierType, Components>
	*
	* @tparam EntityIdentifierType A valid entity type (see EntityTraits for more details).
	* @tparam Components Types of components iterated by the set.
	*/
	template <typename... Components>
	class PersistentView final
	{
	public:
		/**
		* @brief Constructs a persistent set around a dedicated pool of entities.
		*
		* A persistent set is created out of:
		*
		* * A dedicated pool of entities that is shared between all the persistent
		* views of the same type.
		* * A bunch of sets of components to which to refer to get instances.
		*
		* @param set Shared reference to a dedicated pool of entities.
		* @param sets References to sets of components.
		*/
		PersistentView(EntityManager* manager, Collection& set, ComponentCollection<Components>&... sets)
			: set(set), sets(sets...), manager(manager)
		{}

		/**
		* @brief Iterate the entities and applies them the given function object.
		*
		* The function object is invoked for each entity. It is provided with the
		* entity itself and a set of const references to all the components of the
		* set.<br/>
		* The signature of the function should be equivalent to the following:
		*
		* @code{.cpp}
		* void(std::uint32_t, const Components&...);
		* @endcode
		*
		* @tparam Function Type of the function object to invoke.
		* @param function A valid function object.
		*/
		template <typename Function>
		void each(Function function) const {
			for (auto id : set) {
				function(Entity(manager, id), std::get<ComponentCollection<Components>&>(sets).get(id)...);
			}
		}

		/**
		* @brief Iterate the entities and applies them the given function object.
		*
		* The function object is invoked for each entity. It is provided with the
		* entity itself and a set of references to all the components of the set.<br/>
		* The signature of the function should be equivalent to the following:
		*
		* @code{.cpp}
		* void(std::uint32_t, Components&...);
		* @endcode
		*
		* @tparam Function Type of the function object to invoke.
		* @param function A valid function object.
		*/
		template <typename Function>
		void each(Function function) {
			for (auto id : set) {
				function(Entity(manager, id), std::get<ComponentCollection<Components>&>(sets).get(id)...);
			}
		}

		/**
		* @brief Sort the shared pool of entities according to the given component.
		*
		* Persistent views of the same type share with the Registry a pool of
		* entities with its own order that doesn't depend on the order of any pool
		* of components. Users can order the underlying data structure so that it
		* respects the order of the pool of the given component.
		*
		* @note
		* The shared pool of entities and thus its order is affected by the changes
		* to each and every pool that it tracks. Therefore changes to those sets
		* can quickly ruin the order imposed to the pool of entities shared between
		* the persistent views.
		*
		* @tparam Component Type of the component to use to impose the order.
		*/
		template <typename Component>
		void sort() {
			//set.respect(std::get<ComponentCollection<Component>&>(sets));
		}

	private:
		Collection& set;
		EntityManager* manager;
		std::tuple<ComponentCollection<Components>&...> sets;
	};
}