#include "Task/NextGenGraphics/DebugRenderTask.hpp"

#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/Viewports.hpp"
#include "GraphicData/Shaders.hpp"
#include "Core/StateSharedData.hpp"
#include "NextGenGraphics/Context.hpp"
#include "NextGenGraphics/Resource.hpp"
#include "Graphics/PipelineFactory.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/DepthTest.hpp"
#include "Graphics/InputLayoutFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/RootSignature.hpp"
#include "Graphics/RootSignatureFactory.hpp"
#include "Graphics/SwapChain.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/DescriptorHeapFactory.hpp"
#include "Graphics/DescriptorHeap.hpp"
#include "Graphics/Heap.hpp"
#include "Graphics/HeapFactory.hpp"
#include "Graphics/ResourceFactory.hpp"
#include "Graphics/View.hpp"
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
				_TextureHeap	= CreateHeap(DeviceObj, HEAP_TYPE_TEXTURE, 1, true, false, false, false);
				_Texture		= CreateResource(DeviceObj, *_TextureHeap, L"Test Texture", RESOURCE_DIMENSION_TEXTURE_2D, FORMAT_BGRA8888, (TextureType)(TEXTURE_READ | TEXTURE_COPY_WRITE), 1024, 1024, 1, 1, TRANSITION_COPY_WRITE);

				uint32_t Height, Width;
				uint8_t* TextureData = Eternal::Import::ImportTga::Get()->Import(Eternal::File::FilePath::Find("fzero.tga", Eternal::File::FilePath::TEXTURES), Height, Width);
				UploadTextureDataCommand* UploadCommand = new UploadTextureDataCommand;
				UploadCommand->DestinationTexture	= _Texture;
				UploadCommand->TextureData			= TextureData;
				UploadCommand->Width				= Width;
				UploadCommand->Height				= Height;
				SharedData->EngineCommands[SharedData->CurrentFrame][0].push_back(UploadCommand);

				vector<RootSignatureParameter> TextureParameters[2];
				RootSignatureParameter TexParam = { ROOT_SIGNATURE_PARAMETER_TEXTURE, ROOT_SIGNATURE_ACCESS_IA_VS_PS, 0 };
				RootSignatureParameter SamParam = { ROOT_SIGNATURE_PARAMETER_SAMPLER, ROOT_SIGNATURE_ACCESS_IA_VS_PS, 0 };
				TextureParameters[0].push_back(TexParam);
				TextureParameters[1].push_back(SamParam);

				_TextureDescriptorHeap = CreateDescriptorHeap(DeviceObj, TextureParameters[0].data(), TextureParameters[0].size());
				TexView = _Texture->CreateView(DeviceObj, *_TextureDescriptorHeap, TEXTURE_VIEW_TYPE_2D, FORMAT_BGRA8888);
				TexDescriptorTable = _TextureDescriptorHeap->CreateView(DeviceObj);

				_Viewport		= &SwapChainObj.GetMainViewport();
				_VS				= Resources->GetShaders()->Get(POST_PROCESS_VS);
				_PS				= Resources->GetShaders()->Get(DEBUG_RENDER_PS);
				_InputLayout	= CreateInputLayout(DeviceObj, nullptr, 0);
				_RootSignature	= CreateRootSignature(DeviceObj, TextureParameters, ETERNAL_ARRAYSIZE(TextureParameters), ROOT_SIGNATURE_ACCESS_PS);
				//_RootSignature	= CreateRootSignature(DeviceObj);
				_Pipeline		= CreatePipeline(DeviceObj, *_RootSignature, *_InputLayout, SwapChainObj.GetMainRenderPass(), *GetVS(), *GetPS(), GetDepthTest(), GetStencilTest(), SwapChainObj.GetMainViewport());
			}
			
			const StencilTest&	GetStencilTest()	const	{ return _StencilTest; }
			const DepthTest&	GetDepthTest()		const	{ return _DepthTest; }
			const Viewport*		GetViewport()		const	{ return _Viewport; }
			Shader*				GetVS()				const	{ return _VS; }
			Shader*				GetPS()				const	{ return _PS; }
			Pipeline*			GetPipeline()				{ return _Pipeline; }
			RootSignature*		GetRootSignature()			{ return _RootSignature; }

			DescriptorHeap*		GetTextureDescriptorHeap() { return _TextureDescriptorHeap; }
			Resource*			GetTexture() { return _Texture; }
			Resource*			Upload = nullptr;
			View*				TexView = nullptr;
			View*				TexDescriptorTable = nullptr;
		private:
			DepthTest		_DepthTest;
			StencilTest		_StencilTest;
			const Viewport*	_Viewport				= nullptr;
			Pipeline*		_Pipeline				= nullptr;
			RootSignature*	_RootSignature			= nullptr;
			Shader*			_VS						= nullptr;
			Shader*			_PS						= nullptr;
			InputLayout*	_InputLayout			= nullptr;

			DescriptorHeap*	_TextureDescriptorHeap	= nullptr;
			Heap*			_TextureHeap			= nullptr;
			Resource*		_Texture				= nullptr;
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
	return true;
}

void DebugRenderTask::Render(_Inout_ CommandList* CommandListObj)
{
	Resource* CurrentBackBuffer = GetSharedData()->GfxContexts[GetSharedData()->CurrentFrame]->GetFrameBackBuffer();
	ResourceTransition TransitionBefore(CurrentBackBuffer, TRANSITION_PRESENT, TRANSITION_RENDER_TARGET_WRITE);
	ResourceTransition TransitionAfter(CurrentBackBuffer, TRANSITION_RENDER_TARGET_WRITE, TRANSITION_PRESENT);

	DescriptorHeap* TestTex = _DebugRenderTaskData->GetTextureDescriptorHeap();

	CommandListObj->BindPipelineInput(*_DebugRenderTaskData->GetRootSignature(), &TestTex, 1);
	CommandListObj->BindDescriptorTable(0, *_DebugRenderTaskData->TexDescriptorTable);
	CommandListObj->Transition(nullptr, 0u, &TransitionBefore, 1);
	CommandListObj->DrawPrimitive(6);
	CommandListObj->Transition(nullptr, 0u, &TransitionAfter, 1);
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
	return GetSharedData()->GfxContexts[GetSharedData()->CurrentFrame]->GetFrameRenderPass();
}
