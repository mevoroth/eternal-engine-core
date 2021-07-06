#include "Imgui/Imgui.hpp"

#include "Window/Window.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "Input/Input.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	namespace Imgui
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Types;

		struct ImguiProjectionConstants
		{
			Matrix4x4 ProjectionMatrix;
		};

		struct ImguiVertex
		{
			Vector4 Color;
			Vector2 Position;
			Vector2 UV;
		};

		struct ImguiRenderContext
		{
			ImguiProjectionConstants*	ImguiConstantsPointer	= nullptr;
			ImguiVertex*				ImguiVerticesPointer	= nullptr;
			uint16_t*					ImguiIndicesPointer		= nullptr;
			uint32_t					ImguiProjectionCount	= 0;
			uint32_t					ImguiVerticesCount		= 0;
			uint32_t					ImguiIndicesCount		= 0;
		};

		Imgui::Imgui(_In_ const Window& InWindow, _In_ GraphicsContext& InContext)
		{
			using ImguiVertexStream = VertexStream<ImguiVertex>;

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& IO = ImGui::GetIO();
			IO.BackendPlatformName	= "PC";
			IO.BackendRendererName	= "EternalGraphics";
			IO.DisplaySize.x		= static_cast<float>(InWindow.GetWidth());
			IO.DisplaySize.y		= static_cast<float>(InWindow.GetHeight());
			IO.IniFilename			= "imgui.ini";

			IO.BackendFlags			|= ImGuiBackendFlags_HasGamepad
									| ImGuiBackendFlags_HasMouseCursors
									//| ImGuiBackendFlags_HasSetMousePos
									| ImGuiBackendFlags_RendererHasVtxOffset;

			IO.ConfigFlags			|= ImGuiConfigFlags_NavEnableKeyboard
									| ImGuiConfigFlags_NavEnableGamepad;

			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			//ImGui::StyleColorsLight();

			//unsigned char* Pixels;
			//int Width, Height;

			//io.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

			_Map(Input::Input::TAB,			ImGuiKey_Tab);
			_Map(Input::Input::LEFT,		ImGuiKey_LeftArrow);
			_Map(Input::Input::RIGHT,		ImGuiKey_RightArrow);
			_Map(Input::Input::UP,			ImGuiKey_UpArrow);
			_Map(Input::Input::DOWN,		ImGuiKey_DownArrow);
			_Map(Input::Input::PGUP,		ImGuiKey_PageUp);
			_Map(Input::Input::PGDOWN,		ImGuiKey_PageDown);
			_Map(Input::Input::HOME,		ImGuiKey_Home);
			_Map(Input::Input::END,			ImGuiKey_End);
			_Map(Input::Input::DEL,			ImGuiKey_Delete);
			_Map(Input::Input::BACKSPACE,	ImGuiKey_Backspace);
			_Map(Input::Input::RETURN,		ImGuiKey_Enter);
			_Map(Input::Input::ESC,			ImGuiKey_Escape);
			_Map(Input::Input::A,			ImGuiKey_A);
			_Map(Input::Input::C,			ImGuiKey_C);
			_Map(Input::Input::V,			ImGuiKey_V);
			_Map(Input::Input::X,			ImGuiKey_X);
			_Map(Input::Input::Y,			ImGuiKey_Y);
			_Map(Input::Input::Z,			ImGuiKey_Z);

			IO.ImeWindowHandle = InWindow.GetWindowHandler();

			//////////////////////////////////////////////////////////////////////////
			// Graphics resources
			ShaderCreateInformation ImguiVSCreateInformation(
				ShaderType::VS,
				"ImguiVS",
				"imgui.vs.hlsl"
			);
			_ImguiVS = InContext.GetShader(ImguiVSCreateInformation);
			ShaderCreateInformation ImguiPSCreateInformation(
				ShaderType::PS,
				"ImguiPS",
				"imgui.ps.hlsl"
			);
			_ImguiPS = InContext.GetShader(ImguiPSCreateInformation);
			
			_ImguiBlendState = new BlendState(
				Blend::BLEND_SOURCE_ALPHA,
				Blend::BLEND_INVERSE_SOURCE_ALPHA,
				BlendOperator::BLEND_OPERATOR_ADD,
				Blend::BLEND_ONE,
				Blend::BLEND_INVERSE_SOURCE_ALPHA,
				BlendOperator::BLEND_OPERATOR_ADD
			);

			vector<View*>& BackBuffersViews = InContext.GetSwapChain().GetBackBufferRenderTargetViews();
			for (uint32_t RenderPassIndex = 0; RenderPassIndex < ETERNAL_ARRAYSIZE(_ImguiRenderPasses); ++RenderPassIndex)
			{
				RenderPassCreateInformation ImguiRenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(*_ImguiBlendState, RenderTargetOperator::Load_Store, BackBuffersViews[RenderPassIndex])
					}
				);
				_ImguiRenderPasses[RenderPassIndex] = CreateRenderPass(InContext, ImguiRenderPassCreateInformation);
			}

			vector<VertexStreamBase> ImguiVertexStreams =
			{
				ImguiVertexStream({
					{ Format::FORMAT_RG3232,	VertexDataType::VERTEX_DATA_TYPE_POSITION },
					{ Format::FORMAT_RG3232,	VertexDataType::VERTEX_DATA_TYPE_UV },
					{ Format::FORMAT_RGBA8888,	VertexDataType::VERTEX_DATA_TYPE_COLOR }
				})
			};
			_ImguiInputLayout = CreateInputLayout(InContext, ImguiVertexStreams);

			RootSignatureCreateInformation ImguiRootSignatureInformation(
				{
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				}
			);
			_ImguiRootSignature		= CreateRootSignature(InContext, ImguiRootSignatureInformation);
			_ImguiDescriptorTable	= _ImguiRootSignature->CreateRootDescriptorTable(InContext);

			PipelineCreateInformation ImguiPipelineCreateInformation(
				*_ImguiRootSignature,
				*_ImguiInputLayout,
				*_ImguiRenderPasses[0],
				*_ImguiVS,
				*_ImguiPS
			);
			_ImguiPipeline			= CreatePipeline(InContext, ImguiPipelineCreateInformation);

			//////////////////////////////////////////////////////////////////////////
			// Buffers

			BufferResourceCreateInformation ImguiConstantBufferResourceCreateInformation(
				InContext.GetDevice(),
				"ImguiProjectionConstantBuffer",
				BufferCreateInformation(
					Format::FORMAT_UNKNOWN,
					BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
					sizeof(ImguiProjectionConstants) * ImguiMaxProjections
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
			);
			_ImguiConstantBuffer		= CreateMultiBufferedBuffer(ImguiConstantBufferResourceCreateInformation);

			ViewMetaData ImguiConstantBufferViewMetaData;
			ImguiConstantBufferViewMetaData.ConstantBufferView.BufferSize = sizeof(ImguiProjectionConstants);
			ConstantBufferViewCreateInformation ImguiConstantBufferViewCreateInformation(
				InContext,
				_ImguiConstantBuffer,
				ImguiConstantBufferViewMetaData
			);
			_ImguiConstantBufferView	= CreateMultiBufferedConstantBufferView(_ImguiConstantBuffer, ImguiConstantBufferViewCreateInformation);

			BufferResourceCreateInformation ImguiVertexBufferResourceCreateInformation(
				InContext.GetDevice(),
				"ImguiVertexBuffer",
				BufferCreateInformation(
					Format::FORMAT_UNKNOWN,
					BufferResourceUsage::BUFFER_RESOURCE_USAGE_VERTEX_BUFFER,
					sizeof(ImguiVertex) * ImguiMaxVertices
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
			);
			_ImguiVertexBuffer			= CreateMultiBufferedBuffer(ImguiVertexBufferResourceCreateInformation);

			BufferResourceCreateInformation ImguiIndexBufferResourceCreateInformation(
				InContext.GetDevice(),
				"ImguiIndexBuffer",
				BufferCreateInformation(
					Format::FORMAT_UNKNOWN,
					BufferResourceUsage::BUFFER_RESOURCE_USAGE_INDEX_BUFFER,
					sizeof(uint16_t) * ImguiMaxIndices
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
			);
			_ImguiIndexBuffer			= CreateMultiBufferedBuffer(ImguiIndexBufferResourceCreateInformation);
		}

		void Imgui::Begin()
		{
			_UpdateInputs();

			ImGui::NewFrame();
		}

		void Imgui::End()
		{
			ImGui::Render();
		}

		void Imgui::Render(_In_ GraphicsContext& InContext)
		{
			ImguiRenderContext RenderContext;

			MapRange ProjectionMapRange(sizeof(ImguiProjectionConstants) * ImguiMaxProjections);
			MapRange VerticesMapRange(sizeof(ImguiVertex) * ImguiMaxVertices);
			MapRange IndicesMapRange(sizeof(uint16_t) * ImguiMaxIndices);

			RenderContext.ImguiConstantsPointer	= static_cast<ImguiProjectionConstants*>(_ImguiConstantBuffer->Map(ProjectionMapRange));
			RenderContext.ImguiVerticesPointer	= static_cast<ImguiVertex*>(_ImguiVertexBuffer->Map(VerticesMapRange));
			RenderContext.ImguiIndicesPointer	= static_cast<uint16_t*>(_ImguiIndexBuffer->Map(IndicesMapRange));

			CommandList* ImguiCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC);

			ImguiCommandList->Begin(InContext);
			ImguiCommandList->BeginRenderPass(*_ImguiRenderPasses[InContext.GetCurrentFrameIndex()]);
			ImguiCommandList->SetGraphicsPipeline(*_ImguiPipeline);

			ImguiCommandList->EndRenderPass();
			ImguiCommandList->End();
		}

		void Imgui::_Map(_In_ const Input::Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey)
		{
			ImGui::GetIO().KeyMap[ImguiKey] = EternalKey;
			_MappedKeys.push_back(EternalKey);
		}

		void Imgui::_ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Input::Key& KeyName)
		{
			ImGuiIO& IO = ImGui::GetIO();
			if (Input::Input::Get()->IsPressed(KeyName))
				IO.AddInputCharacter(ImguiKey);
		}

		void Imgui::_ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Input::Key& KeyNameStart, _In_ uint32_t Range)
		{
			ImGuiIO& IO = ImGui::GetIO();
			for (uint32_t KeyIndex = 0; KeyIndex < Range; ++KeyIndex)
			{
				if (_KeyboardInput->IsPressed((Input::Input::Key)(KeyNameStart + KeyIndex)))
					IO.AddInputCharacter(ImguiKeyStart + (ImWchar)KeyIndex);
			}
		}

		void Imgui::_UpdateInputs()
		{
			ImGuiIO& IO = ImGui::GetIO();
			for (int MappedKeyIndex = 0; MappedKeyIndex < _MappedKeys.size(); ++MappedKeyIndex)
				IO.KeysDown[_MappedKeys[MappedKeyIndex]] = _KeyboardInput->IsDown(_MappedKeys[MappedKeyIndex]);

			IO.MouseDown[0]	= _KeyboardInput->IsPressed(Input::Input::MOUSE0); // L-Click
			IO.MouseDown[1]	= _KeyboardInput->IsPressed(Input::Input::MOUSE1); // R-Click
			IO.MouseDown[2]	= _KeyboardInput->IsPressed(Input::Input::MOUSE2); // M-Click
			IO.MousePos.x	= _KeyboardInput->GetAxis(Input::Input::MOUSE_X);
			IO.MousePos.y	= _KeyboardInput->GetAxis(Input::Input::MOUSE_Y);

			_ProcessInputCharacterRange((ImWchar)'a',	Input::Input::A,		26);
			_ProcessInputCharacterRange((ImWchar)'0',	Input::Input::KP0,		10);
			_ProcessInputCharacter('.',					Input::Input::KPPERIOD);
			_ProcessInputCharacter('+',					Input::Input::KPPLUS);
			_ProcessInputCharacter('-',					Input::Input::KPMINUS);
			_ProcessInputCharacter('*',					Input::Input::KPMUL);
			_ProcessInputCharacter('/',					Input::Input::KPDIV);
		}

		void Imgui::_ImGui_SetupRenderState(_In_ ImDrawData* DrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* ImguiCommandList)
		{
			Viewport* ImguiViewport = CreateInvertedViewport(
				InContext,
				static_cast<int32_t>(DrawData->DisplaySize.x),
				static_cast<int32_t>(DrawData->DisplaySize.y)
			);
			ImguiCommandList->SetViewport(*ImguiViewport);
			delete ImguiViewport;
			ImguiViewport = nullptr;

			float Left		= DrawData->DisplayPos.x;
			float Right		= DrawData->DisplayPos.x + DrawData->DisplaySize.x;
			float Top		= DrawData->DisplayPos.y;
			float Bottom	= DrawData->DisplayPos.y + DrawData->DisplaySize.y;

			ImguiProjectionConstants& Constants = InImguiContext.ImguiConstantsPointer[InImguiContext.ImguiProjectionCount++];
			Constants.ProjectionMatrix = Matrix4x4(
				2.0f / (Right - Left),				0.0f,								0.0f, 0.0f,
				0.0f,								2.0f / (Top - Bottom),				0.0f, 0.0f,
				0.0f,								0.0f,								0.5f, 0.0f,
				(Right + Left) / (Left - Right),	(Top + Bottom) / (Bottom - Top),	0.5f, 1.0f
			);

			//// Bind shader and vertex buffers
			//unsigned int stride = sizeof(ImDrawVert);
			//unsigned int offset = 0;
			//D3D12_VERTEX_BUFFER_VIEW vbv;
			//memset(&vbv, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
			//vbv.BufferLocation = fr->VertexBuffer->GetGPUVirtualAddress() + offset;
			//vbv.SizeInBytes = fr->VertexBufferSize * stride;
			//vbv.StrideInBytes = stride;
			//ctx->IASetVertexBuffers(0, 1, &vbv);
			//D3D12_INDEX_BUFFER_VIEW ibv;
			//memset(&ibv, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
			//ibv.BufferLocation = fr->IndexBuffer->GetGPUVirtualAddress();
			//ibv.SizeInBytes = fr->IndexBufferSize * sizeof(ImDrawIdx);
			//ibv.Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			//ctx->IASetIndexBuffer(&ibv);
			
			//ctx->SetGraphicsRoot32BitConstants(0, 16, &vertex_constant_buffer, 0);

			//// Setup blend factor
			//const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
			//ctx->OMSetBlendFactor(blend_factor);
		}
	}
}
