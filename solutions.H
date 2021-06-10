// Declarations for alternative numerical methods to accomplish a time-step.

#pragma once

bool
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

bool
update_solution_upwind15(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

bool
update_solution_crankn(int n,
    Number *curr, Number const *back1,
    Number const *cn_Amat,
    Number bc_0, Number bc_1);

bool
update_solution_dufrank(int n, Number *curr,
    Number const *back1, Number const *back2,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);
