#ifndef _TRANSFORM_COMPONENT_HPP_
#define _TRANSFORM_COMPONENT_HPP_

#include "GameComponent.hpp"
#include "Transform/Transform.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal;

		class TransformComponent : public GameComponent
		{
		public:
			TransformComponent();

			virtual void Begin() override;
			virtual void Update(const TimeT& ElapsedMilliseconds) override;
			virtual void End() override;

			void AttachTo(TransformComponent* Parent);
			Components::Transform GetRelativeToWorldComputed() const;

			Components::Transform Transform;
		private:
			Components::Transform _RelativeToWorld;
			TransformComponent* _Parent;
		};
	}
}

#endif
