#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class RayTracedReflectionsPass final : public Pass
		{
		public:
			static const string RayTracedReflectionsPassName;

			RayTracedReflectionsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override;

		private:

			ShaderTable*		_RayTracedReflectionsShaderTable		= nullptr;
			DescriptorTable*	_RayTracedReflectionsDescriptorTable	= nullptr;
		};
	}
}
