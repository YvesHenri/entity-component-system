#ifndef COMPONENT_CONTAINER_COMPONENT_INTERSECTION_H
#define COMPONENT_CONTAINER_COMPONENT_INTERSECTION_H

#include <tuple>

#include "ComponentIntersectionIterator.hpp"

namespace cs
{
	template <typename... Components>
	class ComponentIntersection
	{
	public:
		ComponentIntersection(cs::ComponentCollection<Components>&... collections);

		ComponentIntersectionIterator begin();
		ComponentIntersectionIterator end();

		template <typename Component>
		ComponentCollection<Component>& get();

	private:
		cs::Collection* smallest;
		std::vector<cs::Collection*> others;
		std::tuple<ComponentCollection<Components>&...> all;
	};
}

#endif