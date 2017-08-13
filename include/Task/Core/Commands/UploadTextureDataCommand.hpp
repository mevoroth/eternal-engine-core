#ifndef _UPLOAD_TEXTURE_DATA_COMMAND_HPP_
#define _UPLOAD_TEXTURE_DATA_COMMAND_HPP_

#include <cstdint>
#include "Task/Core/Commands/EngineCommand.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Resource;
	}

	namespace Core
	{
		struct UploadTextureDataCommand : public EngineCommand
		{
			Resource*	DestinationTexture	= nullptr;
			void*		TextureData			= nullptr;
			uint32_t	Width				= 0u;
			uint32_t	Height				= 0u;

			virtual void Execute(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ CommandList& CommandListObj) override;
		};
	}
}

#endif
