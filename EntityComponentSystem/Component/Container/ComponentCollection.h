#ifndef COMPONENT_CONTAINER_COMPONENT_SET_H
#define COMPONENT_CONTAINER_COMPONENT_SET_H

#include <vector>

namespace cs
{
	/**
	* @brief Sparse set implementation.
	*/
	class Collection
	{
	public:
		using Iterator = std::vector<std::uint32_t>::iterator;
		using IteratorConst = std::vector<std::uint32_t>::const_iterator;

		Collection() = default;
		Collection(const Collection&) = delete; // No copying
		Collection(Collection&&) = default;
		virtual ~Collection() = default;

		virtual bool empty() const;
		virtual void clear(); // Overriden
		virtual void resize(std::uint32_t capacity);  // Overriden
		virtual bool add(std::uint32_t value);
		virtual bool remove(std::uint32_t value);  // Overriden
		virtual bool contains(std::uint32_t value) const;

		std::uint32_t size() const;
		std::uint32_t* data();

		Iterator begin();
		Iterator end();
		IteratorConst begin() const;
		IteratorConst end() const;

	protected:
		static const int OCCUPIED = 0x01000000;
		std::vector<std::uint32_t> values; // Where the actual values are stored (dense set)
		std::vector<std::uint32_t> indices; // Where the indices to values are stored (sparse set)
	};

	/**
	* @brief Extended sparse set implementation for components.
	*
	* This specialization of a sparse set associates a component to an entity. The
	* main purpose of this class is to use sparse sets to store components.
	* It guarantees fast access both to the components and to the entities.
	*
	* @note
	* Entities and components have the same order. It's guaranteed both in case of raw
	* access (either to entities or components) and when using input iterators.
	*
	* @note
	* Internal data structures arrange elements to maximize performance. Because of
	* that, there are no guarantees that elements have the expected order when
	* iterate directly the internal packed array (see `raw` and `size` member
	* functions for that). Use `begin` and `end` instead.
	*
	* @tparam Component Type of component assigned to the entities.
	*/
	template <typename Component>
	class ComponentCollection final : public Collection
	{
	public:
		ComponentCollection() = default;
		ComponentCollection(const ComponentCollection&) = delete;
		ComponentCollection(ComponentCollection&&) = default;

		void clear() override;
		void resize(std::uint32_t capacity) override;
		bool reset(std::uint32_t value);
		bool remove(std::uint32_t value) override;
		bool add(std::uint32_t value, const Component& component);
		bool update(std::uint32_t value, const Component& component);
		void accomodate(std::uint32_t value, const Component& component);

		Component& get(std::uint32_t value);

	private:
		std::vector<Component> components;
	};
}

#endif
