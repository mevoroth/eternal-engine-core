#include "Core/World.hpp"
#include "Core/Level.hpp"

namespace Eternal
{
	namespace Core
	{
		void World::AddLevel(_In_ Level* InLevel)
		{
			_Levels.push_back(InLevel);
			InLevel->SetWorld(this);
		}
	}
}
