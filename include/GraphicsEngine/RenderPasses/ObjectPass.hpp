#pragma once

#include "GraphicsEngine/Pass.hpp"
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

		using TransitionFunctorType	= std::function<void(_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer)>;
		using PerPassFunctorType	= std::function<void(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)>;
		using PerDrawFunctorType	= std::function<void(_In_ Material* InPerDrawMaterial)>;

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

			virtual const string& _GetPassName() const = 0;
			
			void _InitializeObjectPass(_In_ GraphicsContext& InContext, _In_ const ObjectPassCreateInformation& InObjectPassCreateInformation);
			template<
				typename TransitionFunctor,
				typename PerPassFunctor,
				typename PerDrawFunctor
			>
			void _RenderInternal(
				_In_ GraphicsContext& InContext,
				_In_ System& InSystem,
				_In_ Renderer& InRenderer,
				_In_ TransitionFunctor InTransitionFunction,
				_In_ PerPassFunctor InPerPassFunction,
				_In_ PerDrawFunctor InPerDrawFunction
			);

			InputLayout*		_ObjectInputLayout		= nullptr;
			RenderPass*			_ObjectRenderPass		= nullptr;
			DescriptorTable*	_ObjectDescriptorTable	= nullptr;
		};
	}
}
