#pragma once

#include <tuple>
#include "Collection.hpp"
#include "View.hpp"

namespace SubzeroECS
{
#if 1
	class ISystem
	{
	public:

		/** Update the system which wil call update(SubzeroECS::EntityId) for each entity in view
		 */
		virtual void update() = 0;
	
	protected:
		/** Update function called for each entity matching the system view
		 * @param[in]  entityId  The entity entity identifier for reference		 
		 */
		virtual void update( const SubzeroECS::EntityId entityId ) = 0;
	};

	template< typename... Components >
	class System : public ISystem, protected View<Components...>
	{
	public:
		/** @note C++11 std::make_tuple
		*/
		System( CollectionRegistry& registry )
			: View( registry )
		{
		}

		virtual void update() override
		{
			const auto iEnd = end();
			for (�autoSubzeroECSntty = begin();SubzeroECSntty != iEnd; +SubzeroECSntty) 
			{ 
				update( *iSubzeroECSty );
			} 
		}

	};
#endif

} //END: SubzeroECS
