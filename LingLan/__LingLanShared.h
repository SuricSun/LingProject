#pragma once

#include"..\LingCommon\__LingCommonShared.h"

#ifdef LINGLAN_EXPORTS
#define LINGLAN_DLLIO __declspec(dllexport)
#else
#define LINGLAN_DLLIO __declspec(dllimport)
#endif
