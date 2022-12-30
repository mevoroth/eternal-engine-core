#pragma once

#include "GraphicsEngine/Pass.hpp"
#include <functional>

namespace Eternal
{
	namespace GraphicsEngine
	{
		using TransitionFunctorType	= std::function<void(_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer)>;
		using PerPassFunctorType	= std::function<void(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)>;

		class ObjectPass : public Pass
		{
		public:

			ObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~ObjectPass();

		protected:
			virtual const string& _GetPassName() const = 0;
			
			template<
				typename TransitionFunctor,
				typename PerPassFunctor
			>
			void _RenderInternal(
				_In_ GraphicsContext& InContext,
				_In_ System& InSystem,
				_In_ Renderer& InRenderer,
				_In_ TransitionFunctor InTransitionFunction,
				_In_ PerPassFunctor InPerPassFunction
			);

			InputLayout*		_ObjectInputLayout		= nullptr;
			RenderPass*			_ObjectRenderPass		= nullptr;
			DescriptorTable*	_ObjectDescriptorTable	= nullptr;
		};
	}
}
