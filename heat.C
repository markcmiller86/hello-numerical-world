#include "heat.H"

// Double class' statics
int         Double::nadds  = 0;
int         Double::nmults = 0;
int         Double::ndivs  = 0;
std::size_t Double::nbytes = 0;

// Command-line argument variables
int noout        = 0;
int savi         = 0;
int outi         = 100;
int save         = 0;
char const *runame = "heat_results";
char const *alg  = "ftcs";
char const *prec = "double";
char const *ic   = "const(1)";
Double lenx      = 1.0;
Double alpha     = 0.2;
Double dt        = 0.004;
Double dx        = 0.1;
Double bc0       = 0;
Double bc1       = 1;
Double maxt      = 2.0;
Double min_change = 1e-8*1e-8;

// Various arrays of numerical data
Double *curr           = 0; // current solution
Double *last           = 0; // last solution
Double *exact          = 0; // exact solution (when available)
Double *change_history = 0; // solution l2norm change history
Double *error_history  = 0; // solution error history (when available)
Double *cn_Amat        = 0; // A matrix for Crank-Nicholson

// Number of points in space, x, and time, t.
int Nx = (int) (lenx/dx);
int Nt = (int) (maxt/dt);

// Utilities
extern Double
l2_norm(int n, Double const *a, Double const *b);

extern void
copy(int n, Double *dst, Double const *src);

extern void
write_array(int t, int n, Double dx, Double const *a);

extern void
set_initial_condition(int n, Double *a, Double dx, char const *ic);

extern void
initialize_crankn(int n,
    Double alpha, Double dx, Double dt,
    Double **_cn_Amat);

extern void
process_args(int argc, char **argv);

extern void 
compute_exact_solution(int n, Double *a, Double dx, char const *ic,
    Double alpha, Double t, Double bc0, Double bc1);

extern bool
update_solution_ftcs(int n,
    Double *curr, Double const *last,
    Double alpha, Double dx, Double dt,
    Double bc_0, Double bc_1);

extern bool
update_solution_upwind15(int n,
    Double *curr, Double const *last,
    Double alpha, Double dx, Double dt,
    Double bc_0, Double bc_1);

extern bool
update_solution_crankn(int n,
    Double *curr, Double const *last,
    Double const *cn_Amat,
    Double bc_0, Double bc_1);

static void
initialize(void)
{
    Nx = (int) (lenx/dx)+1;
    Nt = (int) (maxt/dt);
    dx = lenx/(Nx-1);

    curr = new Double[Nx]();
    last = new Double[Nx]();
    if (save)
    {
        exact = new Double[Nx]();
        change_history = new Double[Nx]();
        error_history = new Double[Nx]();
    }

    assert(strncmp(alg, "ftcs", 4)==0 ||
           strncmp(alg, "upwind15", 8)==0 ||
           strncmp(alg, "crankn", 6)==0);

#ifdef HAVE_FEENABLEEXCEPT
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#endif

    if (!strncmp(alg, "crankn", 6))
        initialize_crankn(Nx, alpha, dx, dt, &cn_Amat);

    /* Initial condition */
    set_initial_condition(Nx, last, dx, ic);
}

int finalize(int ti, Double maxt, Double change)
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
        printf("Counts: %s\n", Double::counts_string());
    }

    delete [] curr;
    delete [] last;
    if (exact) delete [] exact;
    if (change_history) delete [] change_history;
    if (error_history) delete [] error_history;
    if (cn_Amat) delete [] cn_Amat;
    if (strncmp(alg, "ftcs", 4)) free((void*)alg);
    if (strncmp(prec, "double", 6)) free((void*)prec);
    if (strncmp(ic, "const(1)", 8)) free((void*)ic);

    return retval;
}

static bool
update_solution()
{
    if (!strcmp(alg, "ftcs"))
        return update_solution_ftcs(Nx, curr, last, alpha, dx, dt, bc0, bc1);
    else if (!strcmp(alg, "upwind15"))
        return update_solution_upwind15(Nx, curr, last, alpha, dx, dt, bc0, bc1);
    else if (!strcmp(alg, "crankn"))
        return update_solution_crankn(Nx, curr, last, cn_Amat, bc0, bc1);
    return false;
}

static Double
update_output_files(int ti)
{
    Double change;

    if (ti>0 && save)
    {
        compute_exact_solution(Nx, exact, dx, ic, alpha, ti*dt, bc0, bc1);
        if (savi && ti%savi==0)
            write_array(ti, Nx, dx, exact);
    }

    if (ti>0 && savi && ti%savi==0)
        write_array(ti, Nx, dx, curr);

    change = l2_norm(Nx, curr, last);
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
    Double change;

    // Read command-line args and set values
    process_args(argc, argv);

    // Allocate arrays and set initial conditions
    initialize();

    // Iterate to max iterations or solution change is below threshold
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

        // Copy current solution to last
        copy(Nx, last, curr);
    }

    // Delete storage and output final results
    return finalize(ti, maxt, change);
}
