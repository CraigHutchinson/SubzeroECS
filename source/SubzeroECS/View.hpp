#pragma once

#include <tuple>
#include "Collection.hpp"

template <typename T, typename... Ts> struct get_type_index;

template <typename T, typename... Ts>
struct get_type_index<T, T, Ts...> : std::integral_constant<std::size_t, 0U> {};

template <typename T, typename Tail, typename... Ts>
struct get_type_index<T, Tail, Ts...> :
	std::integral_constant<std::size_t, 1U + get_type_index<T, Ts...>::value> {};
 
template <typename T>
struct get_type_index<T>
{
	// condition is always false, but should be dependant of T
	static_assert(sizeof(T) == 0U, "type not found");
};
template<typename T>
using Bare = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

namespace SubzeroECS
{
	/** Creates a union view for SubzeroECSties with the selected components 
	 * @tparam Components  The components that will be iterated over to findSubzeroECSties containign all
						   Each type can  define required access pattern using standard C++ language as follows:
						   * Type - Required component with no acces (TBD: Could be same as Type&)
						   * Type& - Required component for read & write by reference
						   * Type* - Optional component for read & write could be nullptr
						   * const Type& - Required component for read-only
						   * const Type* - Optional component for read-only could be nullptr

	*/
	template< typename... Components >
	class View
	{
	public: 
		static constexpr uint_fast32_t Size
              = sizeof...(Components);  ///< number of components

		typedef std::tuple< Collection<Components>&... > Collections; ///< All component collections

#if 0
		typedef std::tuple< typename Collection<Components>::Iterator... > CollectionIterators; ///< All omcponent iterators
#else
		/// @temp Detect all iterators of same type and use std::array automatically
		typedef std::array< typename Collection<void>::Iterator, Size > CollectionIterators;
#endif

		class Iterator
		{
		public:
			Iterator( Collections& collections, CollectionIterators&& iterators )
			   : collections_(collections)
				, iterators_( std::move(iterators) )
			{
			}

			template< typename Component>
			Component& get()
			{
				static const uint32_t iComponent = get_type_index<Component,Components...>::value; 
				return std::get<iComponent>(collections_).at( 
					std::get<iComponent>(iterators_) );
			}

			Iterator& operator++()
			{
				auto& interA = std::get<0U>(iterators_);
				++interA;
				//auto& interB = std::tuple_element<1U>(iterators_);

				return *this;
			}

			bool operator != ( const Iterator& rhs ) const
			{ 
				return iterators_ != rhs.iterators_; 
			}

			bool operator == ( const Iterator& rhs ) const
			{ 
				return iterators_ == rhs.iterators_; 
			}

			operator EntityId() const
			{ 
				auto iSubzeroECSty = std::get<0U>(iterators_);
				auto iEnd = std::get<0U>(collections_).end();
				return (iSubzeroECSty != iEnd) ? iSubzeroECSty->first 
										 : cInvalid_EntityId; 
			}

			Iterator& operator*()
			{
				return *this;
			}

		private:
			Collections collections_; ////< Collections from the view
			CollectionIterators iterators_;
		};

	public:
		/** @note C++11 std::make_tuple
		*/
		View( CollectionRegistry& registry )
			: collections_( (sizeof( Components ), registry.get<Components>() )... )
		{
		}

		/** @note Uses C++14 std::get<>
		*/
		template< typename Component>
		Collection<Component>& getCollection()
		{
			static constexpr uint_fast32_t iComponent = get_type_index<Component,Components...>::value;
			return std::get<iComponent>(collections_);
		}

		/** TODO */
		Iterator begin() 
		{ return Iterator( collections_, CollectionIterators( getCollection<Components>().begin()...) ); }
		
		/** TODO */
		Iterator end() 
		{ return Iterator( collections_, CollectionIterators( getCollection<Components>().end()... )  ); }
	private:
		Collections collections_;
	};

} //END: SubzeroECS