/* vi: set sw=4 ts=4: */
/*
 * Safe gethostname implementation for busybox
 *
 * Copyright (C) 2008 Tito Ragusa <farmatito@tiscali.it>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

/*
 * SUSv2 guarantees that "Host names are limited to 255 bytes"
 * POSIX.1-2001 guarantees that "Host names (not including the terminating
 * null byte) are limited to HOST_NAME_MAX bytes" (64 bytes on my box).
 *
 * RFC1123 says:
 *
 * The syntax of a legal Internet host name was specified in RFC-952
 * [DNS:4].  One aspect of host name syntax is hereby changed: the
 * restriction on the first character is relaxed to allow either a
 * letter or a digit.  Host software MUST support this more liberal
 * syntax.
 *
 * Host software MUST handle host names of up to 63 characters and
 * SHOULD handle host names of up to 255 characters.
 */

#include "libbb.h"
#if defined(__linux__)
#include <sys/utsname.h>
#endif

/*
 * On success return the current malloced and NUL terminated hostname.
 * On error return malloced and NUL terminated string "?".
 * This is an illegal first character for a hostname.
 * The returned malloced string must be freed by the caller.
 */
char* FAST_FUNC safe_gethostname(void)
{
#if defined(__linux__)
	struct utsname uts;

	/* The length of the arrays in a struct utsname is unspecified;
	 * the fields are terminated by a null byte.
	 * Note that there is no standard that says that the hostname
	 * set by sethostname(2) is the same string as the nodename field of the
	 * struct returned by uname (indeed, some systems allow a 256-byte host-
	 * name and an 8-byte nodename), but this is true on Linux. The same holds
	 * for setdomainname(2) and the domainname field.
	 */

	/* Uname can fail only if you pass a bad pointer to it. */
	uname(&uts);
	return xstrndup(!uts.nodename[0] ? "?" : uts.nodename, sizeof(uts.nodename));
#else
	/* We really don't care about people with host names wider than most screens */
	char buf[256];
	int r = gethostname(buf, sizeof(buf));
	buf[sizeof(buf)-1] = '\0';
	return xstrdup(r < 0 ? "?" : buf);
#endif
}

/*
 * On success return the current malloced and NUL terminated domainname.
 * On error return malloced and NUL terminated string "?".
 * This is an illegal first character for a domainname.
 * The returned malloced string must be freed by the caller.
 */
char* FAST_FUNC safe_getdomainname(void)
{
#if defined(__linux__)
/* The field domainname of struct utsname is Linux specific. */
	struct utsname uts;
	uname(&uts);
	return xstrndup(!uts.domainname[0] ? "?" : uts.domainname, sizeof(uts.domainname));
#else
	/* We really don't care about people with domain names wider than most screens */
	/*
	char buf[256];
	int r = getdomainname(buf, sizeof(buf));
	buf[sizeof(buf)-1] = '\0';
	return xstrdup(r < 0 ? "?" : buf);
	*/
	return xstrdup("?");
#endif
}
