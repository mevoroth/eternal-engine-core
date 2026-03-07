#include "Configuration/ConfigurationSetting.hpp"
#include "Types/Types.hpp"
#include "File/File.hpp"
#include "File/FileFactory.hpp"
#include "File/FilePath.hpp"
#include "Math/Math.hpp"
#include "rapidjson/allocators.h"

namespace Eternal
{
	namespace Core
	{
		template<> void ReadConfigurationValue(_In_ const rapidjson::Document& InConfiguration, _In_ const std::string& InSettingKey, _Inout_ std::vector<float>& InOutSettingValue)
		{
			for (uint32_t SettingArrayIndex = 0; SettingArrayIndex < InOutSettingValue.size(); ++SettingArrayIndex)
			{
				InOutSettingValue[SettingArrayIndex] = InConfiguration[InSettingKey.c_str()].GetArray()[SettingArrayIndex].GetFloat();
			}
		}

		template<> void ReadConfigurationValue(_In_ const rapidjson::Document& InConfiguration, _In_ const std::string& InSettingKey, _Inout_ std::vector<Types::Vector3>& InOutSettingValue)
		{
			const auto& ConfigurationArray = InConfiguration[InSettingKey.c_str()].GetArray();

			for (uint32_t SettingArrayIndex = 0, SettingArrayCount = Math::Min<uint32_t>(ConfigurationArray.Size(), InOutSettingValue.size()); SettingArrayIndex < SettingArrayCount; ++SettingArrayIndex)
			{
				InOutSettingValue[SettingArrayIndex] = Types::Vector3(
					ConfigurationArray[SettingArrayIndex]["x"].GetFloat(),
					ConfigurationArray[SettingArrayIndex]["y"].GetFloat(),
					ConfigurationArray[SettingArrayIndex]["z"].GetFloat()
				);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		template<> void WriteConfigurationValue(_In_ rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& InAllocator, _In_ const std::vector<float>& InSettingValue, _In_ const std::string& InSettingKey, _Out_ rapidjson::Value& OutConfiguration)
		{
			ETERNAL_ASSERT(!OutConfiguration.HasMember(InSettingKey.c_str()));
			rapidjson::Value& ConfigurationValue = OutConfiguration[InSettingKey.c_str()].SetArray();

			for (uint32_t SettingArrayIndex = 0; SettingArrayIndex < InSettingValue.size(); ++SettingArrayIndex)
			{
				ConfigurationValue[SettingArrayIndex] = InSettingValue[SettingArrayIndex];
			}
		}

		template<> void WriteConfigurationValue(_In_ rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& InAllocator, _In_ const std::vector<Types::Vector3>& InSettingValue, _In_ const std::string& InSettingKey, _Out_ rapidjson::Value& OutConfiguration)
		{
			ETERNAL_ASSERT(!OutConfiguration.HasMember(InSettingKey.c_str()));
			//OutConfiguration.AddMember(InSettingKey.c_str(), rapidjson::Value().Move(), InAllocator);
			rapidjson::Value& ConfigurationValue = OutConfiguration[InSettingKey.c_str()].SetArray();

			for (uint32_t SettingArrayIndex = 0; SettingArrayIndex < InSettingValue.size(); ++SettingArrayIndex)
			{
				ConfigurationValue[SettingArrayIndex]["x"] = InSettingValue[SettingArrayIndex].x;
				ConfigurationValue[SettingArrayIndex]["y"] = InSettingValue[SettingArrayIndex].y;
				ConfigurationValue[SettingArrayIndex]["z"] = InSettingValue[SettingArrayIndex].z;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		void LoadConfiguration()
		{
			using namespace FileSystem;

			string GlobalConfigurationFullPath = FilePath::FindOrCreate("global.config", FileType::FILE_TYPE_CONFIGURATIONS);

			if (!FileExists(GlobalConfigurationFullPath))
			{
				FileScope SafeCreateFile(GlobalConfigurationFullPath, FileOpenMode::FILE_OPEN_MODE_WRITE);
			}

			FileContent GlobalConfigurationContent = LoadFileToMemory(GlobalConfigurationFullPath);

			rapidjson::Document JsonGlobalConfiguration;
			JsonGlobalConfiguration.Parse(reinterpret_cast<const char*>(GlobalConfigurationContent.Content), GlobalConfigurationContent.Size);

			ConfigurationSetting<float>::LoadConfiguration(JsonGlobalConfiguration);
			ConfigurationSetting<Types::Vector3>::LoadConfiguration(JsonGlobalConfiguration);

			UnloadFileFromMemory(GlobalConfigurationContent);
		}

		void UpdateConfiguration()
		{
			using namespace FileSystem;

			bool IsDirty = false;
			IsDirty |= ConfigurationSetting<float>::UpdateConfiguration();
			IsDirty |= ConfigurationSetting<Types::Vector3>::UpdateConfiguration();
			if (IsDirty)
			{
				rapidjson::Document JsonGlobalConfiguration;
				rapidjson::Value& GlobalConfigurationRoot = JsonGlobalConfiguration.SetObject();
				rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& Allocator = JsonGlobalConfiguration.GetAllocator();
				ConfigurationSetting<float>::WriteConfiguration(Allocator, GlobalConfigurationRoot);
				ConfigurationSetting<Types::Vector3>::WriteConfiguration(Allocator, GlobalConfigurationRoot);

				{
					string GlobalConfigurationFullPath = FilePath::FindOrCreate("global.config", FileType::FILE_TYPE_CONFIGURATIONS);

					FileScope GlobalConfigurationFileScope(GlobalConfigurationFullPath, FileOpenMode::FILE_OPEN_MODE_WRITE);
					
					GlobalConfigurationFileScope->Write(reinterpret_cast<const uint8_t*>(JsonGlobalConfiguration.GetString()), JsonGlobalConfiguration.GetStringLength());
				}
			}
		}
	}
}
