#include "heat.H"

bool
update_solution_upwind15(int n, Number *curr, Number const *last,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1)
{
    Number const f2 = 1.0/24;
    Number const f1 = 1.0/6;
    Number const f0 = 1.0/4;
    Number const k = alpha * alpha * dt / (dx * dx);
    Number const k2 = k*k;

    int i;
    curr[0  ] = bc_0;
    curr[1  ] = last[1  ] + k * (last[0  ] - 2 * last[1  ] + last[2  ]);
    curr[n-2] = last[n-2] + k * (last[n-3] - 2 * last[n-2] + last[n-1]);
    curr[n-1] = bc_1;
    for (i = 2; i < n-2; i++)
        curr[i] =  f2*(12*k2  -2*k    )*last[i-2]
                  +f2*(12*k2  -2*k    )*last[i+2]
                  -f1*(12*k2  -8*k    )*last[i-1]
                  -f1*(12*k2  -8*k    )*last[i+1]
                  +f0*(12*k2 -10*k  +4)*last[i  ];

    return true;
}
