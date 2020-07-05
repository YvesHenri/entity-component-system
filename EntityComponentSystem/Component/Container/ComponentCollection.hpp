#ifndef COMPONENT_CONTAINER_COMPONENT_SET_IMPL
#define COMPONENT_CONTAINER_COMPONENT_SET_IMPL

#include "ComponentCollection.h"

namespace cs
{
	void Collection::clear() {
		values.clear();
		indices.clear();
	}

	void Collection::resize(std::uint32_t capacity) {
		values.resize(capacity);
		indices.resize(capacity);
	}

	bool Collection::empty() const {
		return values.empty();
	}

	bool Collection::add(std::uint32_t value) {
		auto exists = contains(value);

		// Duplicates can not be stored, otherwise it'll mess the index array
		if (!exists) {
			if (value >= indices.size()) {
				indices.resize(value + 1U);
			}
			indices[value] = values.size() | OCCUPIED;
			values.push_back(value);
		}

		return !exists;
	}

	bool Collection::remove(std::uint32_t value) {
		auto exists = contains(value);

		if (exists) {
			const auto last = values.back();
			const auto index = indices[value] & ~OCCUPIED;

			indices[last] = index | OCCUPIED;
			indices[value] = 0U;

			values[index] = last;
			values.pop_back();
		}

		return exists;
	}

	bool Collection::contains(std::uint32_t value) const {
		return value < indices.size() && (indices[value] & OCCUPIED) != 0U;
	}

	std::uint32_t Collection::size() const {
		return values.size();
	}

	std::uint32_t* Collection::data() {
		return values.data();
	}

	cs::Collection::Iterator Collection::begin() {
		return values.begin();
	}

	cs::Collection::Iterator Collection::end() {
		return values.end();
	}

	cs::Collection::IteratorConst Collection::begin() const {
		return values.cbegin();
	}

	cs::Collection::IteratorConst Collection::end() const {
		return values.cend();
	}

	template <typename Component>
	void ComponentCollection<Component>::clear() {
		components.clear();
		Collection::clear();
	}

	template <typename Component>
	void ComponentCollection<Component>::resize(std::uint32_t capacity) {
		components.resize(capacity);
		Collection::resize(capacity);
	}

	template <typename Component>
	bool ComponentCollection<Component>::reset(std::uint32_t value) {
		return contains(value) ? remove(value) : false;
	}

	template <typename Component>
	bool ComponentCollection<Component>::remove(std::uint32_t value) {
		auto removed = Collection::remove(value);

		if (removed) {
			components[indices[value] & ~OCCUPIED] = std::move(components.back());
			components.pop_back();
		}

		return removed;
	}

	template <typename Component>
	bool ComponentCollection<Component>::add(std::uint32_t value, const Component& component) {
		return Collection::add(value) ? components.emplace_back(component), true : false;
	}

	template <typename Component>
	bool ComponentCollection<Component>::replace(std::uint32_t value, const Component& component) {
		return contains(value) ? components[indices[value] & ~OCCUPIED] = component, true : false; // Execute and return
	}

	template <typename Component>
	void ComponentCollection<Component>::save(std::uint32_t value, const Component& component) {
		contains(value) ? replace(value, component) : add(value, component);
	}

	template <typename Component>
	Component& ComponentCollection<Component>::get(std::uint32_t value) {
		return components[indices[value] & ~OCCUPIED];
	}
}

#endif