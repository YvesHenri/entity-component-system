#ifndef COMPONENT_CONTAINER_COMPONENT_VIEW_IMPL
#define COMPONENT_CONTAINER_COMPONENT_VIEW_IMPL

#include "ComponentView.h"

namespace cs
{
	template <typename... Components>
	ComponentView<Components...>::ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Components>&... components)
		: manager(manager)
		, intersection(components...)
	{}


	template <typename... Components>
	void ComponentView<Components...>::each(std::function<void(cs::Entity&, Components&...)>& callback)
	{
		for (auto entityId : intersection) {
			auto entity = cs::Entity(manager, entityId);
			callback(entity, intersection.get<Components>().get(entityId)...);
		}
	}

	template <typename Component>
	ComponentView<Component>::ComponentView(cs::EntityManager* manager, cs::ComponentCollection<Component>& components)
		: manager(manager)
		, components(components)
	{}

	template <typename Component>
	void ComponentView<Component>::each(std::function<void(cs::Entity&, Component&)>& callback) {
		for (auto entityId : components) {
			auto entity = cs::Entity(manager, entityId);
			callback(entity, components.get(entityId));
		}
	}
}

#endif