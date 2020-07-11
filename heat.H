#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#ifdef HAVE_FEENABLEEXCEPT
#define _GNU_SOURCE
#include <cfenv>
#endif
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sstream>

#include "Double.H"
