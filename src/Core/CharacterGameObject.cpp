#include "Core/CharacterGameObject.hpp"

#include "GamePools.hpp"

namespace Eternal
{
	namespace Core
	{
		class CharacterGameObjectData
		{
		public:
			CharacterGameObjectData()
			{
				_TransformComponent = g_TransformComponentPool->Initialize(_TransformHandle);
			}

			GameComponent* GetTransformComponent()
			{
				return _TransformComponent;
			}

			GameComponent* GetMeshComponent()
			{
				return _MeshComponent;
			}

		private:
			PoolHandle _TransformHandle = InvalidHandle;
			GameComponent* _TransformComponent = nullptr;
			GameComponent* _MeshComponent = nullptr;
		};
	}
}

using namespace Eternal::Core;

CharacterGameObject::CharacterGameObject()
{
	_CharacterGameObjectData = new CharacterGameObjectData();
}

TransformComponent* CharacterGameObject::GetTransformComponent()
{
	return (TransformComponent*)_CharacterGameObjectData->GetTransformComponent();
}

MeshComponent* CharacterGameObject::GetMeshComponent()
{
	return (MeshComponent*)_CharacterGameObjectData->GetMeshComponent();
}
