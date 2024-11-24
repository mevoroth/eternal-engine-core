#include "GraphicsEngine/MipMapGeneration.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/PipelineFactory.hpp"
#include "Graphics/RootSignature.hpp"
#include "Graphics/RootSignatureFactory.hpp"
#include "Graphics/Shader.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		namespace MipMapGenerationPrivate
		{
			static constexpr char* MIPMAP_TEXTURE_TYPE_NAMES[] =
			{
				"MIPMAP_TEXTURE_TYPE_TEXTURE1D",
				"MIPMAP_TEXTURE_TYPE_TEXTURE1DARRAY",
				"MIPMAP_TEXTURE_TYPE_TEXTURE2D",
				"MIPMAP_TEXTURE_TYPE_TEXTURE2DARRAY",
				"MIPMAP_TEXTURE_TYPE_TEXTURE3D"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(MIPMAP_TEXTURE_TYPE_NAMES) == static_cast<uint32_t>(MipMapTextureType::MIPMAP_TEXTURE_TYPE_COUNT), "Mismatch between MipMapTextureType and MIPMAP_TEXTURE_TYPE_NAMES");

			static constexpr char* MIPMAP_TEXTURE_FORMAT_NAMES[] =
			{
				"RGBA8888 UNORM",
				"RGB111110 FLOAT"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(MIPMAP_TEXTURE_FORMAT_NAMES) == static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT), "Mismatch between MipMapTextureFormat and MIPMAP_TEXTURE_FORMAT_NAMES");

			static constexpr char* MIPMAP_TEXTURE_FORMAT_HLSLS[] =
			{
				"float4",
				"float3"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(MIPMAP_TEXTURE_FORMAT_HLSLS) == static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT), "Mismatch between MipMapTextureFormat and MIPMAP_TEXTURE_FORMAT_HLSLS");

			static constexpr char* MIPMAP_TEXTURE_FORMAT_SPIRVS[] =
			{
				"SPIRV_FORMAT_RGBA8",
				"SPIRV_FORMAT_R11FG11FB10F"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(MIPMAP_TEXTURE_FORMAT_HLSLS) == static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT), "Mismatch between MipMapTextureFormat and MIPMAP_TEXTURE_FORMAT_SPIRVS");
		}

		MipMapGeneration::MipMapGeneration(_In_ GraphicsContext& InContext)
		{
			using namespace MipMapGenerationPrivate;

			char MipMapShaderName[1024];
			char MipMapThreadGroupCountXString[4];
			char MipMapThreadGroupCountYString[4];

			_MipMapRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE)
					}
				)
			);

			for (uint32_t TextureTypeIndex = 0; TextureTypeIndex < static_cast<uint32_t>(MipMapTextureType::MIPMAP_TEXTURE_TYPE_COUNT); ++TextureTypeIndex)
			{
				for (uint32_t FormatIndex = 0; FormatIndex < static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT); ++FormatIndex)
				{
					for (uint32_t ThreadGroupMultiple = 0; ThreadGroupMultiple < static_cast<uint32_t>(MipMapThreadGroupCount::MIPMAP_THREAD_GROUP_COUNT_MAX); ++ThreadGroupMultiple)
					{
						uint32_t ThreadGroupCount = 1 << ThreadGroupMultiple;
						sprintf_s(MipMapShaderName, "MipMap %s %s %dx%d", MIPMAP_TEXTURE_TYPE_NAMES[TextureTypeIndex], MIPMAP_TEXTURE_FORMAT_NAMES[FormatIndex], ThreadGroupCount, ThreadGroupCount);
						sprintf_s(MipMapThreadGroupCountXString, "%d", ThreadGroupCount);
						sprintf_s(MipMapThreadGroupCountYString, "%d", ThreadGroupCount);

						_MipMapPipeline[TextureTypeIndex][FormatIndex][ThreadGroupMultiple] = CreatePipeline(
							InContext,
							ComputePipelineCreateInformation(
								_MipMapRootSignature,
								InContext.GetShader(
									ShaderCreateInformation(
										ShaderType::SHADER_TYPE_COMPUTE,
										MipMapShaderName,
										"MipMap/mipmap.compute.hlsl",
										{
											"MIPMAP_TEXTURE_TYPE",		MIPMAP_TEXTURE_TYPE_NAMES[TextureTypeIndex],
											"MIPMAP_TEXTURE_FORMAT",	MIPMAP_TEXTURE_FORMAT_HLSLS[FormatIndex],
											"MIPMAP_SPIRV_FORMAT",		MIPMAP_TEXTURE_FORMAT_SPIRVS[FormatIndex],
											"THREAD_GROUP_COUNT_X",		MipMapThreadGroupCountXString,
											"THREAD_GROUP_COUNT_Y",		MipMapThreadGroupCountYString,
											"THREAD_GROUP_COUNT_Z",		"1",
										}
									)
								)
							)
						);
					}
				}
			}
		}

		MipMapGeneration::~MipMapGeneration()
		{
			for (uint32_t TextureTypeIndex = 0; TextureTypeIndex < static_cast<uint32_t>(MipMapTextureType::MIPMAP_TEXTURE_TYPE_COUNT); ++TextureTypeIndex)
			{
				for (uint32_t FormatIndex = 0; FormatIndex < static_cast<uint32_t>(MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_COUNT); ++FormatIndex)
				{
					for (uint32_t ThreadGroupMultiple = 0; ThreadGroupMultiple < static_cast<uint32_t>(MipMapThreadGroupCount::MIPMAP_THREAD_GROUP_COUNT_MAX); ++ThreadGroupMultiple)
						DestroyPipeline(_MipMapPipeline[TextureTypeIndex][FormatIndex][ThreadGroupMultiple]);
				}
			}
			DestroyRootSignature(_MipMapRootSignature);
		}

		Pipeline& MipMapGeneration::GetPipeline(_In_ const MipMapTextureType& InMipMapTextureType, _In_ const MipMapTextureFormat& InMipMapTextureFormat, _In_ const MipMapThreadGroupCount& InMipMapThreadGroupCount)
		{
			uint32_t InMipMapTextureTypeInt				= static_cast<uint32_t>(InMipMapTextureType);
			uint32_t InMipMapTextureTextureFormatInt	= static_cast<uint32_t>(InMipMapTextureFormat);
			uint32_t InMipMapThreadGroupCountInt		= static_cast<uint32_t>(InMipMapThreadGroupCount);
			ETERNAL_ASSERT(_MipMapPipeline[InMipMapTextureTypeInt][InMipMapTextureTextureFormatInt][InMipMapThreadGroupCountInt]);
			return *_MipMapPipeline[InMipMapTextureTypeInt][InMipMapTextureTextureFormatInt][InMipMapThreadGroupCountInt];
		}

		DescriptorTable* MipMapGeneration::CreateDescriptorTable(_In_ GraphicsContext& InContext)
		{
			return _MipMapRootSignature->CreateRootDescriptorTable(InContext);
		}
	}
}
