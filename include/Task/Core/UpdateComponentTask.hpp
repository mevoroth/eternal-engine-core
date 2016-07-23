#ifndef _UPDATE_COMPONENT_HPP_
#define _UPDATE_COMPONENT_HPP_

#include "Parallel/Task.hpp"
#include "Resources/Pool.hpp"
#include "Time/Time.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Time;

		template<class Pool>
		class UpdateComponentTask : public Task
		{
		public:
			UpdateComponentTask(_In_ Pool* PoolObj)
				: _Pool(PoolObj)
			{
			}

			virtual void Setup() override
			{
				SetState(SETUP);
			}

			virtual void Reset() override
			{
				SetState(IDLE);
			}

			virtual void Execute() override
			{
				SetState(EXECUTING);

				TimeT DeltaTime = Time::Time::Get()->GetDeltaTime();

				for (int ObjectIndex = 0; ObjectIndex < _Pool->GetSize(); ++ObjectIndex)
				{
					if (_Pool->IsValid((PoolHandle)ObjectIndex))
					{
						_Pool->Get((PoolHandle)ObjectIndex)->Update(DeltaTime);
					}
				}

				SetState(DONE);
			}

		private:
			Pool* _Pool = nullptr;
		};
	}
}

#endif
