#pragma once

#include <functional>

namespace Json
{
	class Value;
}

namespace Eternal
{
	namespace GameDataSystem
	{
		enum class GameDataSourceKey : uint32_t
		{
			CAMERAS		= 0,
			MESHES,
			LIGHTS,
			TRANSFORM
		};

		struct GameDataSource
		{
		public:
			virtual ~GameDataSource() {}

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const
			{
				ETERNAL_BREAK();
				return *static_cast<const GameDataSource*>(nullptr);
			}
			const GameDataSource& GetSubNode(_In_ const uint32_t& InKey) const
			{
				return GetSubNode(static_cast<GameDataSourceKey>(InKey));
			}
			template<typename GameDataType>
			void Get(_Out_ GameDataType& OutValue) const
			{
				InternalGet(&OutValue);
			}

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const {}
		};

		class GameData
		{
		public:
			template<typename GameDataType>
			GameDataType* Load(_In_ const GameDataSource& InNode) const
			{
				return static_cast<GameDataType*>(InternalLoad(InNode));
			}
		protected:
			virtual void* InternalLoad(_In_ const GameDataSource& InNode) const = 0;
		};

		using GameObjectFunction = std::function<void (_In_ const GameDataSource& InNode)>;
		extern void IterateGameDataCollection(_In_ const GameDataSource& InSource, _In_ GameObjectFunction Functor);
	}
}
