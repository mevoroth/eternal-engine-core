#include "Import/ImportLevel.hpp"
#include "File/FilePath.hpp"
#include "File/FileFactory.hpp"
#include "File/File.hpp"
#include "GameData/LevelGameData.hpp"
#include "GameData/JsonGameData.hpp"

namespace Eternal
{
	namespace Import
	{
		using namespace Eternal::FileSystem;
		using namespace Eternal::GameDataSystem;

		ImportLevel::ImportLevel()
		{
		}

		Level* ImportLevel::Import(_In_ const string& InName)
		{
			Level* OutLevel = nullptr;

			string FullPath = FilePath::Find(InName, FileType::FILE_TYPE_LEVELS);
			if (FullPath.size() > 0)
			{
				File* LevelFile = CreateFileHandle(FullPath);
				LevelFile->Open(File::READ);
				string LevelContent;
				LevelContent.resize(LevelFile->GetFileSize());
				LevelFile->Read(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(LevelContent.data())), LevelContent.size());
				LevelFile->Close();
				DestroyFileHandle(LevelFile);

				JsonLevel RootNode(LevelContent);

				LevelGameData GameData;
				OutLevel = GameData.Load<Level>(RootNode);
			}
			return OutLevel;
		}
	}
}
