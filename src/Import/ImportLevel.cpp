#include "Import/ImportLevel.hpp"
#include "File/FilePath.hpp"
#include "File/FileFactory.hpp"
#include "File/File.hpp"
#include "GameData/LevelGameData.hpp"
#include "GameData/JsonGameData.hpp"
#include "Resources/LevelPayload.hpp"

namespace Eternal
{
	namespace Import
	{
		using namespace Eternal::FileSystem;
		using namespace Eternal::GameDataSystem;
		using namespace Eternal::Resources;

		ImportLevel::ImportLevel()
		{
		}

		LevelPayload* ImportLevel::Import(_In_ const string& InName)
		{
			LevelPayload* OutLevelPayload = nullptr;

			string FullPath = FilePath::Find(InName, FileType::FILE_TYPE_LEVELS);
			if (FullPath.size() > 0)
			{
				string LevelContent;

				File* LevelFile = CreateFileHandle(FullPath);
				LevelFile->Open(FileOpenMode::FILE_OPEN_MODE_READ);
				LevelContent.resize(LevelFile->GetFileSize());
				LevelFile->Read(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(LevelContent.data())), LevelContent.size());
				LevelFile->Close();
				DestroyFileHandle(LevelFile);

				JsonLevel RootNode(LevelContent);
				LevelGameData GameData;
				OutLevelPayload = GameData.Load<LevelPayload>(RootNode);
			}

			return OutLevelPayload;
		}
	}
}
