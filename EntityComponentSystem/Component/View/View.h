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
	* @brief Multi component view.
	*
	* Multi component views iterate over those entities that have at least all the
	* given components in their bags. During initialization, a multi component view
	* looks at the number of entities available for each component and picks up a
	* reference to the smallest set of candidate entities in order to get a
	* performance boost when iterate.<br/>
	* Order of elements during iterations are highly dependent on the order of the
	* underlying data strctures. See ComponentCollection and its specializations for more details.
	*
	* @b Important
	*
	* Iterators aren't invalidated if:
	*
	* * New instances of the given components are created and assigned to entities.
	* * The entity currently pointed is modified (as an example, if one of the
	* given components is removed from the entity to which the iterator points).
	*
	* In all the other cases, modify the components of the given components somehow
	* invalidates all the iterators and using them results in undefined behavior.
	*
	* @note
	* Views share references to the underlying data structures with the Registry
	* that generated them. Therefore any change to the entities and to the
	* components made by means of the registry are immediately reflected by views.
	*
	* @warning
	* Lifetime of a view must overcome the one of the registry that generated it.
	* In any other case, attempting to use a view results in undefined behavior.
	*
	* @tparam Components Types of components iterated by the view.
	*/
	template <typename... Components>
	class View final
	{
	public:
		/**
		* @brief Constructs a view out of a bunch of components of components.
		* @param components References to components of components.
		*/
		View(EntityManager* manager, ComponentCollection<Components>&... components)
			: components(components...)
			, manager(manager)
			, smallest(nullptr)
			, comparables()
		{
			reset();
		}

		template <typename Function>
		void each(Function& function) {
			for (auto id : *smallest) {
				if (intersects(id)) {
					function(Entity(manager, id), std::get<ComponentCollection<Components>&>(components).get(id)...);
				}
			}
		}

		/**
		* @brief Resets the view and reinitializes it.
		*
		* A multi component view keeps a reference to the smallest set of candidate
		* entities to iterate. Resetting a view means querying the underlying data
		* structures and reinitializing the view.<br/>
		* Use it only if copies of views are stored around and there is a
		* possibility that a component has become the best candidate in the
		* meantime.
		*/
		void reset() {
			auto index = std::uint32_t(0);
			auto size = std::max({ std::get<ComponentCollection<Components>&>(components).size()... }) + std::uint32_t(1);

			// Assigns the set with the lowest amount of elements
			auto probe = [this](auto size, const auto& set) {
				return set.size() < size ? (smallest = &set, set.size()) : size;
			};

			// Filters all those sets bigger than the smallest set
			auto filter = [this](auto index, const auto& set) {
				return (smallest != &set) ? (comparables[index++] = &set, index) : index;
			};

			// Execute lambdas using braced-init-lists
			auto probing = { 0U, (size = probe(size, std::get<ComponentCollection<Components>&>(components)), 0U)... };
			auto filtering = { 0U, (index = filter(index, std::get<ComponentCollection<Components>&>(components)), 0U)... };
		}

	private:
		inline bool intersects(std::uint32_t id) {
			auto index = comparables.size();
			for (; index && comparables[index - 1U]->contains(id); --index);
			return index == 0U;
		}

	private:
		const Collection* smallest;
		EntityManager* manager; // New
		std::tuple<ComponentCollection<Components>&...> components;
		std::array<const Collection*, (sizeof...(Components) - 1U)> comparables;
	};

	/**
	* @brief Single component view specialization.
	*
	* Single component views are specialized in order to get a boost in terms of
	* performance. This kind of views can access the underlying data structure
	* directly and avoid superflous checks.<br/>
	* Order of elements during iterations are highly dependent on the order of the
	* underlying data structure. See ComponentCollection and its specializations for more details.
	*
	* @b Important
	*
	* Iterators aren't invalidated if:
	*
	* * New instances of the given components are created and assigned to entities.
	* * The entity currently pointed is modified (as an example, if one of the
	* given components is removed from the entity to which the iterator points).
	*
	* In all the other cases, modify the components of the given components somehow
	* invalidates all the iterators and using them results in undefined behavior.
	*
	* @note
	* Views share a reference to the underlying data structure with the Registry
	* that generated them. Therefore any change to the entities and to the
	* components made by means of the registry are immediately reflected by views.
	*
	* @warning
	* Lifetime of a view must overcome the one of the registry that generated it.
	* In any other case, attempting to use a view results in undefined behavior.
	*
	* @tparam Component Type of the component iterated by the view.
	*/
	template <typename Component>
	class View<Component> final
	{
	public:
		/**
		* @brief Constructs a view out of a set of components.
		* @param set A reference to a set of components.
		*/
		View(EntityManager* manager, ComponentCollection<Component>& set) 
			: components(set)
			, manager(manager)
		{}

		/**
		* @brief Iterate the entities and applies them the given function object.
		*
		* The function object is invoked for each entity. It is provided with the
		* entity itself and a const reference to the component of the view.<br/>
		* The signature of the function should be equivalent to the following:
		*
		* @code{.cpp}
		* void(cs::Entity, const Component&);
		* @endcode
		*
		* @tparam Function Type of the function object to invoke.
		* @param function A intersects function object.
		*/
		template <typename Function>
		void each(Function& function) const {
			for (auto id : components) {
				function(Entity(manager, id), components.get(id));
			}
		}

		/**
		* @brief Iterate the entities and applies them the given function object.
		*
		* The function object is invoked for each entity. It is provided with the
		* entity itself and a reference to the component of the view.<br/>
		* The signature of the function should be equivalent to the following:
		*
		* @code{.cpp}
		* void(cs::Entity, Component&);
		* @endcode
		*
		* @tparam Function Type of the function object to invoke.
		* @param function A intersects function object.
		*/
		template <typename Function>
		void each(Function& function) {
			for (auto id : components) {
				function(Entity(manager, id), components.get(id));
			}
		}

	private:
		EntityManager* manager;
		ComponentCollection<Component>& components;
	};
}