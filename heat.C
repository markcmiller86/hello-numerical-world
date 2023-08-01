#include <cmath>

#include "heat.H"

// Number class' statics
#ifndef _OPENMP
int         Number::nadds  = 0;
int         Number::nmults = 0;
int         Number::ndivs  = 0;
std::size_t Number::nbytes = 0;
#endif

// Command-line argument variables
int noout        = 0;
int savi         = 0;
int outi         = 100;
int save         = 0;
int nt           = 0; // number of parallel tasks
char const *runame = "heat_results";
char const *alg  = "ftcs";
char const *ic   = "const(1)";
Number lenx      = Number(1.0);
Number alpha     = Number(0.2);
Number dt        = Number(0.004);
Number dx        = Number(0.1);
Number bc0       = Number(0.0);
Number bc1       = Number(1.0);
Number maxt      = Number(2.0);
Number min_change = Number(1e-8*1e-8);

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

// Utilities
extern Number
l2_norm(int n, Number const *a, Number const *b);

extern void
copy(int n, Number *dst, Number const *src);

extern void
write_array(int t, int n, Number dx, Number const *a);

extern void
set_initial_condition(int n, Number *a, Number dx, char const *ic);

extern void
initialize_crankn(int n,
    Number alpha, Number dx, Number dt,
    Number **_cn_Amat);

extern void
process_args(int argc, char **argv);

extern void 
compute_exact_steady_state_solution(int n, Number *a, Number dx, char const *ic,
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

extern double getWallTimeUsec();
void updateAvg(double);
extern double getAvg();

static void
initialize(void)
{
    Nx = (int) round((double)(lenx/dx))+1;
    Nt = (int) (maxt/dt);
    dx = lenx/(Nx-1);

    curr  = new Number[Nx]();
    back1 = new Number[Nx]();
    if (save)
    {
        exact = new Number[Nx]();
        change_history = new Number[Nx]();
        error_history = new Number[Nx]();
    }

    assert(strncmp(alg, "ftcs", 4)==0 ||
           strncmp(alg, "dufrank", 7)==0 ||
           strncmp(alg, "crankn", 6)==0);

#ifdef HAVE_FEENABLEEXCEPT
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#endif

#ifdef _OPENMP
    if (nt > 1)
        omp_set_num_threads(nt);
    else
        omp_set_num_threads(1);
#endif

    if (!strncmp(alg, "crankn", 6))
        initialize_crankn(Nx, alpha, dx, dt, &cn_Amat);

    if (!strncmp(alg, "dufrank", 7))
        back2 = new Number[Nx]();

    // Initial condition
    set_initial_condition(Nx, back1, dx, ic);
}

int finalize(int ti, Number maxt, Number change)
{
    int retval = 0;

    write_array(TFINAL, Nx, dx, curr);
    if (save)
    {
        write_array(RESIDUAL, ti, dt, change_history);
        write_array(ERROR, ti, dt, error_history);
    }

    if (outi)
    {
        printf("Iteration %04d: last change l2=%g\n", ti, (double) change);
#ifndef _OPENMP
        printf("Counts: %s\n", Number::counts_string());
#endif
    }

    delete [] curr;
    delete [] back1;
    if (back2) delete [] back2;
    if (exact) delete [] exact;
    if (change_history) delete [] change_history;
    if (error_history) delete [] error_history;
    if (cn_Amat) delete [] cn_Amat;
    if (strncmp(alg, "ftcs", 4)) free((void*)alg);
    if (strncmp(ic, "const(1)", 8)) free((void*)ic);

    return retval;
}

static bool
update_solution()
{
    if (!strcmp(alg, "ftcs"))
        return update_solution_ftcs(Nx, curr, back1, alpha, dx, dt, bc0, bc1);
    else if (!strcmp(alg, "crankn"))
        return update_solution_crankn(Nx, curr, back1, cn_Amat, bc0, bc1);
    else if (!strcmp(alg, "dufrank"))
        return update_solution_dufrank(Nx, curr, back1, back2, alpha, dx, dt, bc0, bc1);
    return false;
}

static Number
update_output_files(int ti)
{
    Number change;

    if (ti>0 && save)
    {
        compute_exact_steady_state_solution(Nx, exact, dx, ic, alpha, ti*dt, bc0, bc1);
        if (savi && ti%savi==0)
            write_array(ti, Nx, dx, exact);
    }

    if (ti>0 && savi && ti%savi==0)
        write_array(ti, Nx, dx, curr);

    change = l2_norm(Nx, curr, back1);
    if (save)
    {
        change_history[ti] = change;
        error_history[ti] = l2_norm(Nx, curr, exact);
    }

    return change;
}

int main(int argc, char **argv)
{
    int ti;
    double t1, t2, tdiff;
    Number change;

    // Read command-line args and set values
    process_args(argc, argv);

    // Allocate arrays and set initial conditions
    initialize();

    // Iterate to max iterations or solution change is below threshold
    t1 = getWallTimeUsec();
    for (ti = 0; ti*dt < maxt; ti++)
    {
        // compute the next solution step
        if (!update_solution())
        {
            fprintf(stderr, "Solution criteria violated. Make better choices\n");
            exit(1);
        }

        // compute amount of change in solution
        change = update_output_files(ti);

        // Handle possible termination by change threshold
        if (maxt == INT_MAX && change < min_change)
        {
            printf("Stopped after %06d iterations for threshold %g\n",
                ti, (double) change);
            break;
        }

        // Output progress
        if (outi && ti%outi==0)
            printf("Iteration %04d: last change l2=%g\n", ti, (double) change);

        // Copy current solution to backi
        if (back2)
            copy(Nx, back2, back1);
        copy(Nx, back1, curr);

    }
    t2 = getWallTimeUsec();
    printf("Elapsed time = %8.16g msec\n\n", (t2 - t1) / 1000.0);

    // Delete storage and output final results
    return finalize(ti, maxt, change);
}
