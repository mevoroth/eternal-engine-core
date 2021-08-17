#pragma once

#include "Input/Input.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "imgui.h"

enum ImGuiKey_;
struct ImDrawData;

namespace Eternal
{
	namespace GraphicsEngine
	{
		class Renderer;
	}

	namespace ImguiSystem
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;
		using namespace Eternal::InputSystem;

		struct ImguiRenderContext;

		struct ImguiContext
		{
			ImGuiContext* Context = nullptr;
		};

		class Imgui
		{
		public:
			static constexpr uint32_t ImguiMaxProjections	= 64;
			static constexpr uint32_t ImguiMaxVertices		= 65536;
			static constexpr uint32_t ImguiMaxIndices		= 65536;

			Imgui(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ Input* InInput);
			~Imgui();

			void Begin();
			void End();

			ImguiContext CreateContext(_In_ GraphicsContext& InContext);
			void SetContext(_In_ const ImguiContext& InContext);
			void DestroyContext(_In_ const ImguiContext& InContext);

			void Render(_In_ GraphicsContext& InContext);

		private:
			void _Map(_In_ const Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey);
			void _ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Key& KeyName);
			void _ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Key& KeyNameStart, _In_ uint32_t Range);
			void _UpdateInputs();
			void _Render(_In_ GraphicsContext& InContext);
			void _UploadFontTexture(_In_ GraphicsContext& InContext);

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

			vector<Input::Key>			_MappedKeys;
			Input*						_Input = nullptr;

			ImguiFontMetaData			_ImguiFontMetaData;
			uint32_t					_PreviousDrawCount = 1;

			// Graphics resources
			MultiBuffered<Resource>*	_ImguiConstantBuffer;
			MultiBuffered<View>*		_ImguiConstantBufferView;
			MultiBuffered<Resource>*	_ImguiVertexBuffer;
			MultiBuffered<Resource>*	_ImguiIndexBuffer;
			
			RenderPass*					_ImguiRenderPass			= nullptr;
			Pipeline*					_ImguiPipeline				= nullptr;
			Sampler*					_ImguiBilinearSampler		= nullptr;
			InputLayout*				_ImguiInputLayout			= nullptr;
			RootSignature*				_ImguiRootSignature			= nullptr;
			BlendState*					_ImguiBlendState			= nullptr;
			DescriptorTable*			_ImguiDescriptorTable		= nullptr;
			Resource*					_ImguiFontTexture			= nullptr;
			View*						_ImguiFontTextureView		= nullptr;
		};
	}
}
