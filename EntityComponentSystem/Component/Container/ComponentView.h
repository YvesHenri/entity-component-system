#ifndef COMPONENT_CONTAINER_COMPONENT_VIEW_H
#define COMPONENT_CONTAINER_COMPONENT_VIEW_H

#include <functional>

#include "../Container/ComponentIntersection.hpp"

namespace cs
{
	class EntityManager;

	template <typename... Components>
	class ComponentView
	{
	public:
		ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Components>&... components);

		void each(std::function<void(cs::Entity&, Components&...)>& callback);

	private:
		cs::EntityManager* manager;
		cs::ComponentIntersection<Components...> intersection;
	};

	template <typename Component>
	class ComponentView<Component> final
	{
	public:
		ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Component>& components);

		void each(std::function<void(cs::Entity&, Component&)>& callback);

	private:
		cs::EntityManager* manager;
		cs::ComponentCollection<Component>& components;
	};
}

#endif