#include "heat.h"

void 
compute_exact_steady_state_solution(int n, Number *a, Number dx, char const *ic,
    Number alpha, Number t, Number bc0, Number bc1)
{
    int i;
    double x = 0;
    
    #pragma omp parallel for
    for (i = 0; i < n; i++)
        a[i] = bc0 + (bc1-bc0)*i*dx/(double)n;
}
