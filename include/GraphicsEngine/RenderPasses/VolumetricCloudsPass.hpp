#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class VolumetricCloudsPass final : public Pass
		{
		public:
			VolumetricCloudsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~VolumetricCloudsPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

		private:

			DescriptorTable*			_VolumetricCloudsDescriptorTable	= nullptr;
			MultiBuffered<Resource>*	_VolumetricCloudsConstantBuffer		= nullptr;
			MultiBuffered<View>*		_VolumetricCloudsConstantBufferView	= nullptr;

		};
	}
}
