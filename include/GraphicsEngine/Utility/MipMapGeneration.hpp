#pragma once

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
		class Pipeline;
		class RootSignature;
		class DescriptorTable;
	}

	namespace GraphicsEngine
	{
		using namespace Eternal::Graphics;

		enum class MipMapTextureType
		{
			MIPMAP_TEXTURE_TYPE_1D			= 0,
			MIPMAP_TEXTURE_TYPE_1D_ARRAY,
			MIPMAP_TEXTURE_TYPE_2D,
			MIPMAP_TEXTURE_TYPE_2D_ARRAY,
			MIPMAP_TEXTURE_TYPE_3D,
			MIPMAP_TEXTURE_TYPE_COUNT
		};

		enum class MipMapTextureFormat
		{
			MIPMAP_TEXTURE_FORMAT_RGBA8888_UNORM,
			MIPMAP_TEXTURE_FORMAT_RGB111110_FLOAT,
			MIPMAP_TEXTURE_FORMAT_COUNT
		};

		enum class MipMapThreadGroupCount
		{
			MIPMAP_THREAD_GROUP_COUNT_1 = 0,
			MIPMAP_THREAD_GROUP_COUNT_2,
			MIPMAP_THREAD_GROUP_COUNT_4,
			MIPMAP_THREAD_GROUP_COUNT_8,
			MIPMAP_THREAD_GROUP_COUNT_MAX
		};

		class MipMapGeneration
		{
		public:

			MipMapGeneration(_In_ GraphicsContext& InContext);
			~MipMapGeneration();

			Pipeline& GetPipeline(_In_ const MipMapTextureType& InMipMapTextureType, _In_ const MipMapTextureFormat& InMipMapTextureFormat, _In_ const MipMapThreadGroupCount& InMipMapThreadGroupCount);
			DescriptorTable* CreateDescriptorTable(_In_ GraphicsContext& InContext);

		private:
			RootSignature*	_MipMapRootSignature = nullptr;
			Pipeline*		_MipMapPipeline[static_cast<uint32_t>(MipMapTextureType::MIPMAP_TEXTURE_TYPE_COUNT)][static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT)][static_cast<uint32_t>(MipMapThreadGroupCount::MIPMAP_THREAD_GROUP_COUNT_MAX)] = {};
		};
	}
}
