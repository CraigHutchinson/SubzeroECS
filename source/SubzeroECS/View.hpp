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

			/** Optimized helper for 2-way intersection increment
			*/
			Iterator& begin2()
			{				
				auto& it1 = std::get<0>(iterators_);
				auto& it2 = std::get<1>(iterators_);
				auto end1 = std::get<0>(collections_).end();
				auto end2 = std::get<1>(collections_).end();

				// Advance past the current position
				if ( it1 == end1 )
				{
					return it2 = end2, *this; // Reached end
				}

				if ( it2 == end2 )
				{
					return it1 = end1, *this; // Reached end
				}

				//TODO: PERF does this help here? == Optimize for likely case where *it1 and *it2 are close
				if (*it2 == *it1)
					return *this; // *it1 and *it2 are equivalent (intersection found)

				return intersect2();
			}

			/** Optimized helper for 2-way intersection increment
			*/
			Iterator& increment2()
			{				
				auto& it1 = std::get<0>(iterators_);
				auto& it2 = std::get<1>(iterators_);
				auto end1 = std::get<0>(collections_).end();
				auto end2 = std::get<1>(collections_).end();

				// Advance past the current position
				if ( ++it1 == end1 )
				{
					return it2 = end2, *this; // Reached end
				}

				if ( ++it2 == end2 )
				{
					return it1 = end1, *this; // Reached end
				}

				//TODO: PERF does this help here? == Optimize for likely case where *it1 and *it2 are close
				if (*it2 == *it1)
					return *this; // *it1 and *it2 are equivalent (intersection found)

				return intersect2();
			}

			/** Optimized helper for 2-way intersection - find intersection point
			*/
			Iterator& intersect2()
			{
				auto& it1 = std::get<0>(iterators_);
				auto& it2 = std::get<1>(iterators_);
				auto end1 = std::get<0>(collections_).end();
				auto end2 = std::get<1>(collections_).end();

				// Find next intersection point
				do
				{
					if (*it1 < *it2)
					{
						if ( ++it1 == end1 )
						{
							return it2 = end2, *this; // Reached end
						}
					}
					else
					//TODO: PERF using == vs !..<
					if (!(*it2 < *it1)) // *it1 and *it2 are equivalent (intersection found)	
					{
						return *this;
					}
					else
					if ( ++it2 == end2 )
					{
						return it1 = end1, *this; // Reached end
					}
				} while (true);
			}

			/** Helper for N-way intersection (N >= 3)
			 * Uses adaptive galloping algorithm based on:
			 * - https://www.vldb.org/pvldb/vol8/p293-inoue.pdf (VLDB 2015, Inoue et al.)
			 * - https://ceur-ws.org/Vol-2840/short2.pdf
			 * 
			 * Strategy: "Max-Skip" approach
			 * 1. Find maximum EntityId among all current iterator positions
			 * 2. Advance lagging iterators using binary search (for large gaps)
			 * 3. Check if all iterators now point to same EntityId (intersection found)
			 * 4. Repeat until intersection found or any iterator reaches end
			 * 
			 * Complexity: O(n log k) where n is size of smallest set, k is max skip distance
			 */
			template<std::size_t... Is>
			Iterator& intersectN( std::index_sequence<Is...> )
			{
				// Threshold for switching from linear scan to binary search (galloping)
				// Based on VLDB paper: small gaps benefit from linear scan (better cache locality)
				//TODO: Should this size be defined by cache line read size?
				constexpr std::size_t GallopingThreshold = 32;
				
				// Main galloping intersection loop
				while (true)
				{
					// Step 1: Find the maximum EntityId among all current positions
					EntityId maxId = *std::get<0>(iterators_);  // Start with first iterator's value
					((void)(Is == 0 ? void() : (void)(maxId = std::max(maxId, *std::get<Is>(iterators_)))), ...);
					
					// Step 2: Advance all iterators that are behind maxId
					bool allAtMax = true;
					bool anyAtEnd = false;
					
					// Process each iterator
					([&]()
					{
						auto& it = std::get<Is>(iterators_);
						auto end = std::get<Is>(collections_).end();
						
						if (*it < maxId)
						{
							allAtMax = false;
							
							// Adaptive: use linear scan for small gaps, binary search for large gaps
							std::size_t linearCount = 0;
							
							// Try linear scan first up to threshold
							while (linearCount < GallopingThreshold && it != end && *it < maxId)
							{
								++it;
								++linearCount;
							}
							
							if (it != end )
							{
								if( *it < maxId)
								{
									// Gap is large - use binary search (galloping) from current position
									it = std::lower_bound(it, end, maxId);
									if ( it == end )
									{
										anyAtEnd = true;
									}
								}
							}
							else
							{
								anyAtEnd = true;
							}
						}
					}(), ...);
					
					// Step 3: Check termination conditions
					if (anyAtEnd)
					{
						// At least one iterator reached end - no more intersections
						// Set all to end for consistency
						((std::get<Is>(iterators_) = std::get<Is>(collections_).end()), ...);
						return *this;
					}
					
					if (allAtMax)
					{
						// All iterators point to maxId - intersection found!
						return *this;
					}
					
					// Continue loop - new max will be computed in next iteration
				}
			}

			template<std::size_t... Is>
			Iterator& beginN( std::index_sequence<Is...> )
			{
				// Check if any iterator is already at end
				if (((std::get<Is>(iterators_) == std::get<Is>(collections_).end()) || ...))
				{
					// Set all to end
					((std::get<Is>(iterators_) = std::get<Is>(collections_).end()), ...);
					return *this;
				}

				// Check if all iterators already point to the same EntityId
				EntityId firstId = *std::get<0>(iterators_);
				bool allMatch = ((Is == 0 || *std::get<Is>(iterators_) == firstId) && ...);
				
				if (allMatch)
				{
					// Already at an intersection
					return *this;
				}

				// Need to find first intersection
				return intersectN( std::index_sequence<Is...>{} );
			}

			template<std::size_t... Is>
			Iterator& incrementN( std::index_sequence<Is...> )
			{
				// Advance all iterators by one position
				((++std::get<Is>(iterators_)), ...);
				
				// Check if any iterator reached end after increment
				if (((std::get<Is>(iterators_) == std::get<Is>(collections_).end()) || ...))
				{
					// Set all to end
					((std::get<Is>(iterators_) = std::get<Is>(collections_).end()), ...);
					return *this;
				}

				// Check if we're already at an intersection after increment
				EntityId firstId = *std::get<0>(iterators_);
				bool allMatch = ((Is == 0 || *std::get<Is>(iterators_) == firstId) && ...);
				
				if (allMatch)
				{
					// Lucky! Already at next intersection
					return *this;
				}

				// Need to find next intersection
				return intersectN( std::index_sequence<Is...>{} );
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