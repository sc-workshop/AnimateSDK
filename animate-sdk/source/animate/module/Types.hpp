#pragma once

#include <string>
#include <map>
#include <optional>

namespace Animate {
	class Value final
	{
	public:
		using Map = std::map<std::u16string, Value>;

	public:

		Value(bool supported)
		{
			m_isSupported = supported;
		}

		bool IsSupported() const
		{
			return m_isSupported;
		}

	private:
		bool m_isSupported;
	};

	class Property final
	{
	public:
		using Map = std::map<std::u16string, Property>;

	public:
		Property(const std::string& def, bool supported)
		{
			m_isSupported = supported;
			m_default = def;
		}

		std::optional<std::reference_wrapper<const Value>> FindValue(const std::u16string& inValueName) const
		{
			Value::Map::const_iterator itr = m_values.find(inValueName);
			if (itr != m_values.end())
				return itr->second;
			return std::nullopt;
		}

		bool AddValue(const std::u16string& valueName, const Value& value)
		{
			m_values.insert(std::pair<std::u16string, Value>(valueName, value));

			return true;
		}

		bool IsSupported() const
		{
			return m_isSupported;
		}

		std::string GetDefault() const
		{
			return m_default;
		}

	private:
		std::string m_default;
		bool m_isSupported;
		Value::Map m_values;
	};

	class Feature final
	{
	public:
		using Map = std::map<std::u16string, Feature>;

	public:

		Feature(bool supported)
		{
			m_isSupported = supported;
		}

		std::optional<std::reference_wrapper<const Property>> FindProperty(const std::u16string& inPropertyName) const
		{
			Property::Map::const_iterator itr = m_properties.find(inPropertyName);
			if (itr != m_properties.end())
			{
				return itr->second;
			}
			return std::nullopt;
		}

		bool AddProperty(const std::u16string& name, const Property& property)
		{
			m_properties.insert(std::pair<std::u16string, Property>(name, property));

			return true;
		}

		bool IsSupported() const
		{
			return m_isSupported;
		}

	private:
		bool m_isSupported;
		Property::Map m_properties;
	};

	struct ModuleInfo
	{
		const FCM::U_Int32& version;

		const char* name;
		const char* universal_name;
		const char* description;
		const char* publisher_ui_id;

		ModuleInfo(const FCM::U_Int32& ver, const char* nam, const char* uvn, const char* dsc, const char* puiid) :
			version(ver),
			name(nam),
			universal_name(uvn),
			description(dsc),
			publisher_ui_id(puiid)
		{
		}
	};

	struct FCMPluginID
	{
		using T = const FCM::FCMCLSID&;
		T PublisherID;
		T DocumentTypeID;
		T FeatureMatrixID;

		FCMPluginID(T p, T d, T f) :
			PublisherID(p),
			DocumentTypeID(d),
			FeatureMatrixID(f)
		{
		}
	};
}
