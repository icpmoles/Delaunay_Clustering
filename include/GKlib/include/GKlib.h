/*
 * GKlib.h
 *
 * George's library of most frequently used routines
 *
 * $Id: GKlib.h 14866 2013-08-03 16:40:04Z karypis $
 *
 */

#ifndef _GKLIB_H_
#define _GKLIB_H_ 1

#define GKMSPACE

#if defined(_MSC_VER)
#define __MSC__
#endif
#if defined(__ICC)
#define __ICC__
#endif


#include "gk_arch.h" /*!< This should be here, prior to the includes */


/*************************************************************************
 * Header file inclusion section
 **************************************************************************/
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if defined(USE_PCRE) && defined(HAVE_PCREPOSIX_H)
#include <pcreposix.h>
#elif defined(HAVE_REGEX_H)
#include <regex.h>
#else
#include "gkregex.h"
#endif


#if defined(__OPENMP__)
#include <omp.h>
#endif


#include <gk_defs.h>
#include <gk_externs.h>
#include <gk_getopt.h>
#include <gk_macros.h>
#include <gk_struct.h>
#include <gk_types.h>

#include <gk_mkblas.h>
#include <gk_mkmemory.h>
#include <gk_mkpqueue.h>
#include <gk_mkpqueue2.h>
#include <gk_mkrandom.h>
#include <gk_mksort.h>
#include <gk_mkutils.h>

#include <gk_proto.h>


#endif /* GKlib.h */
