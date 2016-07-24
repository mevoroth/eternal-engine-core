#ifndef _MESH_COMPONENT_HPP_
#define _MESH_COMPONENT_HPP_

#include "GameComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		class MeshComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;
		};
	}
}

#endif
