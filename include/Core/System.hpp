#pragma once

#include "Core/SystemCreateInformation.hpp"
#include "Parallel/ParallelSystem.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Resources/TextureFactory.hpp"
#include "Resources/Streaming.hpp"
#include "resources/Payload.hpp"
#include "Bit/BitField.hpp"
#include "Imgui/Imgui.hpp"
#include <array>

namespace Eternal
{
	namespace Graphics
	{
		struct GraphicsContextCreateInformation;
		class GraphicsContext;
		class AccelerationStructure;
	}

	namespace GraphicsEngine
	{
		class Renderer;
	}

	namespace Time
	{
		class Timer;
	}

	namespace LogSystem
	{
		class Log;
	}

	namespace InputSystem
	{
		class Input;
		class InputMapping;
	}

	namespace Import
	{
		class ImportTga;
		class ImportFbx;
		class ImportLevel;
	}

	namespace Tasks
	{
		class RendererTask;
		class StreamingTask;
		class AutoRecompileShaderTask;
	}

	namespace Parallel
	{
		class AtomicS32;
		class Mutex;
	}

	namespace Resources
	{
		class Streaming;
	}

	namespace Components
	{
		class MeshCollection;
		class Camera;
		class Light;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;
		using namespace Eternal::Time;
		using namespace Eternal::LogSystem;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Import;
		using namespace Eternal::Parallel;
		using namespace Eternal::Tasks;
		using namespace Eternal::ImguiSystem;
		using namespace Eternal::Resources;
		using namespace Eternal::Components;

		class Game;
		struct SystemFrame;
		class TransformComponent;

		static constexpr int SystemCanBeRendered = 1;
		static constexpr int SystemCanBeWritten = 0;

		template<typename ObjectType>
		struct ObjectsList
		{
			struct InstancedObjects
			{
				ObjectType* Object = nullptr;
				vector<TransformComponent*> Instances;
			};

			void AddObject(_In_ ObjectType* InObject, _In_ TransformComponent* InTransformComponent)
			{
				InstancedObjects* CurrentInstancedObjects = FindOrCreate(InObject, Objects);
				InstancedObjects* CurrentPendingInstancedObjects = FindOrCreate(InObject, PendingObjects);

				CurrentInstancedObjects->Instances.push_back(InTransformComponent);
				CurrentPendingInstancedObjects->Instances.push_back(InTransformComponent);
			}

			void CommitObjects(_Inout_ ObjectsList<ObjectType>& InOutOldestFrameObjectList)
			{
				for (uint32_t PendingObjectIndex = 0; PendingObjectIndex < InOutOldestFrameObjectList.PendingObjects.size(); ++PendingObjectIndex)
				{
					if (InOutOldestFrameObjectList.PendingObjects[PendingObjectIndex].Instances.size() > 0)
					{
						InstancedObjects& CurrentPendingInstancedObjects = InOutOldestFrameObjectList.PendingObjects[PendingObjectIndex];

						InstancedObjects* CurrentInstancedObjects = FindOrCreate(InOutOldestFrameObjectList.PendingObjects[PendingObjectIndex].Object, Objects);
						CurrentInstancedObjects->Instances.insert(
							CurrentInstancedObjects->Instances.end(),
							CurrentPendingInstancedObjects.Instances.begin(),
							CurrentPendingInstancedObjects.Instances.end()
						);
						CurrentPendingInstancedObjects.Instances.clear();
					}
				}
			}

			operator const vector<InstancedObjects>&() const { return Objects; }

			vector<InstancedObjects> Objects;
			vector<InstancedObjects> PendingObjects;

		private:
			InstancedObjects* FindOrCreate(_In_ ObjectType* InObject, _Inout_ vector<InstancedObjects>& InOutObjects)
			{
				InstancedObjects* CurrentInstancedObjects = nullptr;
				for (uint32_t ObjectIndex = 0; ObjectIndex < InOutObjects.size(); ++ObjectIndex)
				{
					if (InOutObjects[ObjectIndex].Object == InObject)
					{
						CurrentInstancedObjects = &InOutObjects[ObjectIndex];
						break;
					}
				}
				if (!CurrentInstancedObjects)
				{
					InOutObjects.push_back({});
					CurrentInstancedObjects = &InOutObjects.back();
					CurrentInstancedObjects->Object = InObject;
				}
				return CurrentInstancedObjects;
			}
		};

		struct SystemFrame
		{
			static constexpr uint32_t EstimatedGraphicsCommandsCount	= 1024;
			static constexpr uint32_t EstimatedMeshesCount				= 4096;

			SystemFrame();
			~SystemFrame();

			void InitializeSystemFrame(_In_ GraphicsContext& InContext, _In_ const ImguiContext& InImguiContext);

			ImguiContext ImguiFrameContext;
			ObjectsList<MeshCollection> MeshCollections;
			ObjectsList<Light> Lights;
			vector<GraphicsCommand*> GraphicsCommands;
			MaterialUpdateBatcher MaterialUpdateBatches;
			PayloadQueueType DelayedDestroyedRequests;
			DynamicBitField<> MeshCollectionsVisibility;
			DynamicBitField<> MeshCollectionsBoundingBoxVisibility;
			
			AtomicS32* SystemState											= nullptr;
			AccelerationStructure* MeshCollectionsAccelerationStructure		= nullptr;

			Camera* ViewCamera												= nullptr;
			Camera* PendingViewCamera										= nullptr;
		};

		class System
		{
		public:

			System(_In_ SystemCreateInformation& InSystemCreateInformation);
			virtual ~System();

			inline GraphicsContext& GetGraphicsContext()
			{
				ETERNAL_ASSERT(_GraphicsContext);
				return *_GraphicsContext;
			}

			inline InputMapping& GetInputMapping()
			{
				ETERNAL_ASSERT(_InputMapping);
				return *_InputMapping;
			}

			inline ParallelSystem& GetParallelSystem()
			{
				ETERNAL_ASSERT(_ParallelSystem);
				return *_ParallelSystem;
			}

			inline Imgui& GetImgui()
			{
				ETERNAL_ASSERT(_Imgui);
				return *_Imgui;
			}

			inline Streaming& GetStreaming()
			{
				ETERNAL_ASSERT(_Streaming);
				return *_Streaming;
			}

			inline Renderer& GetRenderer()
			{
				ETERNAL_ASSERT(_Renderer);
				return *_Renderer;
			}

			inline Timer& GetTimer()
			{
				ETERNAL_ASSERT(_Timer);
				return *_Timer;
			}

			inline TextureFactory& GetTextureFactory()
			{
				return _TextureFactory;
			}

			inline MaterialUpdateBatcher& GetMaterialUpdateBatcher()
			{
				return _MaterialUpdateBatcher;
			}

			void InitializeSystem();

			SystemFrame& GetGameFrame();
			SystemFrame& GetOldestGameFrame();
			SystemFrame& GetRenderFrame();
			void AdvanceRender();
			void AdvanceGame();

			void StartFrame();
			void EndFrame();
			void Update();
			void Render();

			void UpdateDebug();
			void RenderDebug();

			virtual void UpdatePlatform() {}

		protected:
			Input*														_Input						= nullptr;
			InputMapping*												_InputMapping				= nullptr;
			GraphicsContext*											_GraphicsContext			= nullptr;
			AutoRecompileShaderTask*									_AutoRecompileShaderTask	= nullptr;

		private:
			void _LoadBuiltin();
			
			SystemCreateInformation&									_SystemCreateInformation;
			TextureFactory												_TextureFactory;
			MaterialUpdateBatcher										_MaterialUpdateBatcher;
			ParallelSystem*												_ParallelSystem				= nullptr;
			Imgui*														_Imgui						= nullptr;
			Timer*														_Timer						= nullptr;
			Log*														_Logs						= nullptr;
			Streaming*													_Streaming					= nullptr;
			Renderer*													_Renderer					= nullptr;

			//////////////////////////////////////////////////////////////////////////
			// Tasks
			RendererTask*												_RendererTask				= nullptr;
			StreamingTask*												_StreamingTask				= nullptr;

			//////////////////////////////////////////////////////////////////////////
			// System state
			array<SystemFrame, GraphicsContext::FrameBufferingCount>	_Frames;
			PayloadQueueType											_DelayedDestroyedRequests;
			AtomicS32*													_RenderIndex				= nullptr;
			AtomicS32*													_GameIndex					= nullptr;
		};
	}
}
