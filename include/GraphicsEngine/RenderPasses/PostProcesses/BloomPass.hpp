#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/PostProcesses/HLSLBloomConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class BloomPass final : public Pass
		{
		public:

			BloomPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~BloomPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override final;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override final;

		private:

			Pipeline*					_BloomDownsamplePipeline			= nullptr;
			RootSignature*				_BloomDownsampleRootSignature		= nullptr;
			DescriptorTable*			_BloomDownsampleDescriptorTable		= nullptr;
										
			Pipeline*					_BloomBlurHorizontalPipeline		= nullptr;
			RootSignature*				_BloomBlurHorizontalRootSignature	= nullptr;
			DescriptorTable*			_BloomBlurHorizontalDescriptorTable	= nullptr;
										
			Pipeline*					_BloomBlurVerticalPipeline			= nullptr;
			RootSignature*				_BloomBlurVerticalRootSignature		= nullptr;
			DescriptorTable*			_BloomBlurVerticalDescriptorTable	= nullptr;
										
			DescriptorTable*			_BloomCompositeDescriptorTable		= nullptr;
										
			MultiBuffered<Resource>*	_BloomConstantBuffer 				= nullptr;

		};
	}
}
