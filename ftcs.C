#include "heat.H"

bool                          // false if unstable, true otherwise
update_solution_ftcs(
    int n,                    // number of samples
    Double *uk1,              // new array of u(x,k+1) to compute/return
    Double const *uk0,        // old/last array u(x,k) of samples computed
    Double alpha,             // thermal diffusivity
    Double dx, Double dt,     // spacing in space, x, and time, t.
    Double bc0, Double bc1)   // boundary conditions @ x=0 & x=Lx
{
    Double r = alpha * dt / (dx * dx);

    // sanity check for stability
    if (r > 0.5) return false; 

    // FTCS update algorithm
    for (int i = 1; i < n-1; i++)
        uk1[i] = r*uk0[i+1] + (1-2*r)*uk0[i] + r*uk0[i-1];

    // enforce boundary conditions
    uk1[0  ] = bc0;
    uk1[n-1] = bc1;

    return true;
}
