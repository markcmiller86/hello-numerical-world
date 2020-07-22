#include "heat.H"

void 
compute_exact_solution(int n, Number *a, Number dx, char const *ic,
    Number alpha, Number t, Number bc0, Number bc1)
{
    int i;
    double x;
    
    // For any time t for Sin(Pi*x) initial condition
    // and zero boundary condition
    if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "sin(Pi*x)", 9))
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = sin(M_PI*x)*exp((double)-alpha*M_PI*M_PI*(double)t);
    }
    // For any time t for constant initial condition
    // and zero boundary condition
    else if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "const(", 6))
    {
        double cval = strtod(ic+6, 0);
        for (i = 0, x = 0; i < n; i++, x+=dx)
        {
            int n;
            double fsum = 0;

            // Brute force sum first 1000 terms of Fourier series
            for (n = 1; n < 1000; n++)
            {
                double coeff = 2*cval*(1-pow(-1.0,(double)n))/(n*M_PI);
                double func = sin(n*M_PI*x)*exp(((double)-alpha)*n*n*M_PI*M_PI*((double)t));
                fsum += coeff * func;
            }
            a[i] = fsum;
        }
    }
    // For t>>0 (steady state) for any initial and boundary conditions
    else
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = bc0 + (bc1-bc0)*x;
    }
}
