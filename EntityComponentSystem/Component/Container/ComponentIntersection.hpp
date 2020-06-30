#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_IMPL
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_IMPL

#include "ComponentIntersection.h"

namespace cs
{
	template <typename... Components>
	ComponentIntersection<Components...>::ComponentIntersection(cs::ComponentCollection<Components>&... sets) 
		: others(sizeof...(Components) - 1U)
	{
		auto index = std::uint32_t(0);
		auto size = std::max({ sets.size()... }) + std::uint32_t(1);

		// Assigns the set with the lowest amount of elements
		auto probe = [this](auto size, const auto& set) {
			return set.size() < size ? (smallest = &set, set.size()) : size;
		};

		// Filters all those sets bigger than the smallest set
		auto filter = [this](auto index, const auto& set) {
			return (smallest != &set) ? (others[index++] = &set, index) : index;
		};

		// Execute lambdas using braced-init-lists technique
		auto probing = { 0U, (size = probe(size, sets), 0U)... };
		auto filtering = { 0U, (index = filter(index, sets), 0U)... };
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
	const ComponentIntersectionIterator ComponentIntersection<Components...>::begin() const {
		return ComponentIntersectionIterator(others, smallest->begin(), smallest->end());
	}

	template <typename... Components>
	const ComponentIntersectionIterator ComponentIntersection<Components...>::end() const {
		return ComponentIntersectionIterator(others, smallest->end(), smallest->end());
	}
}

#endif