#pragma once

#include "Animation/AnimationCollection.hpp"
#include <variant>

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Components;
		using namespace std;

		using AnimationCollectionVariants = variant<
			AnimationCollection<float>,
			AnimationCollection<Eternal::Types::Vector2>,
			AnimationCollection<Eternal::Types::Vector3>,
			AnimationCollection<Eternal::Types::Vector4>
		>;

		class AnimationHandle
		{
			static constexpr uint32_t InvalidCollectionIndex	= 0xFFFF;
			static constexpr uint32_t InvalidPropertyIndex		= 0xFFFF;

			uint32_t CollectionIndex	= InvalidCollectionIndex;
			uint32_t PropertyIndex		= InvalidCollectionIndex;

			friend class AnimationSystem;
		};

		static constexpr AnimationHandle InvalidAnimationHandle;

		class AnimationSystem
		{
		public:

			void UpdateAnimationSystem(_In_ float InDeltaSeconds);

			template<typename AnimationStatePropertyType>
			AnimationHandle RegisterProperty(_In_ Animation* InAnimation, _In_ AnimationStatePropertyType& InProperty)
			{
				AnimationHandle Handle = InvalidAnimationHandle;

				bool IsRegistered = false;
				for (uint32_t CollectionIndex = 0; !IsRegistered && CollectionIndex < _AnimationCollections.size(); ++CollectionIndex)
				{
					visit(
						[InAnimation, &InProperty, &IsRegistered, &CollectionIndex, &Handle](auto& InOutTimeline) mutable
						{
							if constexpr (std::is_same_v<decltype(InOutTimeline), AnimationCollection<AnimationStatePropertyType>&>)
							{
								if (InOutTimeline.AnimationData == InAnimation)
								{
									Handle.CollectionIndex	= CollectionIndex;
									Handle.PropertyIndex	= InOutTimeline.AnimationProperties.PushBack(AnimationState<AnimationStatePropertyType> { AnimationPlaybackState::ANIMATIONPLAYBACKSTATE_STOP, 0.0f, & InProperty });
									IsRegistered = true;
								}
							}
						},
						_AnimationCollections[CollectionIndex]
					);
				}

				if (!IsRegistered)
				{
					AnimationCollection<AnimationStatePropertyType> NewCollection;
					NewCollection.AnimationProperties.Reserve(16);
					AnimationState<AnimationStatePropertyType> NewAnimationState;
					NewAnimationState.AnimationProperty = &InProperty;
					Handle.PropertyIndex = NewCollection.AnimationProperties.PushBack(move(NewAnimationState));

					NewCollection.AnimationData = InAnimation;
					_AnimationCollections.push_back(std::move(NewCollection));
					Handle.CollectionIndex = static_cast<uint32_t>(_AnimationCollections.size()) - 1u;
				}

				return Handle;
			}
			
			void UnregisterProperty(_In_ const AnimationHandle& InHandle);

			void RegisterOnCompleteFunction(_In_ const AnimationHandle& InHandle, _In_ const AnimationOnCompleteFunctor& InFunction);

			void Play(_In_ const AnimationHandle& InHandle);
			void Stop(_In_ const AnimationHandle& InHandle);
			void Pause(_In_ const AnimationHandle& InHandle);

		private:

			vector<AnimationCollectionVariants> _AnimationCollections;

		};
	}
}
