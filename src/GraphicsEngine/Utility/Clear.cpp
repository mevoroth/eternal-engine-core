#include "GraphicsEngine/Utility/Clear.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "Math/Math.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::Types;

		namespace ClearPrivate
		{
			static constexpr char* CLEAR_TEXTURE_FORMAT_NAMES[] =
			{
				"RGBA8888 UNORM",
				"RGB111110 FLOAT"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(CLEAR_TEXTURE_FORMAT_NAMES) == static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT), "Mismatch between ClearTextureFormat and CLEAR_TEXTURE_FORMAT_NAMES");

			static uint32_t CLEAR_TEXTURE_CHANNELS[] =
			{
				4,
				3
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(CLEAR_TEXTURE_CHANNELS) == static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT), "Mismatch between ClearTextureFormat and CLEAR_TEXTURE_CHANNELS");

			static constexpr char* CLEAR_TEXTURE_FORMAT_SPIRVS[] =
			{
				"SPIRV_FORMAT_RGBA8",
				"SPIRV_FORMAT_R11FG11FB10F"
			};
			ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(CLEAR_TEXTURE_FORMAT_SPIRVS) == static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT), "Mismatch between ClearTextureFormat and CLEAR_TEXTURE_FORMAT_SPIRVS");
		
			ClearTextureFormat ConvertFormatToClearTextureFormat(_In_ const Format& InFormat)
			{
				switch (InFormat)
				{
				case Format::FORMAT_RGBA8888_UNORM:
					return ClearTextureFormat::CLEAR_TEXTURE_FORMAT_RGBA8888_UNORM;

				case Format::FORMAT_RGB111110_FLOAT:
					return ClearTextureFormat::CLEAR_TEXTURE_FORMAT_RGB111110_FLOAT;

				default:
					break;
				}

				return ClearTextureFormat::CLEAR_TEXTURE_FORMAT_INVALID;
			}

			uint32_t ConvertFormatToClearTextureFormatInt(_In_ const Format& InFormat)
			{
				return static_cast<uint32_t>(ConvertFormatToClearTextureFormat(InFormat));
			}
		}

		constexpr uint32_t Clear::ClearPerFrameCount;
		constexpr uint32_t Clear::ClearInvalidIndex;

		Clear::Clear(_In_ GraphicsContext& InContext)
		{
			using namespace ClearPrivate;

			char ClearShaderName[1024];
			char ClearChannelCount[2];
			char ClearThreadGroupCountXString[4];
			char ClearThreadGroupCountYString[4];

			_ClearRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,    RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,         RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE)
					}
				)
			);
			
			for (uint32_t ResourceDimensionIndex = 0; ResourceDimensionIndex < static_cast<uint32_t>(ResourceDimension::RESOURCE_DIMENSION_COUNT); ++ResourceDimensionIndex)
			{
				for (uint32_t FormatIndex = 0; FormatIndex < static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT); ++FormatIndex)
				{
					for (uint32_t ThreadGroupMultiple = 0; ThreadGroupMultiple < static_cast<uint32_t>(ClearThreadGroupCount::CLEAR_THREAD_GROUP_COUNT_MAX); ++ThreadGroupMultiple)
					{
						uint32_t ThreadGroupCount = 1 << ThreadGroupMultiple;
						sprintf_s(ClearShaderName, "Clear %s %s %dx%d", ResourceDimensionString[ResourceDimensionIndex], CLEAR_TEXTURE_FORMAT_NAMES[FormatIndex], ThreadGroupCount, ThreadGroupCount);
						sprintf_s(ClearThreadGroupCountXString, "%d", ThreadGroupCount);
						sprintf_s(ClearThreadGroupCountYString, "%d", ThreadGroupCount);
						sprintf_s(ClearChannelCount, "%d", CLEAR_TEXTURE_CHANNELS[FormatIndex]);

						_ClearPipelines[ResourceDimensionIndex][FormatIndex][ThreadGroupMultiple] = CreatePipeline(
							InContext,
							ComputePipelineCreateInformation(
								_ClearRootSignature,
								InContext.GetShader(
									ShaderCreateInformation(
										ShaderType::SHADER_TYPE_COMPUTE,
										ClearShaderName,
										"Utility/clear.compute.hlsl",
										{
											"CLEAR_TEXTURE_TYPE",			ResourceDimensionString[ResourceDimensionIndex],
											"CLEAR_SPIRV_FORMAT",			CLEAR_TEXTURE_FORMAT_SPIRVS[FormatIndex],
											"CLEAR_TEXTURE_CHANNELS_COUNT",	ClearChannelCount,
											"THREAD_GROUP_COUNT_X",			ClearThreadGroupCountXString,
											"THREAD_GROUP_COUNT_Y",			ClearThreadGroupCountYString,
											"THREAD_GROUP_COUNT_Z",			"1",
										}
									)
								)
							)
						);
					}
				}
			}
			
			_ClearDescriptorTables.resize(ClearPerFrameCount);
			for (uint32_t DescriptorTableIndex = 0; DescriptorTableIndex < _ClearDescriptorTables.size(); ++DescriptorTableIndex)
				_ClearDescriptorTables[DescriptorTableIndex] = _ClearRootSignature->CreateRootDescriptorTable(InContext);
			
			_ClearConstantBuffer = CreateMultiBufferedBuffer(
				InContext,
				BufferResourceCreateInformation(
					InContext.GetDevice(),
					"ClearConstantBuffer",
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
						sizeof(ClearConstants),
						128
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				)
			);
		}

		Clear::~Clear()
		{
			DestroyMultiBufferedResource(_ClearConstantBuffer);

			for (uint32_t DescriptorTableIndex = 0; DescriptorTableIndex < _ClearDescriptorTables.size(); ++DescriptorTableIndex)
				DestroyDescriptorTable(_ClearDescriptorTables[DescriptorTableIndex]);

			for (uint32_t ResourceDimensionIndex = 0; ResourceDimensionIndex < static_cast<uint32_t>(ResourceDimension::RESOURCE_DIMENSION_COUNT); ++ResourceDimensionIndex)
			{
				for (uint32_t FormatIndex = 0; FormatIndex < static_cast<uint32_t>(ClearTextureFormat::CLEAR_TEXTURE_FORMAT_COUNT); ++FormatIndex)
				{
					for (uint32_t ThreadGroupMultiple = 0; ThreadGroupMultiple < static_cast<uint32_t>(ClearThreadGroupCount::CLEAR_THREAD_GROUP_COUNT_MAX); ++ThreadGroupMultiple)
						DestroyPipeline(_ClearPipelines[ResourceDimensionIndex][FormatIndex][ThreadGroupMultiple]);
				}
			}
			DestroyRootSignature(_ClearRootSignature);
		}

		void Clear::BeginRender()
		{
			MapRange ClearMapRange((*_ClearConstantBuffer)->GetBufferSize());
			_ClearConstantsPointer = (*_ClearConstantBuffer)->Map<ClearConstants>(ClearMapRange);
			_ClearConstantsIndex = 0u;
		}

		void Clear::EndRender()
		{
			ETERNAL_ASSERT(_ClearConstantsIndex < (*_ClearConstantBuffer)->GetElementCount());
			MapRange ClearMapRange((*_ClearConstantBuffer)->GetBufferSize());
			(*_ClearConstantBuffer)->Unmap(ClearMapRange);
			_ClearConstantsIndex = ClearInvalidIndex;
		}

		void Clear::ClearView(_In_ GraphicsContext& InContext, _In_ View* InView, const Types::Float4& InClearValue)
		{
			CommandListScope ClearCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "Clear");
			
			uint32_t CurrentClearConstantsIndex = _ClearConstantsIndex++;

			const Resource& InResource = InView->GetResource();

			{
				ClearConstants& CurrentClearConstants = _ClearConstantsPointer[CurrentClearConstantsIndex];
				CurrentClearConstants.ClearValue	= InClearValue;
				CurrentClearConstants.ClearSize		= InView->GetResourceType() == ResourceType::RESOURCE_TYPE_TEXTURE
					? Uint3(
						InResource.GetWidth(),
						InResource.GetHeight(),
						InResource.GetDepthOrArraySize()
					)
					: Uint3(
						InResource.GetElementCount(),
						0u, 0u
					);
			}

			ViewMetaData ClearConstantsMetaData;
			ClearConstantsMetaData.ConstantBufferView.BufferElementOffset	= CurrentClearConstantsIndex;
			ClearConstantsMetaData.ConstantBufferView.BufferSize			= (*_ClearConstantBuffer)->GetBufferStride();

			View* CurrentClearConstantBufferView = CreateConstantBufferView(
				ConstantBufferViewCreateInformation(
					InContext,
					&(*_ClearConstantBuffer),
					ClearConstantsMetaData
				),
				alloca(GetViewSize(InContext))
			);

			DescriptorTable* CurrentDescriptorTable = _ClearDescriptorTables[CurrentClearConstantsIndex];
			
			CurrentDescriptorTable->SetDescriptor(0, CurrentClearConstantBufferView);
			CurrentDescriptorTable->SetDescriptor(1, InView);
			
			Uint3 ThreadGroupCount(1u);
			uint32_t ThreadCountExponent = 1u;
			if (InView->GetResourceType() == ResourceType::RESOURCE_TYPE_TEXTURE)
			{
				Vector2 ResourceSizeLog2(
					Math::Ceil(Math::Log2(InResource.GetWidth())),
					Math::Ceil(Math::Log2(InResource.GetHeight()))
				);

				ThreadCountExponent = Math::Min(static_cast<uint32_t>(ResourceSizeLog2.x), static_cast<uint32_t>(ResourceSizeLog2.y));
				ThreadCountExponent = Math::Min(ThreadCountExponent, static_cast<uint32_t>(ClearThreadGroupCount::CLEAR_THREAD_GROUP_COUNT_MAX) - 1u);

				ThreadGroupCount = Uint3(
					InResource.GetWidth(),
					InResource.GetHeight(),
					InResource.GetDepthOrArraySize()
				);
			}
			else
			{
				ETERNAL_BREAK(); // Not implemented
			}

			{
				ResourceTransition ViewToUnorderedAccess(
					InView,
					TransitionState::TRANSITION_SHADER_WRITE
				);
				ResourceTransitionScope ClearTransitionScope(*ClearCommandList, &ViewToUnorderedAccess, 1);
				
				ClearCommandList->SetComputePipeline(*_ClearPipelines[static_cast<uint32_t>(InResource.GetResourceDimension())][ClearPrivate::ConvertFormatToClearTextureFormatInt(InResource.GetFormat())][ThreadCountExponent]);
				ClearCommandList->SetComputeDescriptorTable(InContext, *CurrentDescriptorTable);
				ClearCommandList->Dispatch(
					Math::DivideRoundUp(ThreadGroupCount.x, 1u << ThreadCountExponent),
					Math::DivideRoundUp(ThreadGroupCount.y, 1u << ThreadCountExponent),
					ThreadGroupCount.z
				);
			}

			CurrentClearConstantBufferView->~View();
		}
	}
}
