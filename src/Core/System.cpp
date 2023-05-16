#include "Core/System.hpp"
#include "Core/Game.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "GraphicsEngine/Renderer.hpp"
#include "File/FilePath.hpp"
#include "Time/Timer.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/LogFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Task/Core/RendererTask.hpp"
#include "Task/Core/StreamingTask.hpp"
#include "Parallel/AtomicS32.hpp"
#include "Parallel/AtomicS32Factory.hpp"
#include "Parallel/Sleep.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/LevelLoader.hpp"
#include "Resources/Payload.hpp"
#include "Optick/Optick.hpp"
#include "GraphicData/Texture.hpp"
#include "Mesh/Mesh.hpp"
#include "Camera/Camera.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::FileSystem;
		using namespace Eternal::LogSystem;
		using namespace Eternal::Platform;
		using namespace Eternal::Parallel;

		template<typename SystemType>
		void Destroy(_Inout_ SystemType*& InOutSystem)
		{
			delete InOutSystem;
			InOutSystem = nullptr;
		}

		System::System(_In_ SystemCreateInformation& InSystemCreateInformation)
			: _SystemCreateInformation(InSystemCreateInformation)
			, _GameIndex(CreateAtomicS32())
			, _RenderIndex(CreateAtomicS32())
		{
			for (uint32_t IncludeIndex = 0; IncludeIndex < InSystemCreateInformation.ShaderIncludePath.size(); ++IncludeIndex)
				FilePath::Register(InSystemCreateInformation.ShaderIncludePath[IncludeIndex],	FileType::FILE_TYPE_SHADERS);
			FilePath::Register(InSystemCreateInformation.LevelPath,								FileType::FILE_TYPE_LEVELS);
			FilePath::Register(InSystemCreateInformation.FBXPath,								FileType::FILE_TYPE_MESHES);
			FilePath::Register(InSystemCreateInformation.FBXCachePath,							FileType::FILE_TYPE_CACHED_MESHES);
			FilePath::Register(InSystemCreateInformation.TexturePath,							FileType::FILE_TYPE_TEXTURES);

			_Timer				= CreateTimer(TimeType::TIME_TYPE_WIN);
			_Logs				= CreateMultiChannelLog({ LogType::LOG_TYPE_CONSOLE/*, LogType::LOG_TYPE_FILE*/ });
			_Input				= CreateMultiInput({ InputType::INPUT_TYPE_WIN, InputType::INPUT_TYPE_XINPUT });

			WindowsProcess::SetInputHandler(_Input);
			WindowsProcess::SetIsRunning(&InSystemCreateInformation.GameContext->_Running);
			
			_GraphicsContext	= CreateGraphicsContext(InSystemCreateInformation.ContextInformation);

			_Streaming			= new Streaming(_TextureFactory);
			_Streaming->RegisterLoader(AssetType::ASSET_TYPE_LEVEL, new LevelLoader());

			_Renderer			= new Renderer(*_GraphicsContext);
			_Imgui				= new Imgui(*_GraphicsContext, *_Renderer, _Input);

			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Frames[FrameIndex].ImguiFrameContext = _Imgui->CreateContext(*_GraphicsContext);

			TaskCreateInformation RendererCreateInformation("RendererTask");
			_RendererTask		= new RendererTask(RendererCreateInformation, *this);

			TaskCreateInformation StreamingCreateInformation("StreamingTask");
			_StreamingTask		= new StreamingTask(StreamingCreateInformation, *this);

			ParallelSystemCreateInformation ParallelSystemInformation(
				GraphicsContext::FrameBufferingCount,
				{ _RendererTask, _StreamingTask }
			);
			_ParallelSystem		= new ParallelSystem(ParallelSystemInformation);

			Optick::OptickStart(*_GraphicsContext);

			_LoadBuiltin();
		}

		System::~System()
		{
			_Streaming->Shutdown();

			Destroy(_ParallelSystem);

			Optick::OptickStop();

			Destroy(_StreamingTask);
			Destroy(_RendererTask);

			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Imgui->DestroyContext(_Frames[FrameIndex].ImguiFrameContext);

			DestroyGraphicsContext(_GraphicsContext);

			Destroy(_Imgui);
			Destroy(_Streaming);
			Destroy(_Renderer);

			DestroyInput(_Input);
			DestroyLog(_Logs);
			DestroyTimer(_Timer);
		}

		void System::_LoadBuiltin()
		{
			_Streaming->EnqueueRequest(
				new TextureRequest(
					FilePath::Find("black.tga", FileType::FILE_TYPE_TEXTURES),
					"black",
					"black"
				)
			);
			_Streaming->EnqueueRequest(
				new TextureRequest(
					FilePath::Find("noise_curl_3d_xyzw.dds", FileType::FILE_TYPE_TEXTURES),
					"noise_curl_3d_xyzw",
					"noise_curl_3d_xyzw"
				)
			);
			_Streaming->EnqueueRequest(
				new TextureRequest(
					FilePath::Find("cloud_base.dds", FileType::FILE_TYPE_TEXTURES),
					"cloud_base",
					"cloud_base"
				)
			);
			_Streaming->CommitRequests();
		}

		void System::StartFrame()
		{
			ETERNAL_PROFILER(BASIC)();
			SystemFrame& CurrentGameFrame = GetGameFrame();

			{
				ETERNAL_PROFILER(BASIC)("WaitForSystemCanBeWritten");
				while (CurrentGameFrame.SystemState->Load() != SystemCanBeWritten)
				{
					Sleep(10);
				}
			}
			GetParallelSystem().StartFrame();
			GetImgui().Begin(CurrentGameFrame.ImguiFrameContext);
			GetStreaming().GatherPayloads();
			// Delayed payloads delete
			{
				for (uint32_t QueueType = 0; QueueType < CurrentGameFrame.DelayedDestroyedRequests.size(); ++QueueType)
				{
					for (uint32_t RequestIndex = 0; RequestIndex < CurrentGameFrame.DelayedDestroyedRequests[QueueType].size(); ++RequestIndex)
					{
						delete CurrentGameFrame.DelayedDestroyedRequests[QueueType][RequestIndex];
						CurrentGameFrame.DelayedDestroyedRequests[QueueType][RequestIndex] = nullptr;
					}
					CurrentGameFrame.DelayedDestroyedRequests[QueueType].clear();
				}
			}
			{
				SystemFrame& OldestFrame = GetOldestGameFrame();

				CurrentGameFrame.MeshCollections.CommitObjects(OldestFrame.MeshCollections);
				CurrentGameFrame.Lights.CommitObjects(OldestFrame.Lights);

				// Camera
				CurrentGameFrame.ViewCamera = OldestFrame.PendingViewCamera ? OldestFrame.PendingViewCamera : OldestFrame.ViewCamera;
				OldestFrame.PendingViewCamera = nullptr;
			}
		}

		void System::Update()
		{
			ETERNAL_PROFILER(BASIC)();
			WindowsProcess::ExecuteMessageLoop();
			_Timer->Update();
			_Input->Update();
		}

		void System::Render()
		{
			SystemFrame& CurrentRenderFrame = GetRenderFrame();
			GraphicsContext& GfxContext = GetGraphicsContext();
			
			//GetParallelSystem().ParallelFor("Culling", 2, [](_In_ uint32_t InIndex) {
			//});
			
			if (CurrentRenderFrame.ViewCamera)
			{
				const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentRenderFrame.MeshCollections;

				Frustum CameraFrustum;
				CurrentRenderFrame.ViewCamera->ComputeFrustum(CameraFrustum);

				uint32_t BoundingBoxesCount = 0;
				for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
				{
					vector<Mesh*>& Meshes = MeshCollections[CollectionIndex].Object->Meshes;

					uint32_t SubMeshCount = 0;
					for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
						SubMeshCount += static_cast<uint32_t>(Meshes[MeshIndex]->GetGPUMesh().BoundingBoxes.size());

					BoundingBoxesCount += SubMeshCount * static_cast<uint32_t>(MeshCollections[CollectionIndex].Instances.size());
				}

				CurrentRenderFrame.MeshCollectionsVisibility.Resize(BoundingBoxesCount);

				uint32_t VisibilityIndex = 0;
				for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
				{
					vector<Mesh*>& Meshes = MeshCollections[CollectionIndex].Object->Meshes;

					for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
					{
						Matrix4x4 LocalToWorld = MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();

						for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
						{
							vector<AxisAlignedBoundingBox>& CurrentBoundingBoxes = Meshes[MeshIndex]->GetGPUMesh().BoundingBoxes;

							for (uint32_t BoundingBoxIndex = 0; BoundingBoxIndex < CurrentBoundingBoxes.size(); ++BoundingBoxIndex)
							{
								bool IsIntersecting = CameraFrustum.Intersect(CurrentBoundingBoxes[BoundingBoxIndex].TransformBy(LocalToWorld));
								if (IsIntersecting)
									CurrentRenderFrame.MeshCollectionsVisibility.Set(VisibilityIndex);
								++VisibilityIndex;
							}
						}
					}
				}
			}

			GfxContext.RegisterGraphicsCommands(CurrentRenderFrame.GraphicsCommands.size() ? &CurrentRenderFrame.GraphicsCommands : nullptr);
			GfxContext.BeginFrame();

			GetRenderer().Render(GfxContext, *this);
			GetImgui().Render(GfxContext, GetRenderer(), CurrentRenderFrame.ImguiFrameContext);

			GetRenderer().Present(GfxContext, *this);

			GfxContext.EndFrame();
		}
		void System::UpdateDebug()
		{
			ImGui::Begin("Eternal Debug");
			{
				const TextureCacheStorage& Textures = GetTextureFactory().GetTextures();

				if (ImGui::TreeNode("Textures Cache"))
				{
					if (ImGui::BeginTable("Textures", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
					{
						for (auto TexturesIterator = Textures.cbegin(); TexturesIterator != Textures.cend(); ++TexturesIterator)
						{
							ImGui::TableNextRow();
							{
								ImGui::TableNextColumn();
								ImGui::Text(TexturesIterator->first.c_str());
							}
							{
								ImGui::TableNextColumn();
								if (TexturesIterator->second.CachedTexture)
								{
									const Resource& CurrentTexture = TexturesIterator->second.CachedTexture->GetTexture();
									ImGui::Text(
										"%dx%dx%d [%d]",
										CurrentTexture.GetWidth(),
										CurrentTexture.GetHeight(),
										CurrentTexture.GetDepth(),
										static_cast<int>(CurrentTexture.GetFormat())
									);
								}
								else
									ImGui::Text("UNLOADED");
							}
							{
								ImGui::TableNextColumn();
								if (TexturesIterator->second.CachedTexture && TexturesIterator->second.CachedTexture->GetTexture().GetResourceDimension() == ResourceDimension::RESOURCE_DIMENSION_TEXTURE_2D)
								{
									const View* CurrentTexture = TexturesIterator->second.CachedTexture->GetShaderResourceView();
									ImGui::Image(static_cast<ImTextureID>(const_cast<View*>(CurrentTexture)), ImVec2(32.0f, 32.0f));
								}
								else
									ImGui::Text("X");
							}
						}
						ImGui::EndTable();
					}
					ImGui::TreePop();
				}
			}
			{
				if (ImGui::TreeNode("Visibility"))
				{
					SystemFrame& CurrentGameFrame = GetGameFrame();
					GraphicsContext& GfxContext = GetGraphicsContext();

					if (CurrentGameFrame.ViewCamera && CurrentGameFrame.MeshCollectionsVisibility.GetSize() > 0)
					{
						CurrentGameFrame.MeshCollectionsBoundingBoxVisibility = GetRenderFrame().MeshCollectionsBoundingBoxVisibility;
						if (CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.GetBitCount() != CurrentGameFrame.MeshCollectionsVisibility.GetBitCount())
							CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.Resize(CurrentGameFrame.MeshCollectionsVisibility.GetBitCount());

						const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentGameFrame.MeshCollections;

						uint32_t VisibilityIndex = 0;
						for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
						{
							vector<Mesh*>& Meshes = MeshCollections[CollectionIndex].Object->Meshes;

							for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
							{
								Matrix4x4 LocalToWorld = MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();

								for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
								{
									vector<AxisAlignedBoundingBox>& CurrentBoundingBoxes = Meshes[MeshIndex]->GetGPUMesh().BoundingBoxes;

									for (uint32_t BoundingBoxIndex = 0; BoundingBoxIndex < CurrentBoundingBoxes.size(); ++BoundingBoxIndex)
									{
										bool IsIntersecting = CurrentGameFrame.MeshCollectionsVisibility.IsSet(VisibilityIndex);
										bool ShowBoundingBox = CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.IsSet(VisibilityIndex);
										char MeshName[255];
										sprintf_s(MeshName, "[%c] %s %d", IsIntersecting ? 'O' : 'X', Meshes[MeshIndex]->GetName().c_str(), BoundingBoxIndex);
										if (ImGui::Checkbox(MeshName, &ShowBoundingBox))
										{
											if (ShowBoundingBox)
												CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.Set(VisibilityIndex);
											else
												CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.Unset(VisibilityIndex);
										}
										++VisibilityIndex;
									}
								}
							}
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		void System::RenderDebug()
		{
			GetRenderer().RenderDebug();
		}

		void System::EndFrame()
		{
			ETERNAL_PROFILER(BASIC)();
			SystemFrame& CurrentGameFrame = GetGameFrame();

			GetStreaming().CommitRequests();
			GetImgui().End(CurrentGameFrame.ImguiFrameContext);
			GetParallelSystem().EndFrame();
			GetGameFrame().SystemState->Store(SystemCanBeRendered);
			AdvanceGame();
		}

		//////////////////////////////////////////////////////////////////////////
		// System Frame

		SystemFrame& System::GetGameFrame()
		{
			ETERNAL_ASSERT(_GameIndex);
			return _Frames[_GameIndex->Load()];
		}

		SystemFrame& System::GetOldestGameFrame()
		{
			ETERNAL_ASSERT(_GameIndex);
			return _Frames[(_GameIndex->Load() + 1) % _Frames.size()];
		}

		SystemFrame& System::GetRenderFrame()
		{
			ETERNAL_ASSERT(_RenderIndex);
			return _Frames[_RenderIndex->Load()];
		}

		void System::AdvanceRender()
		{
			_RenderIndex->Store((_RenderIndex->Load() + 1) % _Frames.size());
		}

		void System::AdvanceGame()
		{
			_GameIndex->Store((_GameIndex->Load() + 1) % _Frames.size());
		}

		SystemFrame::SystemFrame()
			: SystemState(CreateAtomicS32())
		{
			GraphicsCommands.reserve(EstimatedGraphicsCommandsCount);
		}

		SystemFrame::~SystemFrame()
		{
			DestroyAtomicS32(SystemState);
		}
	}
}
