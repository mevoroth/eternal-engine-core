#include "Resources/LevelPayload.hpp"

#include "File/FilePath.hpp"

namespace Eternal
{
	namespace Resources
	{
		LevelRequest::LevelRequest(_In_ const string& InPath)
			: StreamingRequest(InPath, FileType::FILE_TYPE_LEVELS)
		{
		}
	}
}
