#include <cmath>

#include "heat.H"

// Various arrays of numerical data
Number *curr           = 0; // current solution
Number *back1          = 0; // solution back 1 step
Number *back2          = 0; // solution back 2 steps
Number *exact          = 0; // exact solution (when available)
Number *change_history = 0; // solution l2norm change history
Number *error_history  = 0; // solution error history (when available)
Number *cn_Amat        = 0; // A matrix for Crank-Nicholson

// Number of points in space, x, and time, t.
int Nx;
int Nt;

extern void
initialize_crankn(int n,
    Number alpha, Number dx, Number dt,
    Number **_cn_Amat);

extern void 
compute_exact_solution(int n, Number *a, Number dx, char const *ic,
    Number alpha, Number t, Number bc0, Number bc1);

extern bool
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

extern bool
update_solution_upwind15(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

extern bool
update_solution_crankn(int n,
    Number *curr, Number const *back1,
    Number const *cn_Amat,
    Number bc_0, Number bc_1);

extern bool
update_solution_dufrank(int n, Number *curr,
    Number const *back1, Number const *back2,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

static void
initialize(Args &arg)
{
    Nx = (int) round((double)(arg.lenx/arg.dx))+1;
    Nt = (int) (arg.maxt/arg.dt);
    arg.dx = arg.lenx/(Nx-1);

    curr  = new Number[Nx]();
    back1 = new Number[Nx]();
    if (arg.save)
    {
        exact = new Number[Nx]();
        change_history = new Number[Nx]();
        error_history = new Number[Nx]();
    }

    assert(strncmp(arg.alg, "ftcs", 4)==0 ||
           strncmp(arg.alg, "dufrank", 7)==0 ||
           strncmp(arg.alg, "crankn", 6)==0);

#ifdef HAVE_FEENABLEEXCEPT
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#endif

    if (!strncmp(arg.alg, "crankn", 6))
        initialize_crankn(Nx, arg.alpha, arg.dx, arg.dt, &cn_Amat);

    if (!strncmp(arg.alg, "dufrank", 7))
        back2 = new Number[Nx]();

    /* Initial condition */
    set_initial_condition(arg, Nx, back1);
}

int finalize(Args &arg, int ti, Number maxt, Number change)
{
    int retval = 0;

    write_array(ArrType::TFINAL, arg, Nx, arg.dx, curr);
    if (arg.save)
    {
        write_array(ArrType::RESIDUAL, arg, ti, arg.dt, change_history);
        write_array(ArrType::ERROR, arg, ti, arg.dt, error_history);
    }

    if (arg.outi)
    {
        printf("Iteration %04d: last change l2=%g\n", ti, (double) change);
    }

    delete [] curr;
    delete [] back1;
    if (back2) delete [] back2;
    if (exact) delete [] exact;
    if (change_history) delete [] change_history;
    if (error_history) delete [] error_history;
    if (cn_Amat) delete [] cn_Amat;
    if (strncmp(arg.alg, "ftcs", 4)) free((void*)arg.alg);
    if (strncmp(arg.ic, "const(1)", 8)) free((void*)arg.ic);

    return retval;
}

static bool
update_solution(Args &arg)
{
    if (!strcmp(arg.alg, "ftcs"))
        return update_solution_ftcs(Nx, curr, back1, arg.alpha, arg.dx, arg.dt, arg.bc0, arg.bc1);
    else if (!strcmp(arg.alg, "crankn"))
        return update_solution_crankn(Nx, curr, back1, cn_Amat, arg.bc0, arg.bc1);
    else if (!strcmp(arg.alg, "dufrank"))
        return update_solution_dufrank(Nx, curr, back1, back2, arg.alpha, arg.dx, arg.dt, arg.bc0, arg.bc1);
    return false;
}

static Number
update_output_files(Args &arg, int ti)
{
    Number change;

    if (ti>0 && arg.save)
    {
        compute_exact_solution(Nx, exact, arg.dx, arg.ic, arg.alpha, castNum(ti*arg.dt), arg.bc0, arg.bc1);
        if (arg.savi && ti%arg.savi==0)
            write_array(ArrType::EXACT, arg, Nx, arg.dx, exact, ti);
    }

    if (ti>0 && arg.savi && ti%arg.savi==0)
        write_array(ArrType::STEP, arg, Nx, arg.dx, curr, ti);

    change = l2_norm(Nx, curr, back1);
    if (arg.save)
    {
        change_history[ti] = change;
        error_history[ti] = l2_norm(Nx, curr, exact);
    }

    return change;
}

int main(int argc, char **argv)
{
    int ti;
    Number change;

    // Read command-line args and set values
    Args arg = process_args(argc, argv);

    // Allocate arrays and set initial conditions
    initialize(arg);

    // Iterate to max iterations or solution change is below threshold
    for (ti = 0; ti*arg.dt < arg.maxt; ti++)
    {
        // compute the next solution step
        if (!update_solution(arg))
        {
            fprintf(stderr, "Solution criteria violated. Make better choices\n");
            exit(1);
        }

        // compute amount of change in solution
        change = update_output_files(arg, ti);

        // Handle possible termination by change threshold
        if (arg.maxt == INT_MAX && change < arg.min_change)
        {
            printf("Stopped after %06d iterations for threshold %g\n",
                ti, (double) change);
            break;
        }

        // Output progress
        if (arg.outi && ti%arg.outi==0)
            printf("Iteration %04d: last change l2=%g\n", ti, (double) change);

        // Copy current solution to backi
        if (back2)
            copy(Nx, back2, back1);
        copy(Nx, back1, curr);

    }

    // Delete storage and output final results
    return finalize(arg, ti, arg.maxt, change);
}
