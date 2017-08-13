#ifndef _ENGINE_COMMAND_HPP_
#define _ENGINE_COMMAND_HPP_

namespace Eternal
{
	namespace GraphicData
	{
		class GraphicResources;
	}

	namespace Graphics
	{
		class Device;
		class CommandList;
	}

	namespace Core
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::Graphics;

		struct EngineCommand
		{
			virtual void Execute(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ CommandList& CommandListObj) = 0;
		};
	}
}

#endif
