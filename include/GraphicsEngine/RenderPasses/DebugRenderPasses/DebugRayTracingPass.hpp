#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/RayTracing/Debug/HLSLRayGenerationConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class DebugRayTracingPass final : public Pass
		{
		public:
			static const string DebugRayTracingPassName;

			DebugRayTracingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override final;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override final;

		private:

			ShaderTable*							_DebugRayTracingShaderTable		= nullptr;
			DescriptorTable*						_DebugRayTracingDescriptorTable	= nullptr;
			ConstantBuffer<RayGenerationConstants>	_DebugRayTracingRayGenerationConstantBuffer;
		};
	}
}
