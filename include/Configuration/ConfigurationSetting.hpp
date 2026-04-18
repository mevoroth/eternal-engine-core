#pragma once

#include "rapidjson/document.h"
#include <string>

namespace Eternal
{
	namespace Time
	{
		class Timer;
	}

	namespace Core
	{
		template<typename SettingType>
		void ReadConfigurationValue(_In_ const rapidjson::Document& InConfiguration, _In_ const std::string& InSettingKey, _Out_ std::vector<SettingType>& OutSettingValue);

		template<typename SettingType>
		void WriteConfigurationValue(_In_ rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& InAllocator, _In_ const std::vector<SettingType>& InSettingValue, _In_ const std::string& InSettingKey, _Out_ rapidjson::Value& OutConfiguration);

		void LoadConfiguration();
		void UpdateConfiguration(_In_ const Time::Timer* InTimer);

		template<typename SettingType>
		class ConfigurationSetting
		{
		public:

			static std::vector<ConfigurationSetting<SettingType>*>& GetConfigurationSettings()
			{
				static std::vector<ConfigurationSetting<SettingType>*> StaticConfigurationSettings;
				return StaticConfigurationSettings;
			}

			static void LoadConfiguration(_In_ const rapidjson::Document& InConfiguration)
			{
				std::vector<ConfigurationSetting<SettingType>*>& ConfigurationSettings = GetConfigurationSettings();
				for (uint32_t SettingIndex = 0; SettingIndex < static_cast<uint32_t>(ConfigurationSettings.size()); ++SettingIndex)
				{
					ReadConfigurationValue<SettingType>(InConfiguration, ConfigurationSettings[SettingIndex]->_SettingKey, ConfigurationSettings[SettingIndex]->_SettingValue);
					ConfigurationSettings[SettingIndex]->_SettingValueShadow = ConfigurationSettings[SettingIndex]->_SettingValue;
				}
			}

			static bool UpdateConfiguration()
			{
				std::vector<ConfigurationSetting<SettingType>*>& ConfigurationSettings = GetConfigurationSettings();
				bool IsDirty = false;
				for (uint32_t SettingIndex = 0; SettingIndex < static_cast<uint32_t>(ConfigurationSettings.size()); ++SettingIndex)
				{
					if (ConfigurationSettings[SettingIndex]->_SettingValue != ConfigurationSettings[SettingIndex]->_SettingValueShadow)
					{
						ConfigurationSettings[SettingIndex]->_SettingValueShadow = ConfigurationSettings[SettingIndex]->_SettingValue;
						IsDirty = true;
					}
				}

				return IsDirty;
			}

			static void WriteConfiguration(_In_ rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& InAllocator, _Out_ rapidjson::Value& OutConfiguration)
			{
				std::vector<ConfigurationSetting<SettingType>*>& ConfigurationSettings = GetConfigurationSettings();
				for (uint32_t SettingIndex = 0; SettingIndex < static_cast<uint32_t>(ConfigurationSettings.size()); ++SettingIndex)
				{
					WriteConfigurationValue<SettingType>(InAllocator, ConfigurationSettings[SettingIndex]->_SettingValue, ConfigurationSettings[SettingIndex]->_SettingKey, OutConfiguration);
					ConfigurationSettings[SettingIndex]->_SettingValueShadow = ConfigurationSettings[SettingIndex]->_SettingValue;
				}
			}

			ConfigurationSetting(_In_ const char* InSettingKey, _In_ const SettingType& InSettingValue, uint32_t SettingCount = 1)
				: _SettingKey(InSettingKey)
			{
				ETERNAL_ASSERT(SettingCount > 0 && "ConfigurationSetting needs to be non-zero");

				GetConfigurationSettings().push_back(this);

				_SettingValue.resize(SettingCount);
				_SettingValueShadow.resize(SettingCount);

				for (uint32_t SettingIndex = 0; SettingIndex < static_cast<uint32_t>(_SettingValue.size()); ++SettingIndex)
				{
					_SettingValue[SettingIndex]			= InSettingValue;
					_SettingValueShadow[SettingIndex]	= InSettingValue;
				}
			}

			~ConfigurationSetting()
			{
				std::vector<ConfigurationSetting<SettingType>*>& ConfigurationSettings = GetConfigurationSettings();
				ConfigurationSettings.erase(std::find(ConfigurationSettings.begin(), ConfigurationSettings.end(), this));
			}

			SettingType* operator&()
			{
				return &_SettingValue[0];
			}

			operator SettingType& ()
			{
				return _SettingValue[0];
			}

			SettingType& operator[](_In_ int InIndex)
			{
				ETERNAL_ASSERT(InIndex >= 0 && InIndex < _SettingValue.size());
				return _SettingValue[InIndex];
			}

		private:

			std::string					_SettingKey;
			std::vector<SettingType>	_SettingValue;
			std::vector<SettingType>	_SettingValueShadow;

		};
	}
}
