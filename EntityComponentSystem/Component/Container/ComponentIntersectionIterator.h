#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_H
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_ITERATOR_H

#include "ComponentCollection.hpp"

namespace cs
{
	class ComponentIntersectionIterator : public std::iterator<std::input_iterator_tag, std::uint32_t>
	{
	public:
		ComponentIntersectionIterator(std::vector<cs::Collection*> others, cs::Collection::IteratorConst begin, cs::Collection::IteratorConst end);

		ComponentIntersectionIterator& operator++(); // ++i
		ComponentIntersectionIterator  operator++(int); // i++

		std::uint32_t operator*() const;

		bool operator==(const ComponentIntersectionIterator& other) const;
		bool operator!=(const ComponentIntersectionIterator& other) const;

	private:
		inline bool intersects() const;

	private:
		cs::Collection::IteratorConst begin;
		cs::Collection::IteratorConst end;
		std::vector<cs::Collection*> collections;
	};
}

#endif