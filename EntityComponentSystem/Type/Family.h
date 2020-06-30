#pragma once

#include <type_traits>
#include <cstddef>

namespace cs
{
	/**
	* @brief Dynamic identifier generator.
	*
	* Utility class template that can be used to assign unique identifiers to types
	* at runtime. Use different specializations to create separate sets of identifiers.
	*/
	template <typename...>
	class Family final
	{
	public:
		/**
		* @brief Returns an unique identifier for the given type.
		* @return Statically generated unique identifier for the given type.
		*/
		template <typename... T>
		static std::uint32_t uid() noexcept {
			return generate<std::decay_t<T>...>(); // Remove L/R values
		}

	private:
		static std::uint32_t id() noexcept {
			static std::uint32_t value = 0U;
			return value++;
		}

		template <typename...>
		static std::uint32_t generate() noexcept {
			static std::uint32_t value = id();
			return value;
		}
	};

	using ViewFamily = Family<struct ViewFamilyStruct>;
	using ComponentFamily = Family<struct ComponentFamilyStruct>;
}