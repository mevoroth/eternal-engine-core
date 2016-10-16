#ifndef _LIGHT_GAME_OBJECT_HPP_
#define _LIGHT_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class TransformComponent;
		class LightComponent;
		class LightGameObjectData;

		class LightGameObject : public GameObject
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
	}
}

#endif
