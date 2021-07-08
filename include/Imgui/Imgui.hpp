#pragma once

#include "Input/Input.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "imgui.h"

enum ImGuiKey_;
struct ImDrawData;

namespace Eternal
{
	namespace Imgui
	{
		using namespace Eternal::Graphics;

		struct ImguiRenderContext;

		class Imgui
		{
		public:
			static constexpr uint32_t ImguiMaxProjections	= 64;
			static constexpr uint32_t ImguiMaxVertices		= 65536;
			static constexpr uint32_t ImguiMaxIndices		= 65536;

			Imgui(_In_ GraphicsContext& InContext, _In_ Input::Input* InInput);

			void Begin();
			void End();

			CommandList* Render(_In_ GraphicsContext& InContext);

		private:
			void _Map(_In_ const Input::Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey);
			void _ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Input::Key& KeyName);
			void _ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Input::Key& KeyNameStart, _In_ uint32_t Range);
			void _UpdateInputs();

			void _ImGui_FillBuffers(_In_ ImDrawData* DrawData, _In_ ImguiRenderContext& InImguiContext);
			void _ImGui_SetupRenderState(_In_ ImDrawData* InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList);
			void _ImGui_Render(_In_ ImDrawData* InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList);

			struct ImguiFontMetaData
			{
				unsigned char* Pixels		= nullptr;
				int Width					= 0;
				int Height					= 0;
				int BytesPerPixel			= 0;
			};

			vector<Input::Input::Key>	_MappedKeys;
			Input::Input*				_Input = nullptr;

			ImguiFontMetaData			_ImguiFontMetaData;

			// Graphics resources
			array<RenderPass*, GraphicsContext::FrameBufferingCount>	_ImguiRenderPasses;
			MultiBuffered<Resource>*									_ImguiConstantBuffer;
			MultiBuffered<View>*										_ImguiConstantBufferView;
			MultiBuffered<Resource>*									_ImguiVertexBuffer;
			MultiBuffered<Resource>*									_ImguiIndexBuffer;

			Shader*														_ImguiVS					= nullptr;
			Shader*														_ImguiPS					= nullptr;
			Pipeline*													_ImguiPipeline				= nullptr;
			Sampler*													_ImguiBilinearSampler		= nullptr;
			InputLayout*												_ImguiInputLayout			= nullptr;
			RootSignature*												_ImguiRootSignature			= nullptr;
			BlendState*													_ImguiBlendState			= nullptr;
			DescriptorTable*											_ImguiDescriptorTable		= nullptr;
			Resource*													_ImguiFontTexture			= nullptr;
			View*														_ImguiFontTextureView		= nullptr;
		};
	}
}
