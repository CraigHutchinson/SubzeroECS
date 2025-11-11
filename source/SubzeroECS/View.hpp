#pragma once

#include <algorithm> //< std::all_of, std::distance
#include <array>
#include <tuple>

#include "Collection.hpp"
#include "Intersection.hpp"

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

//TODO: Decide on optimization strategies
#define SUBZEROECS_VIEW_OPTIM2 1

namespace SubzeroECS
{
	/** Creates a union view for ECS-entities with the selected components 
	 * @tparam Components  The components that will be iterated over to find ECS-entities containing all
						   Each type can  define required access pattern using standard C++ language as follows:
						   * Type - Required component with no access (TBD: Could be same as Type&)
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

		using Collections = std::tuple< Collection<Components>&... >; ///< All component collections
		using Iterators = std::tuple< typename Collection<Components>::Iterator... >; ///< All component iterators
		/// @temp Detect all Iterators of same type and use std::array automatically?

		using ViewIterationState = std::tuple<std::pair< Collection<Components>&, typename Collection<Components>::Iterator >...>;

		/** Get the current iteration state */
		template<std::size_t... Is>
		static ViewIterationState makeIterationStateImpl( Collections& collections, Iterators& iterators, std::index_sequence<Is...> )
		{
			return std::make_tuple(
				std::make_pair(
					std::ref(std::get<Is>(collections)),
					std::get<Is>(iterators))... );
		}

		static ViewIterationState makeIterationState( Collections& collections, Iterators& iterators )
		{
			return makeIterationStateImpl(collections, iterators, std::index_sequence_for<Components...>{});
		}

		/** Iterator for the view performing set-intersection over all component collections */
		class Iterator
		{
		public:
			Iterator( Collections& collections, Iterators&& iterators )
			   : collections_( collections)
				, iterators_( std::move(iterators) )
			{
				// Find first valid intersection
				if constexpr (sizeof...(Components) == 1)
				{
					// Single component - already at first element or end
				}
#if SUBZEROECS_VIEW_OPTIM2
				else if constexpr (sizeof...(Components) == 2)
				{
					begin2();
				}
#endif
				else
				{
					beginN( std::make_index_sequence<sizeof...(Components)>{} );
				}
			}

			template< typename Component>
			Component& get()
			{
				static const uint32_t iComponent = get_type_index<Component, Components...>::value;
				return std::get<iComponent>(collections_).at( 
					std::get<iComponent>(iterators_) );
			}

			template< typename Component>
			bool has()
			{
				static const uint32_t iComponent = get_type_index<Component, Components...>::value;
				auto it = std::get<iComponent>(iterators_);
				auto iend = std::get<iComponent>(collections_).end();
				return it != iend && *it == this->operator EntityId();
			}

			Iterator& operator++()
			{
				// Incrementing at end is an error
				assert(std::get<0>(iterators_) != std::get<0>(collections_).end());

				if constexpr (sizeof...(Components) == 1)
				{
					// Single component - just advance the iterator
					++std::get<0>(iterators_);
				}
#if SUBZEROECS_VIEW_OPTIM2
				else if constexpr (sizeof...(Components) == 2)
				{
					increment2();// Optimized 2-way intersection
				}
#endif
				else
				{
					incrementN( std::make_index_sequence<sizeof...(Components)>{} );
				}
				return *this;
			}

			bool operator != ( const Iterator& rhs ) const
			{ 
				//TODO: We could want a deeper test for consistency in debug?
				return std::get<0>(iterators_) != std::get<0>(rhs.iterators_); 
			}

			bool operator == ( const Iterator& rhs ) const
			{ 
				//TODO: We could want a deeper test for consistency in debug?
				return std::get<0>(iterators_) == std::get<0>(rhs.iterators_); 
			}

			operator EntityId() const
			{ 
				auto iEntity = std::get<0U>(iterators_);
				assert(iEntity != std::get<0U>(collections_).end());
				return *iEntity;
			}

			Iterator& operator*()
			{
				return *this;
			}

		private:

			template<size_t index>
			static EntityId getId( auto& its, auto& iends )
			{
				return (std::get<index>(its) != std::get<index>(iends))
					? *std::get<index>(its)
					: EntityId::Invalid;
			}

			/** Optimized helper for 2-way intersection - find first intersection */
			Iterator& begin2()
			{				
				auto& it1 = std::get<0>(iterators_);
				auto& it2 = std::get<1>(iterators_);
				auto end1 = std::get<0>(collections_).end();
				auto end2 = std::get<1>(collections_).end();

				if (!Intersection::begin2(it1, it2, end1, end2))
				{
					// No intersection found - set first iterator to end
					it1 = end1;
				}
				return *this;
			}

			/** Optimized helper for 2-way intersection - increment and find next */
			Iterator& increment2()
			{				
				auto& it1 = std::get<0>(iterators_);
				auto& it2 = std::get<1>(iterators_);
				auto end1 = std::get<0>(collections_).end();
				auto end2 = std::get<1>(collections_).end();

				if (!Intersection::increment2(it1, it2, end1, end2))
				{
					// No intersection found - set first iterator to end
					it1 = end1;
				}
				return *this;
			}

			/** Helper for N-way intersection - find first intersection */
			template<std::size_t... Is>
			Iterator& beginN( std::index_sequence<Is...> indices )
			{
				auto endIterators = std::make_tuple(std::get<Is>(collections_).end()...);
				if (!Intersection::beginN(indices, iterators_, endIterators))
				{
					// No intersection found - set first iterator to end
					std::get<0>(iterators_) = std::get<0>(endIterators);
				}
				return *this;
			}

			/** Helper for N-way intersection - increment and find next */
			template<std::size_t... Is>
			Iterator& incrementN( std::index_sequence<Is...> indices )
			{
				auto endIterators = std::make_tuple(std::get<Is>(collections_).end()...);
				if (!Intersection::incrementN(indices, iterators_, endIterators))
				{
					// No intersection found - set first iterator to end
					std::get<0>(iterators_) = std::get<0>(endIterators);
				}
				return *this;
			}

		private:
			Collections collections_;
			Iterators iterators_;
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
		{ 
			return Iterator( collections_, Iterators( getCollection<Components>().begin()...) );
		}
		
		/** TODO */
		Iterator end() 
		{ return Iterator( collections_, Iterators( getCollection<Components>().end()... )  ); }
	private:
		Collections collections_;
	};

	/** Specialization of View for zero components - represents an empty view
	 * @remarks This provides a consistent interface for template metaprogramming
	 *          where the component count might be zero.
	 */
	template<>
	class View<>
	{
	public:
		static constexpr uint_fast32_t Size = 0U;

		/** Empty iterator that is always at end */
		class Iterator
		{
		public:
			Iterator() = default;

			Iterator& operator++() { return *this; }
			Iterator& operator*() { return *this; }
			
			bool operator==(const Iterator&) const { return true; }
			bool operator!=(const Iterator&) const { return false; }
			
			operator EntityId() const { return EntityId::Invalid; }
		};

		explicit View(CollectionRegistry&) {}

		Iterator begin() const { return Iterator{}; }
		Iterator end() const { return Iterator{}; }
	};

} //END: SubzeroECS