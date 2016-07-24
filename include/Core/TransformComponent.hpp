#ifndef _TRANSFORM_COMPONENT_HPP_
#define _TRANSFORM_COMPONENT_HPP_

#include "Types/Types.hpp"
#include "GameComponent.hpp"
#include "Transform/Transform.hpp"
//#include "Resources/Pool.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal;
		using namespace Eternal::Types;
		using namespace Eternal::Components;
		//using namespace Eternal::Resources;

		class TransformComponent : public GameComponent
		{
		public:
			static void Init();

			TransformComponent();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

			void AttachTo(_In_ TransformComponent* Parent);

			const Matrix4x4& GetLocalToWorldTransform() const { return _LocalToWorldMatrix; }

			const Components::Transform& GetTransform() const;// { return _Transform; }
			Components::Transform& GetTransform();
			//{
			//	_Dirty = true;
			//	return _Transform;
			//}

		private:
			TransformComponent* _Parent = nullptr;
			//PoolHandle _TransformHandle = InvalidHandle;
			Transform _Transform;
			//Matrix4x4 _LocalToParentMatrix;
			Matrix4x4 _LocalToWorldMatrix;
			bool _Dirty = false;

			void _UpdateCache(bool& Dirty);
		};
	}
}

#endif
