#include "GameData/MeshGameData.hpp"

#include <string>

namespace Eternal
{
	namespace GameDataSystem
	{
		void* MeshGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			std::string* MeshPath = new std::string;
			InNode.Get(*MeshPath);
			return MeshPath;
		}
	}
}
