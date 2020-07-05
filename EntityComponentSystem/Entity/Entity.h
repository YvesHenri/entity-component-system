#ifndef CORE_ENTITY_ENTITY_H
#define CORE_ENTITY_ENTITY_H

namespace cs
{
	class EntityManager;

	class Entity final
	{
	public:
		Entity() = default;
		Entity(const Entity&) = default;
		explicit Entity(EntityManager* manager, std::uint32_t id);

		Entity& operator=(const Entity&) = default;		
		
		bool valid() const;
		void destroy();

		std::uint32_t id() const;
		std::uint32_t version() const;

		template <typename Component, typename... Args>
		Component assign(Args&&... args) const;

		template <typename Component, typename... Args>
		Component replace(Args&&... args) const;

		template <typename Component, typename... Args>
		Component save(Args&&... args) const;

		template <typename Component, typename... Components>
		void assign(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void replace(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void save(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void reset();

		template <typename Component, typename... Components>
		void reset(const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void remove();

		template <typename Component, typename... Components>
		void remove(const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		bool has() const;

		template <typename Component, typename... Components>
		bool has(const Component& component, const Components&... components) const;

		template <typename Component>
		Component& component() const;

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;
		bool operator< (const Entity& other) const;

		operator bool() const;

		static const int ID_MASK = 0xFFFFFF;
		static const int VERSION_MASK = 0xFF;
		static const int VERSION_SHIFT = 24;

	private:
		std::uint32_t identifier;
		cs::EntityManager* manager;
	};
}

#endif