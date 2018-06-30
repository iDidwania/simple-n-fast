#ifndef _SNF_COMMON_H_
#define _SNF_COMMON_H_

#include <sys/types.h>
#include <cstring>
#include <cstdint>
#include <cinttypes>
#include <string>
#include <sstream>
#include "dbg.h"

#if defined(_WIN32)

	#include <Windows.h>
	#include <io.h>

	#define strcasecmp      _stricmp
	#define strncasecmp     _strnicmp

	using mode_t = DWORD;
	using pid_t = DWORD;
	using tid_t = DWORD;
	using fhandle_t = HANDLE;

#else /* if !defined(_WIN32) */

	#include <unistd.h>
	#include <sys/syscall.h>
	#include <errno.h>

	#define INVALID_HANDLE_VALUE    (-1)

	using tid_t = uint32_t;
	using fhandle_t = int;

#endif

#if !defined(MAXPATHLEN)
#define MAXPATHLEN      1023
#endif

#if !defined(ERRSTRLEN)
#define ERRSTRLEN       255
#endif

#define is_set(V, F)    (((V) & (F)) == (F))

namespace snf {

template<typename T, typename S>
T narrow_cast(S v)
{
	auto r = static_cast<T>(v);
	if (static_cast<S>(r) != v) {
		throw std::runtime_error("narrowing the value causes data loss");
	}
	return r;
}

constexpr bool
isnewline(int c)
{
	return ((c == '\n') || (c == '\r'));
}

constexpr char
pathsep(void)
{
#if defined(_WIN32)
	return '\\';
#else
	return '/';
#endif
}

inline int
system_error(void)
{
#if defined(_WIN32)
	return static_cast<int>(::GetLastError());
#else
	return errno;
#endif
}

inline void
system_error(int syserr)
{
#if defined(_WIN32)
	::SetLastError(syserr);
#else
	errno = syserr;
#endif
}

inline pid_t
getpid(void)
{
#if defined(_WIN32)
	return ::GetCurrentProcessId();
#else
	return ::getpid();
#endif
}

inline tid_t
gettid(void)
{
#if defined(_WIN32)
	return ::GetCurrentThreadId();
#else
	return static_cast<tid_t>(::syscall(SYS_gettid));
#endif
}

const char *syserr(char *, size_t, int);
const char *basename(char *, size_t, const char *, bool stripExt = false);
std::string trim(const std::string &);

} // namespace snf

#endif // _SNF_COMMON_H_
