#include "heat.H"

void 
compute_exact_solution(int n, Double *a, Double dx, char const *ic,
    Double alpha, Double t, Double bc0, Double bc1)
{
    int i;
    Double x;
    
    if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "sin(Pi*x)", 9))
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = sin(M_PI*x)*exp(-alpha*M_PI*M_PI*t);
    }
    else if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "const(", 6))
    {
        Double cval = strtod(ic+6, 0);
        for (i = 0, x = 0; i < n; i++, x+=dx)
        {
            int n;
            Double fsum = 0;

            // sum first 200 terms of Fourier series
            for (n = 1; n < 200; n++)
            {
                Double coeff = 2*cval*(1-pow(-1.0,(double)n))/(n*M_PI);
                Double func = sin(n*M_PI*x)*exp(((double)-alpha)*n*n*M_PI*M_PI*((double)t));
                fsum += coeff * func;
            }
            a[i] = fsum;
        }
    }
    else // can only compute final steady state solution
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = bc0 + (bc1-bc0)*x;
    }
}
