#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/PostProcesses/HLSLChromaticAberrationConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace HLSL;

		class ChromaticAberrationPass final : public Pass
		{
		public:

			ChromaticAberrationPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~ChromaticAberrationPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override final;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override final;

		private:

			DescriptorTable*								_ChromaticAberrationDescriptorTable = nullptr;
			ConstantBuffer<ChromaticAberrationConstants>	_ChromaticAberrationConstantBuffer;

		};
	}
}
