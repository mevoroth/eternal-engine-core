#ifndef _TRANSFORM_COMPONENT_HPP_
#define _TRANSFORM_COMPONENT_HPP_

#include "GameComponent.hpp"
#include "Transform/Transform.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Components;

		class TransformComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(const TimeT& ElapsedMilliseconds) override;
			virtual void End() override;

			Transform Transform;
		};
	}
}

#endif
