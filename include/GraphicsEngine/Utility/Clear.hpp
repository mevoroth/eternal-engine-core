#pragma once

#include "Graphics/MultiBuffered.hpp"
#include "Graphics/Resource.hpp""
#include "ShadersReflection/Utility/HLSLClearConstants.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
		class Resource;
		class RootSignature;
		class DescriptorTable;
		class Pipeline;
		class Resource;
		class View;
	}

	namespace Types
	{
		struct Vector4;
	}

	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;
		using namespace Eternal::Graphics;
		
		enum class ClearTextureFormat
		{
			CLEAR_TEXTURE_FORMAT_RGBA8888_UNORM,
			CLEAR_TEXTURE_FORMAT_RGB111110_FLOAT,
			CLEAR_TEXTURE_FORMAT_COUNT,
			CLEAR_TEXTURE_FORMAT_INVALID = CLEAR_TEXTURE_FORMAT_COUNT
		};

		enum class ClearThreadGroupCount
		{
			CLEAR_THREAD_GROUP_COUNT_1 = 0,
			CLEAR_THREAD_GROUP_COUNT_2,
			CLEAR_THREAD_GROUP_COUNT_4,
			CLEAR_THREAD_GROUP_COUNT_8,
			CLEAR_THREAD_GROUP_COUNT_MAX
		};

		class Clear
		{
		public:

			Clear(_In_ GraphicsContext& InContext);
			~Clear();

			void BeginRender();
			void EndRender();
			void ClearView(_In_ GraphicsContext& InContext, _In_ View* InView, const Types::Float4& InClearValue);

		private:
			
			static constexpr uint32_t	ClearPerFrameCount	= 128;
			static constexpr uint32_t	ClearInvalidIndex 	= ~0u;
			
			vector<DescriptorTable*>	_ClearDescriptorTables;
			Pipeline*					_ClearPipelines[static_cast<uint32_t>(ResourceDimension::RESOURCE_DIMENSION_COUNT)][static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT)][static_cast<uint32_t>(ClearThreadGroupCount::CLEAR_THREAD_GROUP_COUNT_MAX)] = {};
			RootSignature*				_ClearRootSignature		= nullptr;
			MultiBuffered<Resource>*	_ClearConstantBuffer	= nullptr;
			ClearConstants*				_ClearConstantsPointer	= nullptr;
			uint32_t					_ClearConstantsIndex	= ClearInvalidIndex;

		};
	}
}
