#include <cmath>

#include "heat.H"

extern void
initialize_crankn(int n,
    Number alpha, Number dx, Number dt,
    Number **_cn_Amat);

extern void 
compute_exact_solution(int n, Number *a, Number dx, char const *ic,
    Number alpha, Number t, Number bc0, Number bc1);

struct State {
    Vector curr           ; // current solution
    Vector back1          ; // solution back 1 step
    Vector back2          ; // solution back 2 steps
    Vector exact          ; // exact solution (when available)
    Vector change_history ; // solution l2norm change history
    Vector error_history  ; // solution error history (when available)

    Number *cn_Amat       ; // A matrix for Crank-Nicholson

    State(Args &arg)
        : curr(arg.Nx)
        , back1(arg.Nx)
        , back2(strncmp(arg.alg, "dufrank", 7)==0 ? arg.Nx : 0)
        , exact(arg.save ? arg.Nx : 0)
        , change_history((arg.save && arg.Nt != INT_MAX) ? arg.Nt : 0)
        , error_history((arg.save && arg.Nt != INT_MAX) ? arg.Nt : 0)
        , cn_Amat(nullptr)
        {
        assert(strncmp(arg.alg, "ftcs", 4)==0 ||
               strncmp(arg.alg, "dufrank", 7)==0 ||
               strncmp(arg.alg, "crankn", 6)==0);

#ifdef HAVE_FEENABLEEXCEPT
        feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#endif

        if (!strncmp(arg.alg, "crankn", 6))
            initialize_crankn(arg.Nx, arg.alpha, arg.dx, arg.dt, &cn_Amat);

        /* Initial condition */
        set_initial_condition(arg, back1);
    }
    ~State() {
        if (cn_Amat != nullptr) delete [] cn_Amat;
    }
};

int finalize(Args &arg, State &s, int ti, Number maxt, Number change)
{
    int retval = 0;

    write_array(ArrType::TFINAL, arg, arg.dx, s.curr);
    if (arg.save)
    {
        write_array(ArrType::RESIDUAL, arg, arg.dt, s.change_history, ti);
        write_array(ArrType::ERROR, arg, arg.dt, s.error_history, ti);
    }

    if (arg.outi)
    {
        printf("Iteration %04d: last change l2=%g\n", ti, (double) change);
    }

    // FIXME: this free-s an unallocated pointer if the user enters ftcs, etc.
    if (strncmp(arg.alg, "ftcs", 4)) free((void*)arg.alg);
    if (strncmp(arg.ic, "const(1)", 8)) free((void*)arg.ic);

    return retval;
}

static bool
update_solution(Args &arg, State &s)
{
    if (!strcmp(arg.alg, "ftcs"))
        return update_solution_ftcs(arg.Nx, s.curr.data(), s.back1.data(),
                                    arg.alpha, arg.dx, arg.dt, arg.bc0, arg.bc1);
    else if (!strcmp(arg.alg, "crankn"))
        return update_solution_crankn(arg.Nx, s.curr.data(), s.back1.data(),
                                      s.cn_Amat, arg.bc0, arg.bc1);
    else if (!strcmp(arg.alg, "dufrank"))
        return update_solution_dufrank(arg.Nx, s.curr.data(), s.back1.data(),
                            s.back2.data(), arg.alpha,
                            arg.dx, arg.dt, arg.bc0, arg.bc1);
    return false;
}

static Number
update_output_files(Args &arg, State &s, int ti)
{
    Number change;

    if (ti>0 && arg.save)
    {
        compute_exact_solution(arg.Nx, s.exact.data(), arg.dx,
                        arg.ic, arg.alpha, castNum(ti*arg.dt), arg.bc0, arg.bc1);
        if (arg.savi && ti%arg.savi==0)
            write_array(ArrType::EXACT, arg, arg.dx, s.exact, ti);
    }

    if (ti>0 && arg.savi && ti%arg.savi==0)
        write_array(ArrType::STEP, arg, arg.dx, s.curr, ti);

    change = l2_norm(arg.Nx, s.curr.data(), s.back1.data());
    if (arg.save && ti < s.error_history.size())
    {
        s.change_history[ti] = change;
        s.error_history[ti] = l2_norm(arg.Nx, s.curr.data(), s.exact.data());
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
    State s(arg);

    // Iterate to max iterations or solution change is below threshold
    for (ti = 0; ti*arg.dt < arg.maxt; ti++)
    {
        // compute the next solution step
        if (!update_solution(arg, s))
        {
            fprintf(stderr, "Solution criteria violated. Make better choices\n");
            exit(1);
        }

        // compute amount of change in solution
        change = update_output_files(arg, s, ti);

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
        if (s.back2.size() > 0)
            copy(s.back2, s.back1);
        copy(s.back1, s.curr);

    }

    // Delete storage and output final results
    return finalize(arg, s, ti, arg.maxt, change);
}
