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
					[InDeltaSeconds](_Inout_ auto& InOutTimeline)
					{
						InOutTimeline.EvaluateCollection(InDeltaSeconds);
					},
					_AnimationCollections[CollectionIndex]
				);
			}
		}

		void AnimationSystem::UnregisterProperty(_In_ const AnimationHandle& InHandle)
		{
			visit(
				[&InHandle](_Inout_ auto& InOutTimeline) mutable
				{
					InOutTimeline.AnimationProperties.Erase(InHandle.PropertyIndex);
				},
				_AnimationCollections[InHandle.CollectionIndex]
			);
		}

		void AnimationSystem::RegisterOnCompleteFunction(_In_ const AnimationHandle& InHandle, _In_ const AnimationOnCompleteFunctor& InFunction)
		{
			visit(
				[&InHandle, &InFunction](_Inout_ auto& InOutTimeline) mutable
				{
					InOutTimeline.AnimationProperties[InHandle.PropertyIndex].AnimationOnCompleteFunctions.push_back(InFunction);
				},
				_AnimationCollections[InHandle.CollectionIndex]
			);
		}

		void AnimationSystem::Play(_In_ const AnimationHandle& InHandle)
		{
			visit(
				[&InHandle](_Inout_ auto& InOutTimeline) mutable
				{
					InOutTimeline.AnimationProperties[InHandle.PropertyIndex].AnimationPlayback = AnimationPlaybackState::ANIMATIONPLAYBACKSTATE_PLAY;
				},
				_AnimationCollections[InHandle.CollectionIndex]
			);
		}

		void AnimationSystem::Stop(_In_ const AnimationHandle& InHandle)
		{
			visit(
				[&InHandle](_Inout_ auto& InOutTimeline) mutable
				{
					InOutTimeline.AnimationProperties[InHandle.PropertyIndex].AnimationPlayback		= AnimationPlaybackState::ANIMATIONPLAYBACKSTATE_STOP;
					InOutTimeline.AnimationProperties[InHandle.PropertyIndex].AnimationCurrentTime	= 0.0f;
				},
				_AnimationCollections[InHandle.CollectionIndex]
			);
		}

		void AnimationSystem::Pause(_In_ const AnimationHandle& InHandle)
		{
			visit(
				[&InHandle](_Inout_ auto& InOutTimeline) mutable
				{
					InOutTimeline.AnimationProperties[InHandle.PropertyIndex].AnimationPlayback = AnimationPlaybackState::ANIMATIONPLAYBACKSTATE_PAUSE;
				},
				_AnimationCollections[InHandle.CollectionIndex]
			);
		}
	}
}
