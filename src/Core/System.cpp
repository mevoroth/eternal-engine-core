#include "Core/System.hpp"
#include "Core/Game.hpp"
#include "Core/SystemCreateInformation.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "GraphicsEngine/Renderer.hpp"
#include "File/FilePath.hpp"
#include "Time/Timer.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/Log.hpp"
#include "Log/LogFactory.hpp"
#include "Input/Input.hpp"
#include "Input/InputFactory.hpp"
#include "Input/InputMapping.hpp"
#include "Task/Core/RendererTask.hpp"
#include "Task/Core/StreamingTask.hpp"
#include "Task/Tools/AutoRecompileShaderTask.hpp"
#include "Parallel/AtomicS32.hpp"
#include "Parallel/AtomicS32Factory.hpp"
#include "Parallel/Sleep.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/LevelLoader.hpp"
#include "Resources/Payload.hpp"
#include "Optick/Optick.hpp"
#include "Mesh/Mesh.hpp"
#include "Camera/Camera.hpp"
#include "Components/TransformComponent.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::FileSystem;
		using namespace Eternal::LogSystem;
		using namespace Eternal::Parallel;

		namespace SystemPrivate
		{
			static bool ShowDebugVisibility = 0;
		}

		template<typename SystemType>
		void Destroy(_Inout_ SystemType*& InOutSystem)
		{
			delete InOutSystem;
			InOutSystem = nullptr;
		}

		System::System(_In_ SystemCreateInformation& InSystemCreateInformation)
			: _SystemCreateInformation(InSystemCreateInformation)
			, _RenderIndex(CreateAtomicS32())
			, _GameIndex(CreateAtomicS32())
		{
			for (uint32_t IncludeIndex = 0; IncludeIndex < InSystemCreateInformation.ShaderIncludePath.size(); ++IncludeIndex)
				FilePath::Register(InSystemCreateInformation.ShaderIncludePath[IncludeIndex],	FileType::FILE_TYPE_SHADERS);
			FilePath::Register(InSystemCreateInformation.ShaderPDBPath,							FileType::FILE_TYPE_SHADERS_PDB);
			FilePath::Register(InSystemCreateInformation.LevelPath,								FileType::FILE_TYPE_LEVELS);
			FilePath::Register(InSystemCreateInformation.FBXPath,								FileType::FILE_TYPE_MESHES);
			FilePath::Register(InSystemCreateInformation.FBXCachePath,							FileType::FILE_TYPE_CACHED_MESHES);
			FilePath::Register(InSystemCreateInformation.TexturePath,							FileType::FILE_TYPE_TEXTURES);
			FilePath::Register(InSystemCreateInformation.PipelineCachePath,						FileType::FILE_TYPE_CACHED_PIPELINES);
			FilePath::Register(InSystemCreateInformation.MaterialPath,							FileType::FILE_TYPE_MATERIALS);

			_Timer						= CreateTimer(TimeType::TIME_TYPE_DEFAULT);
			_Logs						= CreateMultiChannelLog({ LogType::LOG_TYPE_CONSOLE/*, LogType::LOG_TYPE_FILE*/ });
			_Input						= CreateMultiInput({ InputType::INPUT_TYPE_KEYBOARD, InputType::INPUT_TYPE_DEFAULT_PAD, InputType::INPUT_TYPE_MOUSE });
			_InputMapping				= new InputMapping(*_Input);

			_Streaming					= new Streaming(_TextureFactory);
			_Streaming->RegisterLoader(AssetType::ASSET_TYPE_LEVEL, new LevelLoader());

			TaskCreateInformation RendererCreateInformation("RendererTask");
			_RendererTask				= new RendererTask(RendererCreateInformation, *this);

			TaskCreateInformation StreamingCreateInformation("StreamingTask");
			_StreamingTask				= new StreamingTask(StreamingCreateInformation, *_Streaming);

			string CurrentDirectoryPath = FilePath::CurrentDirectory();
			LogWrite(LogInfo, LogEngine, CurrentDirectoryPath);
		}

		void System::InitializeSystem()
		{
			_Renderer					= _SystemCreateInformation.CreateCustomRendererFunction ? _SystemCreateInformation.CreateCustomRendererFunction(*_GraphicsContext) : new Renderer(*_GraphicsContext);
			_Imgui						= new Imgui(*_GraphicsContext, *_Renderer, _Input);

			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Frames[FrameIndex].InitializeSystemFrame(*_GraphicsContext, _Imgui->CreateContext(*_GraphicsContext));


			vector<Task*> Tasks = { _RendererTask, _StreamingTask };
			if (_AutoRecompileShaderTask)
				Tasks.push_back(_AutoRecompileShaderTask);

			_ParallelSystem = new ParallelSystem(ParallelSystemCreateInformation(
				GraphicsContext::FrameBufferingCount, Tasks
			));

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
			Destroy(_AutoRecompileShaderTask);

			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Imgui->DestroyContext(_Frames[FrameIndex].ImguiFrameContext);
			Destroy(_Imgui);
			Destroy(_Renderer);

			DestroyGraphicsContext(_GraphicsContext);

			Destroy(_Streaming);

			delete _InputMapping;
			_InputMapping = nullptr;
			DestroyInput(_Input);
			DestroyLog(_Logs);
			DestroyTimer(_Timer);
		}

		void System::_LoadBuiltin()
		{
			_Streaming->EnqueueRequest_MainThread(
				new TextureRequest(
					FilePath::Find("black.tga", FileType::FILE_TYPE_TEXTURES),
					"black",
					"black.tga"
				)
			);
			_Streaming->EnqueueRequest_MainThread(
				new TextureRequest(
					FilePath::Find("noise_curl_3d_xyzw.dds", FileType::FILE_TYPE_TEXTURES),
					"noise_curl_3d_xyzw",
					"noise_curl_3d_xyzw"
				)
			);
			_Streaming->EnqueueRequest_MainThread(
				new TextureRequest(
					FilePath::Find("cloud_base.dds", FileType::FILE_TYPE_TEXTURES),
					"cloud_base",
					"cloud_base"
				)
			);
			{
				for (uint32_t TextureIndex = 0; TextureIndex < static_cast<uint32_t>(TextureType::TEXTURE_TYPE_COUNT); ++TextureIndex)
				{
					if (static_cast<TextureType>(TextureIndex) == TextureType::TEXTURE_TYPE_NORMAL)
						continue;

					TextureRequest* DefaultMaterialTexture = new TextureRequest(
						FilePath::Find("black.tga", FileType::FILE_TYPE_TEXTURES),
						"black",
						"black.tga"
					);
					DefaultMaterialTexture->MaterialToUpdate.MaterialToUpdate	= Material::DefaultMaterial;
					DefaultMaterialTexture->MaterialToUpdate.Slot				= static_cast<TextureType>(TextureIndex);
					_Streaming->EnqueueRequest_MainThread(DefaultMaterialTexture);
				}

				TextureRequest* DefaultNormalTexture = new TextureRequest(
					FilePath::Find("defaultnormal.tga", FileType::FILE_TYPE_TEXTURES),
					"defaultnormal",
					"defaultnormal.tga"
				);
				DefaultNormalTexture->MaterialToUpdate.MaterialToUpdate			= Material::DefaultMaterial;
				DefaultNormalTexture->MaterialToUpdate.Slot						= TextureType::TEXTURE_TYPE_NORMAL;
				_Streaming->EnqueueRequest_MainThread(DefaultNormalTexture);
			}
			_Streaming->CommitRequests_MainThread();
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
			GetStreaming().GatherPayloads_MainThread(CurrentGameFrame.MaterialUpdateBatches);
			// Delayed payloads delete
			{
				MergeStreamingQueues(_DelayedDestroyedRequests, CurrentGameFrame.DelayedDestroyedRequests);

				for (uint32_t QueueType = 0; QueueType < _DelayedDestroyedRequests.size(); ++QueueType)
				{
					uint32_t RequestIndex = 0;
					uint32_t RequestsCount = static_cast<uint32_t>(_DelayedDestroyedRequests[QueueType].size());

					while (RequestIndex < RequestsCount)
					{
						if (_DelayedDestroyedRequests[QueueType][RequestIndex]->IsProcessed())
						{
							delete _DelayedDestroyedRequests[QueueType][RequestIndex];
							_DelayedDestroyedRequests[QueueType][RequestIndex] = nullptr;

							uint32_t LastRequestIndex = RequestsCount - 1;
							std::swap(_DelayedDestroyedRequests[QueueType][RequestIndex], _DelayedDestroyedRequests[QueueType][LastRequestIndex]);
							--RequestsCount;
						}
						else
						{
							++RequestIndex;
						}
					}
					uint32_t DestroyedCount = static_cast<uint32_t>(_DelayedDestroyedRequests[QueueType].size()) - RequestsCount;
					_DelayedDestroyedRequests[QueueType].erase(_DelayedDestroyedRequests[QueueType].end() - DestroyedCount, _DelayedDestroyedRequests[QueueType].end());
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
			{
				SystemFrame& OldestFrame = GetOldestGameFrame();

				const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentGameFrame.MeshCollections;

				uint32_t BoundingBoxesCount = 0;
				for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
				{
					Mesh*& Meshes = MeshCollections[CollectionIndex].Object->Meshes;

					BoundingBoxesCount += static_cast<uint32_t>(Meshes->GetGPUMesh().BoundingBoxes.size()) * static_cast<uint32_t>(MeshCollections[CollectionIndex].Instances.size());
				}

				if (CurrentGameFrame.MeshCollectionsVisibility.GetBitCount() != BoundingBoxesCount)
					CurrentGameFrame.MeshCollectionsVisibility.Resize(BoundingBoxesCount);

				if (CurrentGameFrame.MeshCollectionsVisibility.GetBitCount() == OldestFrame.MeshCollectionsVisibility.GetBitCount())
					CurrentGameFrame.MeshCollectionsVisibility = OldestFrame.MeshCollectionsVisibility;
			}
		}

		void System::Update()
		{
			ETERNAL_PROFILER(BASIC)();
			UpdatePlatform();
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

				if (!SystemPrivate::ShowDebugVisibility)
				{
					uint32_t VisibilityIndex = 0;
					for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
					{
						Mesh* Meshes = MeshCollections[CollectionIndex].Object->Meshes;

						for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
						{
							Matrix4x4 LocalToWorld = MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();

							vector<AxisAlignedBoundingBox>& CurrentBoundingBoxes = Meshes->GetGPUMesh().BoundingBoxes;

							for (uint32_t BoundingBoxIndex = 0; BoundingBoxIndex < CurrentBoundingBoxes.size(); ++BoundingBoxIndex)
							{
								bool IsIntersecting = CameraFrustum.Intersect(CurrentBoundingBoxes[BoundingBoxIndex].TransformBy(LocalToWorld));
								if (IsIntersecting)
									CurrentRenderFrame.MeshCollectionsVisibility.Set(VisibilityIndex);
								else
									CurrentRenderFrame.MeshCollectionsVisibility.Unset(VisibilityIndex);
								++VisibilityIndex;
							}
						}
					}
				}
			}
			MergeMaterialUpdateBatches(_MaterialUpdateBatcher, CurrentRenderFrame.MaterialUpdateBatches);

			GfxContext.RegisterGraphicsCommands(CurrentRenderFrame.GraphicsCommands.size() ? &CurrentRenderFrame.GraphicsCommands : nullptr);
			GfxContext.BeginFrame();

			if (CurrentRenderFrame.MeshCollectionsVisibility.GetBitCount() > 0 && CurrentRenderFrame.MeshCollectionsAccelerationStructure)
			{
				CommandListScope BuildAccelerationStructureCommandlist = GfxContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "BuildAccelerationStructure");
				const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentRenderFrame.MeshCollections;

				RebuildAccelerationStructureInput RebuildInput;
				RebuildInput.Instances.reserve(CurrentRenderFrame.MeshCollectionsVisibility.GetBitCount());

				for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
				{
					AccelerationStructure*& CurrentAccelerationStructure = MeshCollections[CollectionIndex].Object->MeshCollectionAccelerationStructure;

					static constexpr bool ForceRebuildBLAS = false;
					if (ForceRebuildBLAS)
					{
						vector<AccelerationStructureGeometry> Geometries;
						uint32_t GeometryIndex = 0u;
						Geometries.resize(static_cast<uint32_t>(MeshCollections[CollectionIndex].Object->Meshes->GetGPUMesh().PerDrawInformations.size()));

						GPUMesh& CurrentGPUMesh = MeshCollections[CollectionIndex].Object->Meshes->GetGPUMesh();

						for (uint32_t DrawIndex = 0; DrawIndex < CurrentGPUMesh.PerDrawInformations.size(); ++DrawIndex)
						{
							GPUMesh::PerDrawInformation& CurrentPerDrawInformation = CurrentGPUMesh.PerDrawInformations[DrawIndex];

							AccelerationStructureGeometry& CurrentGeometry = Geometries[GeometryIndex++];
							CurrentGeometry.VertexBuffer	= CurrentGPUMesh.MeshVertexBuffer;
							CurrentGeometry.IndexBuffer		= CurrentGPUMesh.MeshIndexBuffer;
							CurrentGeometry.TransformBuffer	= CurrentGPUMesh.MeshConstantBuffer;
							CurrentGeometry.IndicesCount	= CurrentPerDrawInformation.IndicesCount;
							CurrentGeometry.IndicesOffset	= CurrentPerDrawInformation.IndicesOffset;
							CurrentGeometry.VerticesOffset	= CurrentPerDrawInformation.VerticesOffset;
							CurrentGeometry.TransformsOffet	= DrawIndex;
						}

						GfxContext.DelayedDelete(CurrentAccelerationStructure);
						CurrentAccelerationStructure = CreateBottomLevelAccelerationStructure(
							GfxContext,
							BottomLevelAccelerationStructureCreateInformation(
								/*CurrentMesh->GetName()*/"BLAS",
								Geometries
							)
						);

						BuildAccelerationStructureCommandlist->BuildRaytracingAccelerationStructure(GfxContext, *CurrentAccelerationStructure);
						BuildAccelerationStructureCommandlist->TransitionUAV(CurrentAccelerationStructure->GetAccelerationStructure());
					}

					if (!CurrentAccelerationStructure)
						continue;

					for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
					{
						Matrix4x4 LocalToWorldTransposed = MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();
						Transpose(LocalToWorldTransposed);

						RebuildInput.Instances.push_back({});
						AccelerationStructureInstance& CurrentInstance			= RebuildInput.Instances.back();
						CurrentInstance.BottomLevelAccelerationStructureBuffer	= CurrentAccelerationStructure;
						CurrentInstance.InstanceToWorld							= Matrix3x4(&LocalToWorldTransposed.m[0][0]);
					}
				}

				CurrentRenderFrame.MeshCollectionsAccelerationStructure->RebuildAccelerationStructure(GfxContext, RebuildInput);

				BuildAccelerationStructureCommandlist->BuildRaytracingAccelerationStructure(GfxContext, *CurrentRenderFrame.MeshCollectionsAccelerationStructure);
				ResourceTransition TransitionTopLevelAccelerationStructure(CurrentRenderFrame.MeshCollectionsAccelerationStructure->GetAccelerationStructure(), TransitionState::TRANSITION_RAYTRACING_ACCELERATION_STRUCTURE);
				BuildAccelerationStructureCommandlist->Transition(TransitionTopLevelAccelerationStructure);
			}

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

					if (CurrentGameFrame.ViewCamera && CurrentGameFrame.MeshCollectionsVisibility.GetSize() > 0)
					{
						const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentGameFrame.MeshCollections;

						uint32_t VisibilityIndex = 0;
						for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
						{
							const ObjectsList<MeshCollection>::InstancedObjects& CurrentInstancedObject = MeshCollections[CollectionIndex];
							Mesh*& Meshes = CurrentInstancedObject.Object->Meshes;

							for (uint32_t InstanceIndex = 0; InstanceIndex < CurrentInstancedObject.Instances.size(); ++InstanceIndex)
							{
								vector<AxisAlignedBoundingBox>& CurrentBoundingBoxes = Meshes->GetGPUMesh().BoundingBoxes;

								for (uint32_t BoundingBoxIndex = 0; BoundingBoxIndex < CurrentBoundingBoxes.size(); ++BoundingBoxIndex)
								{
									bool IsIntersecting = CurrentGameFrame.MeshCollectionsVisibility.IsSet(VisibilityIndex);
									bool ShowBoundingBox = IsIntersecting;// CurrentGameFrame.MeshCollectionsBoundingBoxVisibility.IsSet(VisibilityIndex);
									char MeshName[255];
									sprintf_s(MeshName, "[%c] %s %d", IsIntersecting ? 'O' : 'X', Meshes->GetName().c_str(), BoundingBoxIndex);
									if (ImGui::Checkbox(MeshName, &ShowBoundingBox))
									{
										if (ShowBoundingBox)
											CurrentGameFrame.MeshCollectionsVisibility.Set(VisibilityIndex);
										else
											CurrentGameFrame.MeshCollectionsVisibility.Unset(VisibilityIndex);
									}
									++VisibilityIndex;
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
			ImGui::Begin("Eternal Debug");
			{
				if (ImGui::TreeNode("System Debug"))
				{
					ImGui::Checkbox("Debug Visibility", &SystemPrivate::ShowDebugVisibility);
					ImGui::TreePop();
				}
			}
			GetRenderer().RenderDebug();
			ImGui::End();
		}

		void System::EndFrame()
		{
			ETERNAL_PROFILER(BASIC)();
			SystemFrame& CurrentGameFrame = GetGameFrame();

			GetStreaming().CommitRequests_MainThread();
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

		//////////////////////////////////////////////////////////////////////////
		// SystemFrame

		SystemFrame::SystemFrame()
			: SystemState(CreateAtomicS32())
		{
			GraphicsCommands.reserve(EstimatedGraphicsCommandsCount);
		}

		SystemFrame::~SystemFrame()
		{
			DestroyAccelerationStructure(MeshCollectionsAccelerationStructure);
			DestroyAtomicS32(SystemState);
		}

		void SystemFrame::InitializeSystemFrame(_In_ GraphicsContext& InContext, _In_ const ImguiContext& InImguiContext)
		{
			MeshCollectionsAccelerationStructure	= InContext.GetDevice().SupportsRayTracing() ? CreateTopLevelAccelerationStructure(InContext, TopLevelAccelerationStructureCreateInformation("MeshCollectionsAccelerationStructure")) : nullptr;
			ImguiFrameContext						= InImguiContext;
		}
	}
}
