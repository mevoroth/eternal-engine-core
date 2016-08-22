#ifndef _GAME_POOLS_HPP_
#define _GAME_POOLS_HPP_

#include "Resources/Pool.hpp"
#include "Core/TransformComponent.hpp"

using namespace Eternal::Resources;
using namespace Eternal::Core;

extern Pool<TransformComponent>* g_TransformComponentPool;

#endif
