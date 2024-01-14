#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/HLSLVolumetricCloudsConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class VolumetricCloudsPass final : public Pass
		{
		public:
			VolumetricCloudsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~VolumetricCloudsPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override;

		private:

			DescriptorTable*							_VolumetricCloudsDescriptorTable	= nullptr;
			ConstantBuffer<VolumetricCloudsConstants>	_VolumetricCloudsConstantBuffer;

		};
	}
}
