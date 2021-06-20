#include "Task/NextGenGraphics/DebugRenderTask.hpp"

#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/Viewports.hpp"
#include "GraphicData/Shaders.hpp"
#include "Core/StateSharedData.hpp"
#include "NextGenGraphics/Context.hpp"
#include "NextGenGraphics/Resource.hpp"
#include "Graphics_deprecated/PipelineFactory.hpp"
#include "Graphics_deprecated/Pipeline.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/DepthTest.hpp"
#include "Graphics_deprecated/InputLayoutFactory.hpp"
#include "Graphics_deprecated/InputLayout.hpp"
#include "Graphics_deprecated/RootSignature.hpp"
#include "Graphics_deprecated/RootSignatureFactory.hpp"
#include "Graphics_deprecated/SwapChain.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics_deprecated/DescriptorHeapFactory.hpp"
#include "Graphics_deprecated/DescriptorHeap.hpp"
#include "Graphics_deprecated/Heap.hpp"
#include "Graphics_deprecated/HeapFactory.hpp"
#include "Graphics_deprecated/ResourceFactory.hpp"
#include "Graphics_deprecated/View.hpp"
#include "Graphics_deprecated/DescriptorTable.h"
#include "NextGenGraphics/SamplerFactory.hpp"
#include "Task/Core/Commands/UploadTextureDataCommand.hpp"

#include "Import/tga/ImportTga.hpp"
#include "File/FilePath.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/UploadBuffers.hpp"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class DebugRenderTaskData
		{
		public:
			DebugRenderTaskData(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
			{
				//_TextureHeap	= CreateHeap(DeviceObj, HEAP_TYPE_TEXTURE, 1, true, false, false, false);
				//_Texture		= CreateResource(DeviceObj, *_TextureHeap, L"Test Texture", RESOURCE_DIMENSION_TEXTURE_2D, FORMAT_BGRA8888, (TextureType)(TEXTURE_READ | TEXTURE_COPY_WRITE), 1024, 1024, 1, 1, TRANSITION_UNDEFINED);

				//uint32_t Height, Width;
				//uint8_t* TextureData = Eternal::Import::ImportTga::Get()->Import(Eternal::File::FilePath::Find("fzero.tga", Eternal::File::FileType::TEXTURES), Height, Width);
				//UploadTextureDataCommand* UploadCommand = new UploadTextureDataCommand;
				//UploadCommand->DestinationTexture	= _Texture;
				//UploadCommand->TextureData			= TextureData;
				//UploadCommand->Width				= Width;
				//UploadCommand->Height				= Height;
				//SharedData->EngineCommands[SharedData->CurrentFrame][0].push_back(UploadCommand);

				//vector<RootSignatureParameter> TextureParameters[2];
				//RootSignatureParameter TexParam = { RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_IA_VS_PS, 0 };
				//RootSignatureParameter SamParam = { RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_IA_VS_PS, 0 };
				//TextureParameters[0].push_back(TexParam);
				//TextureParameters[1].push_back(SamParam);

				//_TextureDescriptorHeap = CreateDescriptorHeap(DeviceObj, TextureParameters[0].data(), uint32_t(TextureParameters[0].size()));
				//TexView = _Texture->CreateView(DeviceObj, *_TextureDescriptorHeap, TEXTURE_VIEW_TYPE_2D, FORMAT_BGRA8888);
				//TexDescriptorTable = _TextureDescriptorHeap->CreateView(DeviceObj);
				//TexDescriptorTable->SetResource(0, TexView);
				//TexDescriptorTable->Update(DeviceObj);

				//_SamplerDescriptorHeap = CreateDescriptorHeap(DeviceObj, TextureParameters[1].data(), uint32_t(TextureParameters[1].size()));
				//_Sampler = CreateSampler(DeviceObj, *_SamplerDescriptorHeap, true, true, true, false, ADDRESS_MODE_WRAP, ADDRESS_MODE_WRAP, ADDRESS_MODE_WRAP);
				//SamDescriptorTable = _SamplerDescriptorHeap->CreateView(DeviceObj);
				//SamDescriptorTable->SetResource(0, _Sampler);
				//SamDescriptorTable->Update(DeviceObj);

				//_Viewport		= &SwapChainObj.GetMainViewport();
				//_VS				= Resources->GetShaders()->Get(POST_PROCESS_VS);
				//_PS				= Resources->GetShaders()->Get(DEBUG_RENDER_PS);
				//_InputLayout	= CreateInputLayout(DeviceObj, nullptr, 0);
				//_RootSignature	= CreateRootSignature(DeviceObj, TextureParameters, ETERNAL_ARRAYSIZE(TextureParameters), RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS);
				////_RootSignature	= CreateRootSignature(DeviceObj);
				//_Pipeline		= CreatePipeline(DeviceObj, *_RootSignature, *_InputLayout, SwapChainObj.GetMainRenderPass(), *GetVS(), *GetPS(), GetDepthTest(), GetStencilTest(), SwapChainObj.GetMainViewport());
				//
				////CreateRenderPass(DeviceObj, *_Viewport, )
			}
			
			const StencilTest&	GetStencilTest()	const	{ return _StencilTest; }
			const DepthTest&	GetDepthTest()		const	{ return _DepthTest; }
			const Viewport*		GetViewport()		const	{ return _Viewport; }
			Shader*				GetVS()				const	{ return _VS; }
			Shader*				GetPS()				const	{ return _PS; }
			Pipeline*			GetPipeline()				{ return _Pipeline; }
			RootSignature*		GetRootSignature()			{ return _RootSignature; }

			DescriptorHeap*		GetTextureDescriptorHeap() { return _TextureDescriptorHeap; }
			DescriptorHeap*		GetSamplerDescriptorHeap() { return _SamplerDescriptorHeap; }
			Resource*			GetTexture() { return _Texture; }
			Resource*			Upload = nullptr;
			View*				TexView = nullptr;
			DescriptorTable*	TexDescriptorTable = nullptr;
			DescriptorTable*	SamDescriptorTable = nullptr;

			Resource*			IntermediateTarget = nullptr;
			View*				IntermediateTargetView = nullptr;
			RenderPass*			IntermediateRenderPass = nullptr;

		private:
			DepthTest		_DepthTest;
			StencilTest		_StencilTest;
			const Viewport*	_Viewport				= nullptr;
			Pipeline*		_Pipeline				= nullptr;
			RootSignature*	_RootSignature			= nullptr;
			Shader*			_VS						= nullptr;
			Shader*			_PS						= nullptr;
			InputLayout*	_InputLayout			= nullptr;
			RenderPass*		_RenderPass				= nullptr;

			DescriptorHeap*	_TextureDescriptorHeap	= nullptr;
			Heap*			_TextureHeap			= nullptr;
			Resource*		_Texture				= nullptr;

			DescriptorHeap*	_SamplerDescriptorHeap	= nullptr;
			Sampler*		_Sampler				= nullptr;
		};
	}
}

DebugRenderTask::DebugRenderTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: GraphicTask(DeviceObj, Resources, SharedData)
	, _DebugRenderTaskData(new DebugRenderTaskData(DeviceObj, SwapChainObj, Resources, SharedData))
{
}

void DebugRenderTask::DoSetup()
{
}

void DebugRenderTask::DoReset()
{
}

bool DebugRenderTask::IsRendered() const
{
	static bool test = false;
	if (!test)
	{
		test = true;
		return false;
	}
	return true;
}

void DebugRenderTask::Render(_Inout_ CommandList* CommandListObj)
{
	//Resource* CurrentBackBuffer = nullptr;// GetSharedData()->GfxContexts[GetSharedData()->CurrentFrame]->GetFrameBackBuffer();
	//ResourceTransition TransitionBefore(CurrentBackBuffer, TRANSITION_PRESENT, TRANSITION_RENDER_TARGET_WRITE);
	//ResourceTransition TransitionAfter(CurrentBackBuffer, TRANSITION_RENDER_TARGET_WRITE, TRANSITION_PRESENT);

	//DescriptorHeap* TestTex = _DebugRenderTaskData->GetTextureDescriptorHeap();

	//DescriptorHeap* DescriptorHeaps[] =
	//{
	//	_DebugRenderTaskData->GetTextureDescriptorHeap(),
	//	_DebugRenderTaskData->GetSamplerDescriptorHeap()
	//};

	//CommandListObj->BindPipelineInput(*_DebugRenderTaskData->GetRootSignature(), DescriptorHeaps, ETERNAL_ARRAYSIZE(DescriptorHeaps));
	//CommandListObj->BindDescriptorTable(0, *_DebugRenderTaskData->TexDescriptorTable);
	//CommandListObj->BindDescriptorTable(1, *_DebugRenderTaskData->SamDescriptorTable);
	////CommandListObj->Transition(nullptr, 0u, &TransitionBefore, 1);
	//CommandListObj->DrawPrimitive(6);
	//CommandListObj->Transition(nullptr, 0u, &TransitionAfter, 1);
}

const Viewport* DebugRenderTask::GetViewport()
{
	return _DebugRenderTaskData->GetViewport();
}

Pipeline* DebugRenderTask::GetPipeline()
{
	return _DebugRenderTaskData->GetPipeline();
}

RenderPass* DebugRenderTask::GetRenderPass()
{
	return nullptr;// GetSharedData()->GfxContexts[GetSharedData()->CurrentFrame]->GetFrameRenderPass();
}
