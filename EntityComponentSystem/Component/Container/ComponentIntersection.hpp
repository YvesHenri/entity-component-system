#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_IMPL
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_IMPL

#include "ComponentIntersection.h"

namespace cs
{
	template <typename... Components>
	ComponentIntersection<Components...>::ComponentIntersection(cs::ComponentCollection<Components>&... collections) 
		: others(sizeof...(Components) - 1U)
		, smallest(nullptr)
		, all(collections...)
	{
		auto index = 0U;
		auto size = std::max({ collections.size()... }) + 1U;

		// Assigns the collection with the lowest amount of elements
		auto probe = [this](auto size, auto& collection) {
			return collection.size() < size ? (smallest = &collection, collection.size()) : size;
		};

		// Filters all those collections bigger than the smallest collection
		auto filter = [this](auto index, auto& collection) {
			return (smallest != &collection) ? (others[index++] = &collection, index) : index;
		};

		// Execute lambdas using braced-init-lists technique
		auto probing = { 0U, (size = probe(size, collections), 0U)... };
		auto filtering = { 0U, (index = filter(index, collections), 0U)... };
	}

	template <typename... Components>
	ComponentIntersectionIterator ComponentIntersection<Components...>::begin() {
		return ComponentIntersectionIterator(others, smallest->begin(), smallest->end());
	}

	template <typename... Components>
	ComponentIntersectionIterator ComponentIntersection<Components...>::end() {
		return ComponentIntersectionIterator(others, smallest->end(), smallest->end());
	}

	template <typename... Components>
	template <typename Component>
	ComponentCollection<Component>& ComponentIntersection<Components...>::get() {
		return std::get<ComponentCollection<Component>&>(all);
	}
}

#endif