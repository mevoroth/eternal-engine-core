#ifndef _MATERIAL_COMPONENT_HPP_
#define _MATERIAL_COMPONENT_HPP_

#include "Core/GameComponent.hpp"

namespace Eternal
{
	namespace GraphicData
	{
		class Material;
	}

	namespace Core
	{
		using namespace Eternal::GraphicData;

		class MaterialComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			Material* GetMaterial();
			void SetMaterial(_In_ Material* MaterialObj);

		private:
			Material* _Material = nullptr;
		};
	}
}

#endif
