#pragma once

#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Import
	{
		class ImportLevel;
	}

	namespace Resources
	{
		using namespace Eternal::Import;

		struct LevelLoader final : public StreamingLoader
		{
			LevelLoader();
			~LevelLoader();

			virtual void LoadPayload(_In_ const StreamingRequest& InRequest, _Out_ Payload*& OutPayload) const override final;
			virtual void DestroyPayloadLoader() override final;

		private:
			ImportLevel* _ImportLevel = nullptr;
		};
	}
}
