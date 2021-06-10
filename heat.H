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

// Lets overlook the fact that we're including one
// header inside of another just for convenience
#include "Half.H"
#include "Number.H"
#include "utils.H"
#include "solutions.H"
