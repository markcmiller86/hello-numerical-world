#include "heat.H"

// Licensing: This code is distributed under the GNU LGPL license. 
// Modified: 30 May 2009 Author: John Burkardt
// Modified by Mark C. Miller, July 23, 2017
static void
r83_np_fa(Vector &a)
{
    int n = a.size()/3;
    int i;

    for ( i = 1; i <= n-1; i++ )
    {
        assert ( a[1+(i-1)*3] != 0.0 );

        // Store the multiplier in L.
        a[2+(i-1)*3] = a[2+(i-1)*3] / a[1+(i-1)*3];

        // Modify the diagonal entry in the next column.
        a[1+i*3] = a[1+i*3] - a[2+(i-1)*3] * a[0+i*3];
    }

    assert( a[1+(n-1)*3] != 0.0 );
}

Vector
initialize_crankn(int n,
    Number alpha, Number dx, Number dt)
{
    int i;
    Number const w = alpha * dt / dx / dx;

    // Build a tri-diagonal matrix
    Vector cn_Amat(3*n);

    cn_Amat[0+0*3] = 0.0;
    cn_Amat[1+0*3] = 1.0;
    cn_Amat[0+1*3] = 0.0;

    for ( i = 1; i < n - 1; i++ )
    {
        cn_Amat[2+(i-1)*3] =           - w;
        cn_Amat[1+ i   *3] = 1.0 + 2.0 * w;
        cn_Amat[0+(i+1)*3] =           - w;
    }
        
    cn_Amat[2+(n-2)*3] = 0.0;
    cn_Amat[1+(n-1)*3] = 1.0;
    cn_Amat[2+(n-1)*3] = 0.0;

    // Factor the matrix.
    r83_np_fa(cn_Amat);

    return cn_Amat;
}

// Licensing: This code is distributed under the GNU LGPL license. 
// Modified: 30 May 2009 Author: John Burkardt
// Modified by Mark C. Miller, miller86@llnl.gov, July 23, 2017
static void 
r83_np_sl ( int n, Number const *a_lu, Number const *b, Number *x)
{
    int i;

    for ( i = 0; i < n; i++ )
        x[i] = b[i];

    // Solve L * Y = B.
    for ( i = 1; i < n; i++ )
        x[i] = x[i] - a_lu[2+(i-1)*3] * x[i-1];

    // Solve U * X = Y.
    for ( i = n; 1 <= i; i-- )
    {
        x[i-1] = x[i-1] / a_lu[1+(i-1)*3];
        if ( 1 < i )
            x[i-2] = x[i-2] - a_lu[0+(i-1)*3] * x[i-1];
    }
}

bool
update_solution_crankn(int n,
    Number *curr, Number const *last,
    Number const *cn_Amat,
    Number bc_0, Number bc_1)
{
    // Do the solve
    r83_np_sl (n, cn_Amat, last, curr);
    curr[0] = bc_0;
    curr[n-1] = bc_1;

    return true;
}
