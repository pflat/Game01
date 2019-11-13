#pragma once

//  These macros are meant to be used before-after the
//  #include directives of third-party code,
//  in order to suppress warning messages.
//  Only useful with gcc

#if defined(__MINGW32__)
#define THIRD_PARTY_GUARDS_BEGIN \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic warning \"-w\"")
#else
#define THIRD_PARTY_GUARDS_BEGIN \
__pragma(warning(push)) \
__pragma(warning(disable: 4305 4275 4251))
#endif

#if defined(__MINGW32__)
#define THIRD_PARTY_GUARDS_END \
_Pragma("GCC diagnostic pop")
#else
#define THIRD_PARTY_GUARDS_END \
__pragma(warning(pop))
#endif
