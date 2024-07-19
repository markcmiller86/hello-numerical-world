#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <assert.h>
#ifdef HAVE_FEENABLEEXCEPT
#define _GNU_SOURCE
#include <cfenv>
#endif
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _OPENMP
#include <omp.h>
#endif

// Lets overlook the fact that we're including one
// header inside of another just for convenience
#include "Number.h"
