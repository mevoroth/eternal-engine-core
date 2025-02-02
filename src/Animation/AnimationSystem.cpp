#include "Animation/AnimationSystem.hpp"

namespace Eternal
{
	namespace Core
	{
		void AnimationSystem::UpdateAnimationSystem(_In_ float InDeltaSeconds)
		{
			for (uint32_t CollectionIndex = 0; CollectionIndex < _AnimationCollections.size(); ++CollectionIndex)
			{
				visit(
					[InDeltaSeconds](auto& InOutTimeline)
					{
						InOutTimeline.EvaluateCollection(InDeltaSeconds);
					},
					_AnimationCollections[CollectionIndex]
				);
			}
		}
	}
}
