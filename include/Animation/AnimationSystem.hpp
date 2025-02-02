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

		//class AnimationHandle
		//{
		//	uint16_t _CollectionIndex = 0;
		//	uint16_t
		//};

		class AnimationSystem
		{
		public:

			template<typename AnimationStatePropertyType>
			void RegisterProperty(_In_ Animation* InAnimation, _In_ AnimationStatePropertyType& InProperty)
			{
				bool IsRegistered = false;
				for (uint32_t CollectionIndex = 0; !IsRegistered && CollectionIndex < _AnimationCollections.size(); ++CollectionIndex)
				{
					visit(
						[InAnimation, &InProperty, &IsRegistered](auto& InOutTimeline) mutable
						{
							if constexpr (std::is_same_v<decltype(InOutTimeline), AnimationCollection<AnimationStatePropertyType>&>)
							{
								if (InOutTimeline.AnimationData == InAnimation)
								{
									InOutTimeline.AnimationProperties.push_back(AnimationState<AnimationStatePropertyType> { 0.0f, & InProperty });
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
					NewCollection.AnimationProperties.reserve(16);
					AnimationState<AnimationStatePropertyType> NewAnimationState;
					NewAnimationState.AnimationProperty = &InProperty;
					NewCollection.AnimationProperties.push_back(NewAnimationState);
					NewCollection.AnimationData = InAnimation;
					_AnimationCollections.push_back(std::move(NewCollection));
				}
			}

			void UpdateAnimationSystem(_In_ float InDeltaSeconds);

		private:

			vector<AnimationCollectionVariants> _AnimationCollections;

		};
	}
}
