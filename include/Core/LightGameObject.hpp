#ifndef _LIGHT_GAME_OBJECT_HPP_
#define _LIGHT_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class TransformComponent;
		class LightComponent;
		class LightGameObjectInstance;
		class LightGameObjectData;

		class LightGameObject : public InstanciableGameObject<LightGameObjectInstance>
		{
		public:
			LightGameObject();
			~LightGameObject();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			TransformComponent* GetTransformComponent();
			LightComponent* GetLightComponent();

		private:
			LightGameObjectData* _LightGameObjectData = nullptr;
		};

		class LightGameObjectInstance : public GameObjectInstance
		{
		public:
			LightGameObjectInstance();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;
		};
	}
}

#endif
