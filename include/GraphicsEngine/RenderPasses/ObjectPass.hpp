#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "HLSLPerDrawConstants.hpp"
#include "HLSLPerInstanceInformation.hpp"
#include <functional>

namespace Eternal
{
	namespace Components
	{
		class Material;
	}

	namespace GraphicsEngine
	{
		using namespace Eternal::Components;

		using TransitionFunctionType	= std::function<void(_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer)>;
		using PerPassFunctionType		= std::function<void(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)>;
		using PerDrawFunctionType		= std::function<void(_In_ Material* InPerDrawMaterial)>;
		using IsVisibleFunctionType		= std::function<bool(_In_ uint32_t InKey)>;

		static constexpr bool UseMeshPipeline = false;

		struct ObjectPassCreateInformation
		{
			const vector<string>&					Defines;
			const vector<RootSignatureParameter>&	RootSignatureParameters;
			RenderPassCreateInformation				RenderPassInformation;
			Shader*									ObjectPS = nullptr;
		};

		class ObjectPass : public Pass
		{
		public:

			virtual ~ObjectPass();

		protected:

			ObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual const string& _GetPassName() const = 0;
			
			void _InitializeObjectPass(_In_ GraphicsContext& InContext, _In_ const ObjectPassCreateInformation& InObjectPassCreateInformation);
			template<
				typename TransitionFunction,
				typename PerPassFunction,
				typename PerDrawFunction,
				typename IsVisibleFunction
			>
			void _RenderInternal(
				_In_ GraphicsContext& InContext,
				_In_ System& InSystem,
				_In_ Renderer& InRenderer,
				_In_ TransitionFunction InTransitionFunctor,
				_In_ PerPassFunction InPerPassFunctor,
				_In_ PerDrawFunction InPerDrawFunctor,
				_In_ IsVisibleFunction InIsVisibleFunctor
			);

			InputLayout*								_ObjectInputLayout					= nullptr;
			RenderPass*									_ObjectRenderPass					= nullptr;
			DescriptorTable*							_ObjectDescriptorTable				= nullptr;
			MultiBuffered<Resource>*					_ObjectPerDrawInstanceBuffer		= nullptr;
			StructuredBuffer<PerInstanceInformation>	_ObjectPerInstanceBuffer;
		};
	}
}
