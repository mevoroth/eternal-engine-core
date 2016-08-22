#include "GameData/TransformGameData.hpp"

#include "Macros/Macros.hpp"
#include "Types/Types.hpp"
#include "Core/TransformComponent.hpp"
#include "include/json/json.h"

using namespace Eternal::GameData;
using namespace Eternal::Types;

namespace Eternal
{
	namespace GameData
	{
		class TransformGameDataCache
		{
		public:
			Json::Value Value;
		};
	}
}

TransformGameData::TransformGameData(_In_ TransformComponent& Component)
	: _Component(Component)
{
	_Cache = new TransformGameDataCache();
}

TransformGameData::~TransformGameData()
{
	delete _Cache;
	_Cache = nullptr;
}

void TransformGameData::Prepare(_In_ void* Parent)
{
	Json::Value* ParentNode = (Json::Value*)Parent;

	_Cache->Value.clear();

	{
		const Vector3& Translation = _Component.GetTransform().GetTranslation();
		Json::Value TranslationValues[] =
		{
			Translation.x,
			Translation.y,
			Translation.z
		};
		for (int ArrayIndex = 0; ArrayIndex < ETERNAL_ARRAYSIZE(TranslationValues); ++ArrayIndex)
		{
			_Cache->Value["Translation"].append(TranslationValues[ArrayIndex]);
		}
	}
	{
		const Vector4& Rotation = _Component.GetTransform().GetRotation();
		Json::Value RotationValues[] =
		{
			Rotation.x,
			Rotation.y,
			Rotation.z,
			Rotation.w
		};
		for (int ArrayIndex = 0; ArrayIndex < ETERNAL_ARRAYSIZE(RotationValues); ++ArrayIndex)
		{
			_Cache->Value["Rotation"].append(RotationValues[ArrayIndex]);
		}
	}
	{
		const Vector3& Scaling = _Component.GetTransform().GetScaling();
		Json::Value ScalingValues[] =
		{
			Scaling.x,
			Scaling.y,
			Scaling.z
		};
		for (int ArrayIndex = 0; ArrayIndex < ETERNAL_ARRAYSIZE(ScalingValues); ++ArrayIndex)
		{
			_Cache->Value["Scaling"].append(ScalingValues[ArrayIndex]);
		}
	}

	ParentNode->append(_Cache->Value);
}

void TransformGameData::GetData(_Out_ uint8_t* Data) const
{
	Json::FastWriter JsonWriter;
	JsonWriter.write(_Cache->Value)._Copy_s((char*)Data, _Cache->Value.size(), _Cache->Value.size());
}

size_t TransformGameData::GetSize() const
{
	Json::FastWriter JsonWriter;
	return JsonWriter.write(_Cache->Value).size();
}
