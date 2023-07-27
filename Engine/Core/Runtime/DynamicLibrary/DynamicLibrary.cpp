#include <Engine/Core/Core.hpp>

#ifdef LMC_WIN
#	include "DynamicLibraryImpl/DynamicLibraryWin.inl"
#elif defined(LMC_APPLE) // Should be working also on linux platform, not yet tried
#	include "DynamicLibraryImpl/DynamicLibraryAPPLE.inl"
#endif
