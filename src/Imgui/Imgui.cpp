#include "Imgui/Imgui.hpp"

#include "Window/Window.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "Input/Input.hpp"
#include "Types/Types.hpp"
#include "Math/Math.hpp"

namespace Eternal
{
	namespace Imgui
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Types;
		using namespace Eternal::Math;

		struct ImguiProjectionConstants
		{
			Matrix4x4 ProjectionMatrix;
		};

		struct ImguiRenderContext
		{
			void Clear()
			{
				ImguiConstantsPointer	= nullptr;
				ImguiVerticesPointer	= nullptr;
				ImguiIndicesPointer		= nullptr;
			}

			ImDrawVert* GetVerticesPointer() const { return ImguiVerticesPointer + ImguiVerticesSize; }
			ImDrawIdx* GetIndicesPointer() const { return ImguiIndicesPointer + ImguiIndicesSize; }

			ImguiProjectionConstants*	ImguiConstantsPointer	= nullptr;
			ImDrawVert*					ImguiVerticesPointer	= nullptr;
			ImDrawIdx*					ImguiIndicesPointer		= nullptr;
			uint32_t					ImguiProjectionCount	= 0;
			uint32_t					ImguiVerticesSize		= 0;
			uint32_t					ImguiIndicesSize		= 0;
		};

		Imgui::Imgui(_In_ GraphicsContext& InContext, _In_ Input* InInput)
			: _Input(InInput)
		{
			using ImguiVertexStream = VertexStream<ImDrawVert>;

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& IO = ImGui::GetIO();
			IO.BackendPlatformName	= "PC";
			IO.BackendRendererName	= "EternalGraphics";
			IO.DisplaySize.x		= static_cast<float>(InContext.GetWindow().GetWidth());
			IO.DisplaySize.y		= static_cast<float>(InContext.GetWindow().GetHeight());
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

			_Map(Input::TAB,		ImGuiKey_Tab);
			_Map(Input::LEFT,		ImGuiKey_LeftArrow);
			_Map(Input::RIGHT,		ImGuiKey_RightArrow);
			_Map(Input::UP,			ImGuiKey_UpArrow);
			_Map(Input::DOWN,		ImGuiKey_DownArrow);
			_Map(Input::PGUP,		ImGuiKey_PageUp);
			_Map(Input::PGDOWN,		ImGuiKey_PageDown);
			_Map(Input::HOME,		ImGuiKey_Home);
			_Map(Input::END,		ImGuiKey_End);
			_Map(Input::DEL,		ImGuiKey_Delete);
			_Map(Input::BACKSPACE,	ImGuiKey_Backspace);
			_Map(Input::RETURN,		ImGuiKey_Enter);
			_Map(Input::ESC,		ImGuiKey_Escape);
			_Map(Input::SPACE,		ImGuiKey_Space);
			_Map(Input::A,			ImGuiKey_A);
			_Map(Input::C,			ImGuiKey_C);
			_Map(Input::V,			ImGuiKey_V);
			_Map(Input::X,			ImGuiKey_X);
			_Map(Input::Y,			ImGuiKey_Y);
			_Map(Input::Z,			ImGuiKey_Z);

			IO.ImeWindowHandle = InContext.GetWindow().GetWindowHandler();

			//////////////////////////////////////////////////////////////////////////
			// Graphics resources
			ShaderCreateInformation ImguiVSCreateInformation(
				ShaderType::VS,
				"ImguiVS",
				"imgui\\imgui.vs.hlsl"
			);
			_ImguiVS = InContext.GetShader(ImguiVSCreateInformation);
			ShaderCreateInformation ImguiPSCreateInformation(
				ShaderType::PS,
				"ImguiPS",
				"imgui\\imgui.ps.hlsl"
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
					{ Format::FORMAT_RG3232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_POSITION },
					{ Format::FORMAT_RG3232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_UV },
					{ Format::FORMAT_RGBA8888_UNORM,	VertexDataType::VERTEX_DATA_TYPE_COLOR }
				})
			};
			_ImguiInputLayout = CreateInputLayout(InContext, ImguiVertexStreams);

			SamplerCreateInformation BilinearSamplerCreateInformation;
			_ImguiBilinearSampler	= CreateSampler(InContext, BilinearSamplerCreateInformation);

			RootSignatureCreateInformation ImguiRootSignatureInformation(
				{
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
					RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				}
			);
			ImguiRootSignatureInformation.HasInputAssembler = true;
			_ImguiRootSignature		= CreateRootSignature(InContext, ImguiRootSignatureInformation);
			_ImguiDescriptorTable	= _ImguiRootSignature->CreateRootDescriptorTable(InContext);

			PipelineCreateInformation ImguiPipelineCreateInformation(
				*_ImguiRootSignature,
				*_ImguiInputLayout,
				*_ImguiRenderPasses[0],
				*_ImguiVS,
				*_ImguiPS,
				Rasterizer(FrontFace::FRONT_FACE_CLOCKWISE)
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
			_ImguiConstantBuffer		= CreateMultiBufferedBuffer(InContext, ImguiConstantBufferResourceCreateInformation);

			ViewMetaData ImguiConstantBufferViewMetaData;
			ImguiConstantBufferViewMetaData.ConstantBufferView.BufferSize = sizeof(ImguiProjectionConstants);
			ConstantBufferViewCreateInformation ImguiConstantBufferViewCreateInformation(
				InContext,
				*_ImguiConstantBuffer,
				ImguiConstantBufferViewMetaData
			);
			_ImguiConstantBufferView	= CreateMultiBufferedConstantBufferView(*_ImguiConstantBuffer, ImguiConstantBufferViewCreateInformation);

			BufferResourceCreateInformation ImguiVertexBufferResourceCreateInformation(
				InContext.GetDevice(),
				"ImguiVertexBuffer",
				BufferCreateInformation(
					Format::FORMAT_UNKNOWN,
					BufferResourceUsage::BUFFER_RESOURCE_USAGE_VERTEX_BUFFER,
					sizeof(ImDrawVert) * ImguiMaxVertices,
					sizeof(ImDrawVert)
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
			);
			_ImguiVertexBuffer			= CreateMultiBufferedBuffer(InContext, ImguiVertexBufferResourceCreateInformation);

			BufferResourceCreateInformation ImguiIndexBufferResourceCreateInformation(
				InContext.GetDevice(),
				"ImguiIndexBuffer",
				BufferCreateInformation(
					Format::FORMAT_R16_UINT,
					BufferResourceUsage::BUFFER_RESOURCE_USAGE_INDEX_BUFFER,
					sizeof(ImDrawIdx) * ImguiMaxIndices
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
			);
			_ImguiIndexBuffer			= CreateMultiBufferedBuffer(InContext, ImguiIndexBufferResourceCreateInformation);

			IO.Fonts->GetTexDataAsRGBA32(
				&_ImguiFontMetaData.Pixels,
				&_ImguiFontMetaData.Width,
				&_ImguiFontMetaData.Height,
				&_ImguiFontMetaData.BytesPerPixel
			);
			TextureResourceCreateInformation ImguiFontCreateInformation(
				InContext.GetDevice(),
				"ImguiFontTexture",
				TextureCreateInformation(
					ResourceDimension::RESOURCE_DIMENSION_TEXTURE_2D,
					Format::FORMAT_RGBA8888_UNORM,
					TextureResourceUsage::TEXTURE_RESOURCE_USAGE_SHADER_RESOURCE | TextureResourceUsage::TEXTURE_RESOURCE_USAGE_COPY_WRITE,
					_ImguiFontMetaData.Width,
					_ImguiFontMetaData.Height
				),
				ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_MEMORY,
				TransitionState::TRANSITION_COPY_WRITE
			);
			_ImguiFontTexture			= CreateTexture(ImguiFontCreateInformation);
			ViewMetaData ImguiFontViewMetaData;
			ImguiFontViewMetaData.ShaderResourceViewTexture2D.MipLevels = 1;
			ShaderResourceViewCreateInformation ImguiFontTextureViewCreateInformation(
				InContext,
				_ImguiFontTexture,
				ImguiFontViewMetaData,
				Format::FORMAT_RGBA8888_UNORM,
				ViewShaderResourceType::VIEW_SHADER_RESOURCE_TEXTURE_2D
			);
			_ImguiFontTextureView		= CreateShaderResourceView(ImguiFontTextureViewCreateInformation);
			IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(_ImguiFontTextureView));
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
			_UploadFontTexture(InContext);
			_Render(InContext);
		}

		void Imgui::_UploadFontTexture(_In_ GraphicsContext& InContext)
		{
			if (_ImguiFontMetaData.Pixels)
			{
				//////////////////////////////////////////////////////////////////////////
				// Upload buffer
				std::string UploadTextureName = "ImguiFontTextureUploadBuffer";
				const uint32_t UploadBufferSize = _ImguiFontMetaData.Width * _ImguiFontMetaData.Height * _ImguiFontMetaData.BytesPerPixel;
				BufferResourceCreateInformation UploadBufferTextureInformation(
					InContext.GetDevice(),
					UploadTextureName,
					BufferCreateInformation(
						Format::FORMAT_RGBA8888_UNORM,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_COPY_READ,
						UploadBufferSize
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* ImguiFontUploadTexture = CreateBuffer(UploadBufferTextureInformation);

				//////////////////////////////////////////////////////////////////////////
				// Map
				MapRange UploadBufferMapRange(UploadBufferSize);
				void* UploadTextureDataPtr = ImguiFontUploadTexture->Map(UploadBufferMapRange);
				memcpy(UploadTextureDataPtr, _ImguiFontMetaData.Pixels, UploadBufferSize);
				ImguiFontUploadTexture->Unmap(UploadBufferMapRange);

				CommandList* UploadFontCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "CopyImguiFontFromCPUToGPU");

				UploadFontCommandList->Begin(InContext);
				CopyRegion ImguiFontCopyRegion(
					TextureFromBufferRegion(
						Extent3D(_ImguiFontMetaData.Width, _ImguiFontMetaData.Height),
						UploadBufferSize
					)
				);
				UploadFontCommandList->CopyResource(*_ImguiFontTexture, *ImguiFontUploadTexture, ImguiFontCopyRegion);
				ResourceTransition TransitionToShaderRead(_ImguiFontTexture, TransitionState::TRANSITION_PIXEL_SHADER_READ);
				UploadFontCommandList->Transition(&TransitionToShaderRead, 1);
				UploadFontCommandList->End();

				_ImguiFontMetaData.Pixels = nullptr;
			}
		}

		void Imgui::_Render(_In_ GraphicsContext& InContext)
		{
			ImDrawData* ImguiDrawData = ImGui::GetDrawData();
			if (ImguiDrawData->DisplaySize.x <= 0 || ImguiDrawData->DisplaySize.y <= 0)
				return;

			ImguiRenderContext RenderContext;

			MapRange ProjectionMapRange(sizeof(ImguiProjectionConstants) * ImguiMaxProjections);
			MapRange VerticesMapRange(sizeof(ImDrawVert) * ImguiMaxVertices);
			MapRange IndicesMapRange(sizeof(ImDrawIdx) * ImguiMaxIndices);

			RenderContext.ImguiConstantsPointer	= static_cast<ImguiProjectionConstants*>((*_ImguiConstantBuffer)->Map(ProjectionMapRange));
			RenderContext.ImguiVerticesPointer	= static_cast<ImDrawVert*>((*_ImguiVertexBuffer)->Map(VerticesMapRange));
			RenderContext.ImguiIndicesPointer	= static_cast<ImDrawIdx*>((*_ImguiIndexBuffer)->Map(IndicesMapRange));

			_ImGui_FillBuffers(ImguiDrawData, RenderContext);

			CommandList* ImguiCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "RenderImgui");

			ResourceTransition Transitions[] =
			{
				ResourceTransition(*_ImguiIndexBuffer,	TransitionState::TRANSITION_INDEX_READ),
				ResourceTransition(*_ImguiVertexBuffer,	TransitionState::TRANSITION_VERTEX_BUFFER_READ)
			};

			ImguiCommandList->Begin(InContext);
			{
				//ResourceTransitionScope ScopedTransitions(*ImguiCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				ImguiCommandList->BeginRenderPass(*_ImguiRenderPasses[InContext.GetCurrentFrameIndex()]);
				ImguiCommandList->SetGraphicsPipeline(*_ImguiPipeline);

				_ImGui_Render(ImguiDrawData, RenderContext, InContext, ImguiCommandList);

				ImguiCommandList->EndRenderPass();
			}
			ImguiCommandList->End();

			(*_ImguiConstantBuffer)->Unmap(ProjectionMapRange);
			(*_ImguiVertexBuffer)->Unmap(VerticesMapRange);
			(*_ImguiIndexBuffer)->Unmap(IndicesMapRange);

			RenderContext.Clear();
		}

		void Imgui::_Map(_In_ const Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey)
		{
			ImGui::GetIO().KeyMap[ImguiKey] = EternalKey;
			_MappedKeys.push_back(EternalKey);
		}

		void Imgui::_ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Key& KeyName)
		{
			ImGuiIO& IO = ImGui::GetIO();
			if (_Input->IsPressed(KeyName))
				IO.AddInputCharacter(ImguiKey);
		}

		void Imgui::_ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Key& KeyNameStart, _In_ uint32_t Range)
		{
			ImGuiIO& IO = ImGui::GetIO();
			for (uint32_t KeyIndex = 0; KeyIndex < Range; ++KeyIndex)
			{
				if (_Input->IsPressed((Input::Input::Key)(KeyNameStart + KeyIndex)))
					IO.AddInputCharacter(ImguiKeyStart + (ImWchar)KeyIndex);
			}
		}

		void Imgui::_UpdateInputs()
		{
			ImGuiIO& IO = ImGui::GetIO();
			for (int MappedKeyIndex = 0; MappedKeyIndex < _MappedKeys.size(); ++MappedKeyIndex)
				IO.KeysDown[_MappedKeys[MappedKeyIndex]] = _Input->IsDown(_MappedKeys[MappedKeyIndex]);

			IO.MouseDown[0]	= _Input->IsPressed(Input::Input::MOUSE0); // L-Click
			IO.MouseDown[1]	= _Input->IsPressed(Input::Input::MOUSE1); // R-Click
			IO.MouseDown[2]	= _Input->IsPressed(Input::Input::MOUSE2); // M-Click
			IO.MousePos.x	= _Input->GetAxis(Input::Input::MOUSE_X);
			IO.MousePos.y	= _Input->GetAxis(Input::Input::MOUSE_Y);

			_ProcessInputCharacterRange((ImWchar)'a',	Input::Input::A,		26);
			_ProcessInputCharacterRange((ImWchar)'0',	Input::Input::KP0,		10);
			_ProcessInputCharacter('.',					Input::Input::KPPERIOD);
			_ProcessInputCharacter('+',					Input::Input::KPPLUS);
			_ProcessInputCharacter('-',					Input::Input::KPMINUS);
			_ProcessInputCharacter('*',					Input::Input::KPMUL);
			_ProcessInputCharacter('/',					Input::Input::KPDIV);
		}

		void Imgui::_ImGui_FillBuffers(_In_ ImDrawData* DrawData, _In_ ImguiRenderContext& InImguiContext)
		{
			for (int DrawListIndex = 0; DrawListIndex < DrawData->CmdListsCount; ++DrawListIndex)
			{
				const ImDrawList* ImguiDrawList = DrawData->CmdLists[DrawListIndex];
				memcpy(InImguiContext.GetVerticesPointer(), ImguiDrawList->VtxBuffer.Data, ImguiDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(InImguiContext.GetIndicesPointer(), ImguiDrawList->IdxBuffer.Data, ImguiDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));

				InImguiContext.ImguiVerticesSize += ImguiDrawList->VtxBuffer.Size;
				InImguiContext.ImguiIndicesSize += ImguiDrawList->IdxBuffer.Size;
			}
		}

		void Imgui::_ImGui_SetupRenderState(_In_ ImDrawData* InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList)
		{
			Viewport* ImguiViewport = CreateInvertedViewport(
				InContext,
				static_cast<int32_t>(InDrawData->DisplaySize.x),
				static_cast<int32_t>(InDrawData->DisplaySize.y)
			);
			InImguiCommandList->SetViewport(*ImguiViewport);
			delete ImguiViewport;
			ImguiViewport = nullptr;

			float Left		= InDrawData->DisplayPos.x;
			float Right		= InDrawData->DisplayPos.x + InDrawData->DisplaySize.x;
			float Top		= InDrawData->DisplayPos.y;
			float Bottom	= InDrawData->DisplayPos.y + InDrawData->DisplaySize.y;

			ImguiProjectionConstants& Constants = InImguiContext.ImguiConstantsPointer[InImguiContext.ImguiProjectionCount];
			Constants.ProjectionMatrix = Matrix4x4(
				2.0f / (Right - Left),				0.0f,								0.0f, 0.0f,
				0.0f,								2.0f / (Top - Bottom),				0.0f, 0.0f,
				0.0f,								0.0f,								0.5f, 0.0f,
				(Right + Left) / (Left - Right),	(Top + Bottom) / (Bottom - Top),	0.5f, 1.0f
			);

			InImguiCommandList->SetIndexBuffer(*_ImguiIndexBuffer);

			const Resource* CurrentVertexBuffer = &(*_ImguiVertexBuffer);
			InImguiCommandList->SetVertexBuffers(&CurrentVertexBuffer);

			ViewMetaData ProjectionConstantsMetaData;
			ProjectionConstantsMetaData.ConstantBufferView.BufferOffset	= InImguiContext.ImguiProjectionCount * sizeof(ImguiProjectionConstants);
			ProjectionConstantsMetaData.ConstantBufferView.BufferSize	= sizeof(ImguiProjectionConstants);
			ConstantBufferViewCreateInformation ProjectionConstantsBufferViewCreateInformation(
				InContext,
				&(*_ImguiConstantBuffer),
				ProjectionConstantsMetaData
			);
			View* ProjectionConstantsView = CreateConstantBufferView(ProjectionConstantsBufferViewCreateInformation);
			if (_PreviousDrawCount > 0)
				_ImguiDescriptorTable->SetDescriptor(0, ProjectionConstantsView);
			InContext.DelayedDelete(ProjectionConstantsView);
			
			++InImguiContext.ImguiProjectionCount;

			//// Setup blend factor
			//const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
			//ctx->OMSetBlendFactor(blend_factor);
		}

		void Imgui::_ImGui_Render(_In_ ImDrawData* InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList)
		{
			int ViewportWidth = (int)(InDrawData->DisplaySize.x * InDrawData->FramebufferScale.x);
			int ViewportHeight = (int)(InDrawData->DisplaySize.y * InDrawData->FramebufferScale.y);
			if (ViewportWidth <= 0 || ViewportHeight <= 0)
				return;

			if (_PreviousDrawCount > 0)
				_ImguiDescriptorTable->SetDescriptor(2, _ImguiBilinearSampler);

			_ImGui_SetupRenderState(InDrawData, InImguiContext, InContext, InImguiCommandList);

			_PreviousDrawCount = 0;

			int GlobalVertexOffset = 0;
			int GlobalIndexOffset = 0;
			ImVec2 ClipOffset = InDrawData->DisplayPos;
			ImVec2 ClipScale = InDrawData->FramebufferScale;

			for (int DrawListIndex = 0; DrawListIndex < InDrawData->CmdListsCount; ++DrawListIndex)
			{
				const ImDrawList* ImguiDrawList = InDrawData->CmdLists[DrawListIndex];
				for (int CommandIndex = 0; CommandIndex < ImguiDrawList->CmdBuffer.Size; ++CommandIndex)
				{
					const ImDrawCmd* ImguiCommand = &ImguiDrawList->CmdBuffer[CommandIndex];
					if (ImguiCommand->UserCallback != NULL)
					{
						// User callback, registered via ImDrawList::AddCallback()
						// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
						if (ImguiCommand->UserCallback == ImDrawCallback_ResetRenderState)
							_ImGui_SetupRenderState(InDrawData, InImguiContext, InContext, InImguiCommandList);
						else
							ImguiCommand->UserCallback(ImguiDrawList, ImguiCommand);
					}
					else
					{
						float Left		= (ImguiCommand->ClipRect.x - ClipOffset.x)* ClipScale.x;
						float Top		= (ImguiCommand->ClipRect.y - ClipOffset.y)* ClipScale.y;
						float Right		= (ImguiCommand->ClipRect.z - ClipOffset.x)* ClipScale.x;
						float Bottom	= (ImguiCommand->ClipRect.w - ClipOffset.y)* ClipScale.y;

						// Apply Scissor, Bind texture, Draw

						if (static_cast<int>(Left) < ViewportWidth &&
							static_cast<int>(Top) < ViewportHeight &&
							Right >= 0.0f &&
							Left >= 0.0f)
						{
							Left	= Max<float>(0.0f, Left);
							Top		= Max<float>(0.0f, Top);

							ScissorRectangle ImguiRectangle =
							{
								static_cast<int32_t>(Left),
								static_cast<int32_t>(Top),
								static_cast<int32_t>(Right),
								static_cast<int32_t>(Bottom)
							};

							View* ImguiTexture = static_cast<View*>(ImguiCommand->GetTexID());
							_ImguiDescriptorTable->SetDescriptor(1, ImguiTexture);
							InImguiCommandList->SetScissorRectangle(ImguiRectangle);
							InImguiCommandList->SetGraphicsDescriptorTable(InContext, *_ImguiDescriptorTable);
							InImguiCommandList->DrawIndexedInstanced(
								ImguiCommand->ElemCount,
								1,
								ImguiCommand->IdxOffset + GlobalIndexOffset,
								ImguiCommand->VtxOffset + GlobalVertexOffset
							);
							++_PreviousDrawCount;
						}
					}
				}
				GlobalIndexOffset += ImguiDrawList->IdxBuffer.Size;
				GlobalVertexOffset += ImguiDrawList->VtxBuffer.Size;
			}
		}
	}
}
