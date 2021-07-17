#pragma once

#include "Core/Component.hpp"
#include "Transform/Transform.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Components;

		class TransformComponent : public Component
		{
		public:

			inline Transform& GetTransform() { return _Transform; }

		private:
			Transform _Transform;
		};
	}
}
