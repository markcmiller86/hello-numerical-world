#include "heat.h"

int                        // false if unstable, true otherwise
update_solution_ftcs(
    int n,                  // number of samples
    Number *uk,             // new array of u(x,k) to compute/return
    Number const *uk1,      // array u(x,k-1) computed @ -1 time index ago
    Number alpha,           // thermal diffusivity
    Number dx, Number dt,   // spacing in space, x, and time, t.
    Number bc0, Number bc1) // boundary conditions @ x=0 & x=Lx
{
    Number r = alpha * dt / (dx * dx);

    // sanity check for stability
    if (r > 0.5) return 0; 

    // FTCS update algorithm
    #pragma omp parallel for
    for (int i = 1; i < n-1; i++)
        uk[i] = r*uk1[i+1] + (1-2*r)*uk1[i] + r*uk1[i-1];

    // enforce boundary conditions
    uk[0  ] = bc0;
    uk[n-1] = bc1;

    return 1;
}
