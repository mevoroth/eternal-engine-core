#pragma once

#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class DepthOnlyPass : public ObjectPass
		{
		public:
			static const string DepthOnlyPassName;

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		protected:
			DepthOnlyPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ const Viewport& InDepthOnlyViewport, _In_ View* InDepthStencilView, _In_ View* InDepthOnlyViewConstantBufferView);

			virtual const string& _GetPassName() const override;

			const Viewport&	_DepthOnlyViewport;
			View*			_DepthStencilView					= nullptr;
			View*			_DepthOnlyViewConstantBufferView	= nullptr;

		private:
			virtual void _BeginRender(_In_ System& InSystem, _In_ Renderer& InRenderer) = 0;
		};
	}
}
