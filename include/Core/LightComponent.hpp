#ifndef _LIGHT_COMPONENT_HPP_
#define _LIGHT_COMPONENT_HPP_

namespace Eternal
{
	namespace Components
	{
		class Light;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class LightComponent
		{
		public:
			Light* GetLight();

		private:
			Light* _Light = nullptr;
		};
	}
}

#endif
