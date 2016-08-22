#ifndef _CHARACTER_GAME_OBJECT_HPP_
#define _CHARACTER_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class CharacterGameObjectData;
		class TransformComponent;
		class MeshComponent;

		class CharacterGameObject : public GameObject
		{
		public:
			CharacterGameObject();

			TransformComponent* GetTransformComponent();
			MeshComponent* GetMeshComponent();

		private:
			CharacterGameObjectData* _CharacterGameObjectData = nullptr;
		};
	}
}

#endif
