#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_IMPL
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_IMPL

#include "ComponentIntersectionIterator.h"

namespace cs
{
	ComponentIntersectionIterator::ComponentIntersectionIterator(std::vector<const cs::Collection*> others, cs::Collection::IteratorConst begin, cs::Collection::IteratorConst end)
		: others(others), begin(begin), end(end)
	{
		if (begin != end && !intersects())
			++(*this);
	}

	ComponentIntersectionIterator& ComponentIntersectionIterator::operator++(void) {
		return (++begin != end && !intersects()) ? ++(*this) : (*this);
	}

	ComponentIntersectionIterator ComponentIntersectionIterator::operator++(int) {
		ComponentIntersectionIterator orign = *this;
		return ++(*this), orign;
	}

	bool ComponentIntersectionIterator::operator==(const ComponentIntersectionIterator& other) const {
		return other.begin == begin;
	}

	bool ComponentIntersectionIterator::operator!=(const ComponentIntersectionIterator& other) const {
		return other.begin != begin;
	}

	std::uint32_t ComponentIntersectionIterator::operator*() const {
		return *begin;
	}

	inline bool ComponentIntersectionIterator::intersects() const {
		auto index = others.size();

		// For all the other sets, check whether they have the smallest set's begin iterator value
		for (const auto id = *begin; index && others[index - 1U]->contains(id); --index);

		// Will only return true if index ever reaches 0 (both comparables contain it)
		return index == 0U;
	}
}

#endif