#include "Core/Game.hpp"
#include "Core/GameState.hpp"
#include "Core/System.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/Payload.hpp"
#include "Resources/LevelPayload.hpp"
#include "GraphicsCommand/GraphicsCommandUploadMesh.hpp"
#include "GraphicsCommand/GraphicsCommandUploadTexture.hpp"
#include "Components/MeshComponent.hpp"
#include "Time/Timer.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Resources;
		using namespace Eternal::GraphicsCommands;

		Game::Game(_In_ GameCreateInformation& InGameCreateInformation)
			: _World(*this)
		{
			InGameCreateInformation.SystemInformation.GameContext = this;
			_System = new System(InGameCreateInformation.SystemInformation);
		}

		Game::~Game()
		{
			delete _System;
			_System = nullptr;
		}

		void Game::Run()
		{
			ETERNAL_ASSERT(_CurrentGameState);

			OPTICK_THREAD("MainThread");

			_CurrentGameState->Begin();
			while (_Running)
			{
				OPTICK_FRAME("GameFrame");
				GetSystem().StartFrame();
				GetSystem().Update();
				Update();
				{
					ETERNAL_PROFILER(BASIC)("GameState Update");
					_CurrentGameState->Update();
				}
				{
					ETERNAL_PROFILER(BASIC)("GameState Debug Update");
					_CurrentGameState->UpdateDebug();
				}
				{
					ETERNAL_PROFILER(BASIC)("GameState Debug Graphics");
					GetSystem().RenderDebug();
				}
				UpdateDebug();
				GetSystem().EndFrame();

				{
					ETERNAL_PROFILER(BASIC)("GameState End");
					GameState* NextState = _CurrentGameState->NextState();

					if (!NextState)
					{
						_CurrentGameState->End();
						_CurrentGameState = nullptr;
						return;
					}

					if (_CurrentGameState != NextState)
					{
						_CurrentGameState->End();
						NextState->Begin();
						_CurrentGameState = NextState;
					}
				}
			}
		}

		void Game::Update()
		{
			ETERNAL_PROFILER(BASIC)();

			ProcessStreamingPayloads();

			_DeltaSeconds	= GetSystem().GetTimer().GetDeltaTimeSeconds();
			_ElapsedTime	+= _DeltaSeconds;
			_World.Update(_DeltaSeconds);
		}

		void Game::UpdateDebug()
		{
			ETERNAL_PROFILER(BASIC)();

			_World.UpdateDebug();
		}

		void Game::ProcessStreamingPayloads()
		{
			Streaming& StreamingSystem = GetSystem().GetStreaming();
			TextureFactory& Factory = GetSystem().GetTextureFactory();
			vector<GraphicsCommand*>& GraphicsCommands = GetSystem().GetGameFrame().GraphicsCommands;

			StreamingSystem.ProcessGathered(
				AssetType::ASSET_TYPE_LEVEL,
				GetSystem().GetGameFrame().DelayedDestroyedRequests,
				[this](_In_ Payload& InPayload)
				{
					LevelPayload& InLevelPayload = static_cast<LevelPayload&>(InPayload);
					_World.AddLevel(InLevelPayload.LoadedLevel);
				}
			);

			StreamingSystem.ProcessGathered(
				AssetType::ASSET_TYPE_MESH,
				GetSystem().GetGameFrame().DelayedDestroyedRequests,
				[this, &GraphicsCommands](_In_ Payload& InPayload)
				{
					MeshPayload& InMeshPayload = static_cast<MeshPayload&>(InPayload);
					GraphicsCommands.push_back(
						new GraphicsCommandUploadMesh(InMeshPayload)
					);
					for (uint32_t ComponentIndex = 0; ComponentIndex < InMeshPayload.ComponentsToUpdate.size(); ++ComponentIndex)
					{
						InMeshPayload.ComponentsToUpdate[ComponentIndex]->SetMesh(InMeshPayload.LoadedMesh);
					}
				}
			);

			StreamingSystem.ProcessGathered(
				AssetType::ASSET_TYPE_TEXTURE,
				GetSystem().GetGameFrame().DelayedDestroyedRequests,
				[this, &GraphicsCommands, &Factory](_In_ Payload& InPayload)
				{
					TexturePayload& InTexturePayload = static_cast<TexturePayload&>(InPayload);
					GraphicsCommands.push_back(
						new GraphicsCommandUploadTexture(InTexturePayload, Factory)
					);
				}
			);
		}
	}
}
