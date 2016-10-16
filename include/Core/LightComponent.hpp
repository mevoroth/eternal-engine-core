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
			static void Initialize();
			static void Release();

			void SetLight(_In_ Light* LightObj);
			Light* GetLight();

		private:
			Light* _Light = nullptr;
		};
	}
}

#endif
