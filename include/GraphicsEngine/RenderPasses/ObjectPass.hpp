#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "Material/MaterialType.hpp"
#include "ShadersReflection/HLSLPerDrawConstants.hpp"
#include "ShadersReflection/HLSLPerInstanceInformation.hpp"
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
		using PerDrawFunctionType		= std::function<void(_In_ Material* InPerDrawMaterial, _In_ Renderer& InRenderer)>;
		using IsVisibleFunctionType		= std::function<bool(_In_ uint32_t InKey)>;

		static constexpr bool UseMeshPipeline = false;

		struct ObjectPassCreateInformation
		{
			ObjectPassCreateInformation(
				_In_ const vector<string>& InDefines,
				_In_ const vector<RootSignatureParameter>& InRootSignatureParameters,
				_In_ const RenderPassCreateInformation& InRenderPassInformation,
				_In_ Shader* InObjectPixel = nullptr,
				_In_ const Rasterizer& InObjectRasterizer = RasterizerDefault,
				_In_ const DepthStencil& InObjectDepthStencil = DepthStencilTestWriteNone
			)
				: Defines(InDefines)
				, RootSignatureParameters(InRootSignatureParameters)
				, RenderPassInformation(InRenderPassInformation)
				, ObjectPixel(InObjectPixel)
				, ObjectRasterizer(InObjectRasterizer)
				, ObjectDepthStencil(InObjectDepthStencil)
			{
			}

			const vector<string>&					Defines;
			const vector<RootSignatureParameter>&	RootSignatureParameters;
			RenderPassCreateInformation				RenderPassInformation;
			Shader*									ObjectPixel			= nullptr;
			const Rasterizer&						ObjectRasterizer	= RasterizerDefault;
			const DepthStencil&						ObjectDepthStencil	= DepthStencilTestWriteNone;
		};

		class ObjectPass : public Pass
		{
		public:

			virtual ~ObjectPass();

		protected:

			ObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ uint32_t InInstanceCount);

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
			MaterialType								_ObjectBucket						= Components::MaterialType::MATERIAL_TYPE_COUNT;
		};
	}
}
