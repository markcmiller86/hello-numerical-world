// Utilities
#pragma once

#include "args.H"

enum class ArrType { STEP, EXACT, TSTART, TFINAL, RESIDUAL, ERROR };

Number l2_norm(int n, Number const *a, Number const *b);

void copy(Vector &dst, const Vector &src);

void write_array(ArrType t, Args &arg, Number dx, const Vector &a, int time=0);

void set_initial_condition(Args &arg, Vector &a);
