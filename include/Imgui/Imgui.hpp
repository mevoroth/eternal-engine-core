#pragma once

#include "Input/Input.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "imgui.h"

enum ImGuiKey_;
struct ImDrawData;
struct ImDrawList;

namespace Eternal
{
	namespace Memory
	{
		template<typename AllocationType>
		class StackAllocation;
	}

	namespace GraphicsEngine
	{
		class Renderer;
	}

	namespace ImguiSystem
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Memory;

		struct ImguiRenderContext;

		struct ImguiContext
		{
			~ImguiContext()
			{
				Reset();
			}

			ImGuiContext* Context = nullptr;
			ImDrawData DrawData;
			ImVector<ImDrawList*> DrawDataBuilderLayer0;

			void Reset();
		};

		class Imgui
		{
		public:
			static constexpr uint32_t ImguiMaxProjections	= 64;
			static constexpr uint32_t ImguiMaxVertices		= 65536;
			static constexpr uint32_t ImguiMaxIndices		= 65536;

			Imgui(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ Input* InInput);
			~Imgui();

			void Begin(_In_ const ImguiContext& InContext);
			void End(_In_ ImguiContext& InContext);

			ImguiContext CreateContext(_In_ GraphicsContext& InContext);
			void DestroyContext(_In_ const ImguiContext& InContext);

			void Render(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ const ImguiContext& InImguiContext);

		private:
			void _Map(_In_ const Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey);
			void _ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Key& KeyName);
			void _ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Key& KeyNameStart, _In_ uint32_t Range);
			void _UpdateInputs();
			void _Render(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ const ImguiContext& InImguiContext);
			void _UploadFontTexture(_In_ GraphicsContext& InContext);

			//void _ImGui_GetDrawData();
			void _ImGui_FillBuffers(_In_ const ImDrawData& InDrawData, _In_ ImguiRenderContext& InImguiContext);
			void _ImGui_SetupRenderState(_In_ const ImDrawData& InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList, _Inout_ StackAllocation<View>& InOutProjectionConstantsViews);
			void _ImGui_Render(_In_ const ImDrawData& InDrawData, _In_ ImguiRenderContext& InImguiContext, _In_ GraphicsContext& InContext, _In_ CommandList* InImguiCommandList);

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
			MultiBuffered<Resource>*	_ImguiConstantBuffer		= nullptr;
			MultiBuffered<View>*		_ImguiConstantBufferView	= nullptr;
			MultiBuffered<Resource>*	_ImguiVertexBuffer			= nullptr;
			MultiBuffered<Resource>*	_ImguiIndexBuffer			= nullptr;
			
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
