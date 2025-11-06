#pragma once

#include <tuple>
#include "Collection.hpp"
#include "View.hpp"

namespace SubzeroECS
{
	// Base interface for type-erased system storage
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void update() = 0;
	};

	// CRTP-based System class for zero-overhead virtual calls
	template<typename Derived, typename... Components>
	class System : public ISystem, protected View<Components...>
	{
	public:
		using ViewType = View<Components...>;
		using Iterator = typename ViewType::Iterator;

		System(CollectionRegistry& registry)
			: View<Components...>(registry)
			, registry_(registry)
		{
		}

		// Non-virtual update that calls derived class's processEntity
		void update() override
		{
			const auto iEnd = this->ViewType::end();
			for (auto iEntity = this->ViewType::begin(); iEntity != iEnd; ++iEntity)
			{
				static_cast<Derived*>(this)->processEntity(iEntity);
			}
		}

	protected:
		// Helper to get a component by EntityId
		template<typename Component>
		Component& get(SubzeroECS::EntityId entityId)
		{
			return registry_.get<Component>().get(entityId);
		}

	private:
		CollectionRegistry& registry_;
	};

} //END: SubzeroECS
