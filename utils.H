// Utilities
#pragma once

#include "args.H"

enum class ArrType { STEP, EXACT, TSTART, TFINAL, RESIDUAL, ERROR };

Number l2_norm(int n, Number const *a, Number const *b);

void copy(int n, Number *dst, Number const *src);

void write_array(ArrType t, Args &arg, int n, Number dx, Number const *a, int time=0);

void set_initial_condition(Args &arg, int n, Number *a);
