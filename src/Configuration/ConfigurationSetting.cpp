#include "Configuration/ConfigurationSetting.hpp"
#include "Types/Types.hpp"
#include "File/File.hpp"
#include "File/FileFactory.hpp"
#include "File/FilePath.hpp"
#include "Math/Math.hpp"
#include "Time/Timer.hpp"
#include "rapidjson/allocators.h"
#include "rapidjson/prettywriter.h"

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
			if (!InConfiguration.HasMember(InSettingKey.c_str()))
				return;

			const auto& ConfigurationArray = InConfiguration[InSettingKey.c_str()].GetArray();

			for (uint32_t SettingArrayIndex = 0, SettingArrayCount = Math::Min<uint32_t>(ConfigurationArray.Size(), static_cast<uint32_t>(InOutSettingValue.size())); SettingArrayIndex < SettingArrayCount; ++SettingArrayIndex)
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
			OutConfiguration.AddMember(rapidjson::StringRef(InSettingKey.c_str()), rapidjson::Value(rapidjson::kArrayType), InAllocator);
			rapidjson::Value& ConfigurationValue = OutConfiguration[InSettingKey.c_str()];

			ConfigurationValue.Reserve(InSettingValue.size(), InAllocator);

			for (uint32_t SettingArrayIndex = 0; SettingArrayIndex < InSettingValue.size(); ++SettingArrayIndex)
			{
				ConfigurationValue.PushBack(rapidjson::Value(rapidjson::kObjectType), InAllocator);

				rapidjson::Value& CurrentSettingEntry = ConfigurationValue[SettingArrayIndex];

				CurrentSettingEntry.AddMember("x", InSettingValue[SettingArrayIndex].x, InAllocator);
				CurrentSettingEntry.AddMember("y", InSettingValue[SettingArrayIndex].y, InAllocator);
				CurrentSettingEntry.AddMember("z", InSettingValue[SettingArrayIndex].z, InAllocator);
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

			if (JsonGlobalConfiguration.IsObject())
			{
				ConfigurationSetting<float>::LoadConfiguration(JsonGlobalConfiguration);
				ConfigurationSetting<Types::Vector3>::LoadConfiguration(JsonGlobalConfiguration);
			}

			UnloadFileFromMemory(GlobalConfigurationContent);
		}

		void UpdateConfiguration(_In_ const Time::Timer* InTimer)
		{
			using namespace FileSystem;

			static constexpr Time::TimeSecondsT LastUpdateConfigurationFrequency = Time::TimeSecondsT(0.5);
			static Time::TimeSecondsT LastUpdateConfiguration = Time::TimeSecondsT(0.0);
			static bool IsDirty = false;

			IsDirty |= ConfigurationSetting<float>::UpdateConfiguration();
			IsDirty |= ConfigurationSetting<Types::Vector3>::UpdateConfiguration();

			Time::TimeSecondsT CurrentTimeSeconds = InTimer->GetTimeSeconds();

			if (IsDirty && (LastUpdateConfiguration + LastUpdateConfigurationFrequency < CurrentTimeSeconds))
			{
				rapidjson::Document JsonGlobalConfiguration;
				rapidjson::Value& GlobalConfigurationRoot = JsonGlobalConfiguration.SetObject();
				rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& Allocator = JsonGlobalConfiguration.GetAllocator();
				ConfigurationSetting<float>::WriteConfiguration(Allocator, GlobalConfigurationRoot);
				ConfigurationSetting<Types::Vector3>::WriteConfiguration(Allocator, GlobalConfigurationRoot);

				{
					string GlobalConfigurationFullPath = FilePath::FindOrCreate("global.config", FileType::FILE_TYPE_CONFIGURATIONS);

					FileScope GlobalConfigurationFileScope(GlobalConfigurationFullPath, FileOpenMode::FILE_OPEN_MODE_WRITE);
					{
						rapidjson::StringBuffer JsonGlobalConfigurationBuffer;
						rapidjson::PrettyWriter<rapidjson::StringBuffer> JsonGlobalConfigurationWriter(JsonGlobalConfigurationBuffer);
						JsonGlobalConfiguration.Accept(JsonGlobalConfigurationWriter);

						GlobalConfigurationFileScope->Write(reinterpret_cast<const uint8_t*>(JsonGlobalConfigurationBuffer.GetString()), JsonGlobalConfigurationBuffer.GetLength());
					}
				}

				LastUpdateConfiguration = CurrentTimeSeconds;
				IsDirty = false;
			}
		}
	}
}
