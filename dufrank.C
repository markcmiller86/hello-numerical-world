#include "heat.H"

bool                        // false if unstable, true otherwise
update_solution_dufrank(
    int n,                  // number of samples
    Number *uk,             // new array of u(x,k) to compute/return
    Number const *uk1,      // array u(x,k-1) computed @ -1 time index ago
    Number const *uk2,      // array u(x,k-2) computed @ -2 time index ago
    Number alpha,           // thermal diffusivity
    Number dx, Number dt,   // spacing in space, x, and time, t.
    Number bc0, Number bc1) // boundary conditions @ x=0 & x=Lx
{
    Number r = alpha * dt / (dx * dx);
    Number q = castNum(1.0) / (castNum(1.0)+r);

    // FTCS update algorithm
    for (int i = 1; i < n-1; i++)
        uk[i] = q * (1-r) * uk2[i] + q * r * (uk1[i+1] + uk1[i-1]);

    // enforce boundary conditions
    uk[0  ] = bc0;
    uk[n-1] = bc1;

    return true;
}
