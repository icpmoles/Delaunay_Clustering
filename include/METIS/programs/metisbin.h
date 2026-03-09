/*
 * metisbin.h
 *
 * This file contains the various header inclusions
 *
 * Started 8/9/02
 * George
 */

#include <GKlib.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#if defined(ENABLE_OPENMP)
#include <omp.h>
#endif


#include <metis.h>
#include "../libmetis/defs.h"
#include "../libmetis/gklib_defs.h"
#include "../libmetis/macros.h"
#include "../libmetis/proto.h"
#include "../libmetis/rename.h"
#include "../libmetis/struct.h"
#include "defs.h"
#include "proto.h"
#include "struct.h"


#if defined(COMPILER_GCC)
extern char* strdup(const char*);
#endif

#define SVNINFO "unknown"

#if defined(COMPILER_MSC)
#if defined(rint)
#undef rint
#endif
#define rint(x) ((idx_t)((x) + 0.5)) /* MSC does not have rint() function */
#define __func__ "dummy-function"
#endif
