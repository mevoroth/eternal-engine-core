#include "Core/Game.hpp"
#include "Core/GameState.hpp"
#include "Core/System.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/Payload.hpp"
#include "Resources/LevelPayload.hpp"
#include "GraphicsCommand/UploadMeshGraphicsCommand.hpp"
#include "Components/MeshComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Resources;
		using namespace Eternal::GraphicsCommands;

		Game::Game(_In_ GameCreateInformation& InGameCreateInformation)
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
			Streaming& StreamingSystem = GetSystem().GetStreaming();
			vector<GraphicsCommand*>& GraphicsCommands = GetSystem().GetGameFrame().GraphicsCommands;
			StreamingSystem.ProcessGathered(
				AssetType::ASSET_TYPE_LEVEL,
				[this](_In_ Payload& InPayload)
				{
					LevelPayload& InLevelPayload = static_cast<LevelPayload&>(InPayload);
					_World.AddLevel(InLevelPayload.LoadedLevel);
				}
			);

			StreamingSystem.ProcessGathered(
				AssetType::ASSET_TYPE_MESH,
				[this, &GraphicsCommands](_In_ Payload& InPayload)
				{
					MeshPayload& InMeshPayload = static_cast<MeshPayload&>(InPayload);
					GraphicsCommands.push_back(
						new UploadMeshGraphicsCommand(InMeshPayload)
					);
					for (uint32_t ComponentIndex = 0; ComponentIndex < InMeshPayload.ComponentsToUpdate.size(); ++ComponentIndex)
					{
						InMeshPayload.ComponentsToUpdate[ComponentIndex]->SetMesh(InMeshPayload.LoadedMesh);
					}
				}
			);
		}
	}
}
