#pragma once

#include"..\LingCommon\__LingCommonShared.h"

#ifdef LINGVM_EXPORTS
#define LINGVM_DLLIO __declspec(dllexport)
#else
#define LINGVM_DLLIO __declspec(dllimport)
#endif
