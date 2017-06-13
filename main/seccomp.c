/*
*   Copyright (c) 2017, Google, Inc.
*
*   Author: Han-Wen Nienhuys <hanwen@google.com>
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*
*/

#include "general.h"

#if HAVE_SECCOMP
#include <seccomp.h>
#include <stdio.h>

int installSyscallFilter (void)
{
	// Use SCMP_ACT_TRAP to get a core dump.
	scmp_filter_ctx ctx = seccomp_init (SCMP_ACT_KILL);
	if (ctx == NULL)
	{
		return 1;
	}

	// Memory allocation.
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (mmap), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (brk), 0);

	// I/O
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (read), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (write), 0);

	// Clean exit
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (exit), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (exit_group), 0);

	// The bowels of stdio want to know the size of a file, even for stdout.
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (fstat), 0);

	// seems unnecessary, but this comes from
	// main/parse.c:2764 : tagFilePosition (&tagfpos);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (lseek), 0);

	// TODO: print verbose progress
	int err = seccomp_load (ctx);
	if (err < 0)
	{
		// TODO: use logging framework
		fprintf (stderr, "seccomp_load: %d", err);
	}

	seccomp_release (ctx);

	return err;
}

/*
   TODO: on OSX, Seatbelt
   (https://dev.chromium.org/developers/design-documents/sandbox/osx-sandboxing-design)
   should be used for equivalent functionality.
 */
#endif
