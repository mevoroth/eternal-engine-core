#pragma once

#define ETERNAL_STACK_VIEWS_PARAMETERS(InCount)			alloca(GetViewSize(InContext) * (InCount)), GetViewSize(InContext), (InCount)
