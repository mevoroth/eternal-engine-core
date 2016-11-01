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

		template<class Pool>
		class UpdateComponentTask : public Task
		{
		public:
			UpdateComponentTask(_In_ Eternal::Time::Time* TimeObj, _In_ Pool* PoolObj)
				: _Time(TimeObj)
				, _Pool(PoolObj)
			{
				ETERNAL_ASSERT(_Time);
				ETERNAL_ASSERT(_Pool);
			}

			virtual void DoSetup() override
			{
				SetState(SETUP);
			}

			virtual void DoReset() override
			{
				SetState(IDLE);
			}

			virtual void DoExecute() override
			{
				SetState(EXECUTING);

				Eternal::Time::TimeSecondsT DeltaTimeSeconds = _Time->GetDeltaTimeSeconds();

				for (uint32_t ObjectIndex = 0; ObjectIndex < _Pool->GetSize(); ++ObjectIndex)
				{
					if (_Pool->IsValid((PoolHandle)ObjectIndex))
					{
						_Pool->Get((PoolHandle)ObjectIndex)->Update(DeltaTimeSeconds);
					}
				}

				SetState(DONE);
			}

		private:
			Pool* _Pool = nullptr;
			Eternal::Time::Time* _Time = nullptr;
		};
	}
}

#endif
