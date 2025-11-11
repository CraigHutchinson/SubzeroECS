#pragma once

#include <algorithm> //< std::lower_bound
#include <tuple>

#include "EntityId.hpp"

namespace SubzeroECS
{
	/** Set intersection algorithms for iterating over entities with multiple components.
	 * 
	 * Provides optimized implementations for:
	 * - 2-way intersection (classic merge algorithm)
	 * - N-way intersection (adaptive galloping algorithm)
	 * 
	 * Based on research from:
	 * - https://www.vldb.org/pvldb/vol8/p293-inoue.pdf (VLDB 2015, Inoue et al.)
	 * - https://ceur-ws.org/Vol-2840/short2.pdf
	 */
	namespace Intersection
	{
		/** Optimized 2-way intersection using classic merge algorithm.
		 * 
		 * Advances iterators to find the next intersection point.
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * @tparam Iterator Type of iterator (must support ++, *, <, ==, and comparison to end)
		 * @param it1 First iterator (modified in place on success)
		 * @param it2 Second iterator (modified in place on success)
		 * @param end1 End iterator for first collection
		 * @param end2 End iterator for second collection
		 * @return true if intersection found, false if any iterator reached end
		 */
		template<typename Iterator>
		bool intersect2(Iterator& it1, Iterator& it2, Iterator end1, Iterator end2)
		{
			// Find next intersection point
			while (true)
			{
				if (*it1 < *it2)
				{
					if (++it1 == end1)
					{
						return false; // Reached end
					}
				}
				else if (!(*it2 < *it1)) // *it1 and *it2 are equivalent (intersection found)	
				{
					return true;
				}
				else
				{
					if (++it2 == end2)
					{
						return false; // Reached end
					}
				}
			}
		}

		/** Check if two iterators are at the same position for 2-way begin.
		 * 
		 * Checks if already at an intersection or finds the first one.
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * @tparam Iterator Type of iterator
		 * @param it1 First iterator
		 * @param it2 Second iterator
		 * @param end1 End iterator for first collection
		 * @param end2 End iterator for second collection
		 * @return true if at intersection, false if any iterator at end
		 */
		template<typename Iterator>
		bool begin2(Iterator& it1, Iterator& it2, Iterator end1, Iterator end2)
		{
			if (it1 == end1 || it2 == end2)
			{
				return false; // At end
			}

			// Check if already at intersection
			if (*it2 == *it1)
				return true;

			// Find first intersection
			return intersect2(it1, it2, end1, end2);
		}

		/** Increment both iterators and find next intersection for 2-way.
		 * 
		 * Advances both iterators past current position and finds next intersection.
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * @tparam Iterator Type of iterator
		 * @param it1 First iterator (will be incremented)
		 * @param it2 Second iterator (will be incremented)
		 * @param end1 End iterator for first collection
		 * @param end2 End iterator for second collection
		 * @return true if next intersection found, false if any iterator reached end
		 */
		template<typename Iterator>
		bool increment2(Iterator& it1, Iterator& it2, Iterator end1, Iterator end2)
		{
			// Advance past current position
			if (++it1 == end1 || ++it2 == end2)
			{
				return false; // Reached end
			}

			// Quick check if already at next intersection
			if (*it2 == *it1)
				return true;

			// Find next intersection
			return intersect2(it1, it2, end1, end2);
		}

		/** N-way intersection using adaptive galloping algorithm.
		 * 
		 * Uses the "max-skip" strategy:
		 * 1. Find maximum EntityId among all current positions
		 * 2. Advance lagging iterators adaptively:
		 *    - Linear scan for small gaps (better cache locality)
		 *    - Binary search for large gaps (galloping)
		 * 3. Repeat until all iterators converge or any reaches end
		 * 
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * Complexity: O(n log k) where n is size of smallest set, k is max skip distance
		 * 
		 * @tparam Is Index sequence for parameter pack expansion
		 * @tparam Iterators Tuple of iterator types
		 * @param iterators Tuple of iterators (modified in place on success)
		 * @param endIterators Tuple of end iterators (same type as iterators)
		 * @return true if intersection found, false if any iterator reached end
		 */
		template<std::size_t... Is, typename Iterators>
		bool intersectN(std::index_sequence<Is...>, Iterators& iterators, const Iterators& endIterators)
		{
			// Threshold for switching from linear scan to binary search (galloping)
			// Based on VLDB paper: small gaps benefit from linear scan (better cache locality)
			// TODO: Should this size be defined by cache line read size?
			constexpr std::size_t GallopingThreshold = 32;
			
			// Main galloping intersection loop
			while (true)
			{
				// Step 1: Find the maximum EntityId among all current positions
				EntityId maxId = *std::get<0>(iterators);  // Start with first iterator's value
				((void)(Is == 0 ? void() : (void)(maxId = std::max(maxId, *std::get<Is>(iterators)))), ...);
				
				// Step 2: Advance all iterators that are behind maxId
				bool allAtMax = true;
				bool anyAtEnd = false;
				
				// Process each iterator
				([&]()
				{
					auto& it = std::get<Is>(iterators);
					auto end = std::get<Is>(endIterators);
					
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
						
						if (it != end)
						{
							if (*it < maxId)
							{
								// Gap is large - use binary search (galloping) from current position
								it = std::lower_bound(it, end, maxId);
								if (it == end)
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
					return false;
				}
				
				if (allAtMax)
				{
					// All iterators point to maxId - intersection found!
					return true;
				}
				
				// Continue loop - new max will be computed in next iteration
			}
		}

		/** Check if all iterators are already at intersection for N-way begin.
		 * 
		 * Checks if already at an intersection or finds the first one.
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * @tparam Is Index sequence for parameter pack expansion
		 * @tparam Iterators Tuple of iterator types
		 * @param iterators Tuple of iterators (modified in place on success)
		 * @param endIterators Tuple of end iterators (same type as iterators)
		 * @return true if at intersection, false if any iterator at end
		 */
		template<std::size_t... Is, typename Iterators>
		bool beginN(std::index_sequence<Is...> indices, Iterators& iterators, const Iterators& endIterators)
		{
			// Check if any iterator is already at end
			if (((std::get<Is>(iterators) == std::get<Is>(endIterators)) || ...))
			{
				return false; // At end
			}

			// Check if all iterators already point to the same EntityId
			EntityId firstId = *std::get<0>(iterators);
			bool allMatch = ((Is == 0 || *std::get<Is>(iterators) == firstId) && ...);
			
			if (allMatch)
			{
				// Already at an intersection
				return true;
			}

			// Need to find first intersection
			return intersectN(indices, iterators, endIterators);
		}

		/** Increment all iterators and find next intersection for N-way.
		 * 
		 * Advances all iterators past current position and finds next intersection.
		 * Does NOT modify iterators on failure - caller should handle end assignment.
		 * 
		 * @tparam Is Index sequence for parameter pack expansion
		 * @tparam Iterators Tuple of iterator types
		 * @param iterators Tuple of iterators (modified in place on success)
		 * @param endIterators Tuple of end iterators (same type as iterators)
		 * @return true if next intersection found, false if any iterator reached end
		 */
		template<std::size_t... Is, typename Iterators>
		bool incrementN(std::index_sequence<Is...> indices, Iterators& iterators, const Iterators& endIterators)
		{
			// Advance all iterators by one position
			// + Check if any iterator reached end after increment
			if (((++std::get<Is>(iterators) == std::get<Is>(endIterators)) || ...))
			{
				return false; // At end
			}

			// Check if we're already at an intersection after increment
			EntityId firstId = *std::get<0>(iterators);
			bool allMatch = ((Is == 0 || *std::get<Is>(iterators) == firstId) && ...);
			
			if (allMatch)
			{
				// Lucky! Already at next intersection
				return true;
			}

			// Need to find next intersection
			return intersectN(indices, iterators, endIterators);
		}

	} // namespace Intersection
} // namespace SubzeroECS
