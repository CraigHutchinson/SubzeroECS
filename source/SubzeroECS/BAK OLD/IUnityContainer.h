#pragma once

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace SubzeroECS
{
	/* Tuple of parameters for supplied types constructor */
	template< typename R, typename... Args>
	struct ConstructorArguments<R, R(Args...) >
	{
		typedef std::tuple<Args...> type;
	};

	/* Interface for object lifetime managment
	*/
	class ILifetime
	{
	public:
		virtual ~ILifetime() {}
		virtual std::shared_ptr<void> get() = 0;
	};

	namespace Lifetime
	{
		template<class Type>
		class Singleton : public ILifetime
		{
		public:
			virtual std::shared_ptr<void> get() override
			{
				if (!instance_)
					instance_ = std::make_shared<Type>();
				return instance_;
			}

		private:
			std::shared_ptr<Type> instance_;
		};

	} //END: Lifetime


	class IWorld
	{
		typedef std::unordered_map<std::type_index, std::unique_ptr<ILifetime>> TypeIndexLookup;
	public:

		template<class BaseType, class DerivedType>
		IWorld& registerType()
		{
			types_.emplace(
				std::make_pair( std::type_index(typeid(BaseType)), std::make_unique<Lifetime<DerivedType>>() ) );
			return *this;
		}


		template<class BaseType>
		std::shared_ptr<BaseType> resolve()
		{
			auto iFind = types_.find(std::type_index(typeid(BaseType)));
			if (iFind == types_.end())
			{
				return nullptr;
			}
			
			return std::static_pointer_cast<BaseType>(iFind->second->get());
		}



		TypeIndexLookup types_;
	};

} //END: SubzeroECS