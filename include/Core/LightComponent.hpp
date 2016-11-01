#ifndef _LIGHT_COMPONENT_HPP_
#define _LIGHT_COMPONENT_HPP_

namespace Eternal
{
	namespace Components
	{
		class Light;
		class Shadow;
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

			Shadow* GetShadow();
			void SetShadow(_In_ Shadow* ShadowObj);

			bool GetShadowEnabled() const;

		private:
			Light* _Light = nullptr;
			Shadow* _Shadow = nullptr;
		};
	}
}

#endif
