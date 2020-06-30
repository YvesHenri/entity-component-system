#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_H
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_H

#include "ComponentIntersectionIterator.hpp"

namespace cs
{
	template <typename... Components>
	class ComponentIntersection
	{
	public:
		ComponentIntersection(cs::ComponentCollection<Components>&... sets);

		ComponentIntersectionIterator begin();
		ComponentIntersectionIterator end();

		const ComponentIntersectionIterator begin() const;
		const ComponentIntersectionIterator end() const;

	private:
		const cs::Collection* smallest;
		std::vector<const cs::Collection*> others;
	};
}

#endif