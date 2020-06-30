#pragma once

#include "../Container/ComponentIntersection.hpp"

namespace cs
{
	class EntityManager;

	template <typename... Components>
	struct ComponentView
	{
		ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Components>&... components)
			: manager(manager)
			, components(components...)
			, intersection(components...) 
		{}

		template <typename Function>
		void each(Function& function) {
			for (auto id : intersection) {
				function(id, std::get<ComponentCollection<Components>&>(components).get(id)...);
			}
		}

		const cs::EntityManager* manager;
		const cs::ComponentIntersection<Components...> intersection;
		const std::tuple<ComponentCollection<Components>&...> components;
	};

	template <typename Component>
	struct ComponentView<Component> final
	{
		ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Component>& components)
			: manager(manager)
			, components(components)
		{}

		template <typename Function>
		void each(Function& function) {
			for (auto id : components) {
				function(id, components.get(id));
			}
		}

		cs::EntityManager* manager;
		cs::ComponentCollection<Component>& components;
	};
}