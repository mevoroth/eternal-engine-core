#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/PostProcesses/HLSLCRTConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace HLSL;

		class CRTPass final : public Pass
		{
		public:

			CRTPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~CRTPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override final;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override final;

		private:

			DescriptorTable*				_CRTDescriptorTable = nullptr;
			ConstantBuffer<CRTConstants>	_CRTConstantBuffer;

		};
	}
}
