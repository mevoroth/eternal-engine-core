#include "Resources/LevelLoader.hpp"
#include "Resources/LevelPayload.hpp"
#include "Import/ImportLevel.hpp"
#include "Log/Log.hpp"

namespace Eternal
{
	namespace Resources
	{
		using namespace Eternal::Import;

		LevelLoader::LevelLoader()
			: _ImportLevel(new ImportLevel())
		{
		}

		LevelLoader::~LevelLoader()
		{
			delete _ImportLevel;
			_ImportLevel = nullptr;
		}

		void LevelLoader::LoadPayload(_In_ const StreamingRequest& InRequest, _Out_ Payload*& OutPayload) const
		{
			ETERNAL_PROFILER(BASIC)();
			OutPayload = _ImportLevel->Import(InRequest.RequestPath);
			LogWrite(LogInfo, LogImport, string("Loaded [") + InRequest.RequestPath + "]");
		}

		void LevelLoader::DestroyPayloadLoader()
		{
			delete this;
		}
	}
}
