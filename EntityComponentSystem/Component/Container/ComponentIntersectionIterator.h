#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_H
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_H

#include "ComponentCollection.h"

namespace cs
{
	class ComponentIntersectionIterator : public std::iterator<std::input_iterator_tag, std::uint32_t>
	{
	public:
		ComponentIntersectionIterator(std::vector<const cs::Collection*> others, cs::Collection::IteratorConst begin, cs::Collection::IteratorConst end);

		ComponentIntersectionIterator& operator++(); // Prefix (++it)
		ComponentIntersectionIterator  operator++(int); // Suffix (it++)

		bool operator==(const ComponentIntersectionIterator& other) const;
		bool operator!=(const ComponentIntersectionIterator& other) const;

		std::uint32_t operator*() const;

	private:
		inline bool intersects() const;

	private:
		cs::Collection::IteratorConst begin;
		cs::Collection::IteratorConst end;
		const std::vector<const cs::Collection*> others;
	};
}

#endif