#pragma once

#include <algorithm> //< std::all_of, std::distance, std::lower_bound
#include <array>
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
				// Advance to first valid iterator position
				if ( !isAtValid() )
					advance();
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
				// Advance to next valid iterator position
				return advance();
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
			
			// Check if all iterators point to the same Entity or all point to end
			bool isAtValid()
			{
				if constexpr (sizeof...(Components) <= 1)
				{
					// None or Single component - always valid or at end
					return true;
				}
				else
				{					
					auto state = makeIterationState( collections_, iterators_ );
					return std::apply( [](auto&... pairings)
					{
						// get all EntityIds from the iterators
						std::array<EntityId, sizeof...(Components)> iEntityIds = { 
							(pairings.second == pairings.first.end() ? EntityId::Invalid : *pairings.second)... };

						// If all are equal to first or all are at end then valid
						return std::all_of(iEntityIds.begin(), iEntityIds.end()
							, [cmp=iEntityIds[0]](EntityId v) { return v == cmp; });
					}, state );
				}
			}

			// Helper for N-way intersection (N >= 3)
			template<std::size_t... Is>
			Iterator& advanceN( std::index_sequence<Is...> )
			{
				// 2-or-more-way intersection
				auto its = std::make_tuple( std::ref(std::get<Is>(iterators_))... );
				auto iends = std::make_tuple( std::get<Is>(collections_).end()... );

				auto notAnyAtEnd = [&its, &iends]() {
					return ((std::get<Is>(its) != std::get<Is>(iends)) && ...);
				};

				auto setAllAtEnd = [&its, &iends]() {
					((std::get<Is>(its) = std::get<Is>(iends)), ...);
				};

				auto idsAllMatch = [&its]() {
					if constexpr (sizeof...(Is) <= 1)
						return true;
					else
					{
						EntityId firstId = *std::get<0>(its);
						return ((Is == 0 || *std::get<Is>(its) == firstId) && ...);
					}
				};

				// Update all iterators that are less than max and update max accordingly
				auto incrementIfLessThanMaxId = [&its, &iends]( EntityId validId ) {
					EntityId maxId = validId;
					(
						[&]() {
							while (*std::get<Is>(its) < validId)
							{
								++std::get<Is>(its);
								const auto id = getId<Is>(its, iends);
								if (id > maxId) // Update max if we found a larger value or EntityId::Invalid if we are at end
								{
									maxId = id;
									break;
								}
							}
						}()
					, ...);
					return maxId;
				};

				// Advance past the current position
				++std::get<0>(its);
				EntityId maxId = getId<0>(its, iends);

				// Find next intersection point
				while ( maxId != EntityId::Invalid )
				{
					// Advance all iterators that are less than max
					EntityId nextValidId = incrementIfLessThanMaxId(maxId);
					if ( nextValidId == maxId ) //< All matched up to max
						return *this; // Found intersection

					maxId = nextValidId;
				}
				
				// No more intersections, set all to end
				setAllAtEnd();

				return *this;
			}

			Iterator& advance()
			{
				// Incrementing at end is an error
				assert(std::get<0>(iterators_) != std::get<0>(collections_).end());

				// Set-intersection operation over all component collections			
				if constexpr (sizeof...(Components) == 0)
				{
					// No components - empty view, always at end
					// Nothing to increment
				}
				else if constexpr (sizeof...(Components) == 1)
				{
					// Single component - just advance the iterator
					++std::get<0>(iterators_);
				}
#if 0 // TODO: Profile vs the generic option! 
				else if constexpr (sizeof...(Components) == 2)
				{
					// Two-way intersection using classic std::set_intersection algorithm
					auto& it1 = std::get<0>(iterators_);
					auto& it2 = std::get<1>(iterators_);
					auto end1 = std::get<0>(collections_).end();
					auto end2 = std::get<1>(collections_).end();
					
					// Advance past the current position
					++it1;
					
					// Find next intersection point
					while (it1 != end1 && it2 != end2)
					{
						if (*it1 < *it2)
							++it1;
						else
						{
							if (!(*it2 < *it1))
								return *this; // *it1 and *it2 are equivalent (intersection found)							
							++it2;
						}
					}
					
					// No more intersections, set both to end
					it1 = end1;
					it2 = end2;
				}	
#endif
				else
				{
					advanceN( std::make_index_sequence<sizeof...(Components)>{} );
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

} //END: SubzeroECS