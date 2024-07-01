import numpy as np

class Problem:
    def __init__(self, lenx, alpha, ic, bc0, bc1):
        self.lenx = lenx        # Length of the spatial domain
        self.alpha = alpha      # Thermal diffusivity constant
        self.ic = ic            # Initial condition (temperature distribution)
        self.bc0 = bc0          # Boundary condition at the left end
        self.bc1 = bc1          # Boundary condition at the right end

    def solve(self, alg, maxt, dx, dt, precision):
        return Solution(self, alg, maxt, dx, dt, precision)


class Solution:
    def __init__(self, problem, alg, maxt, dx, dt, precision):
        self.problem = problem  # Reference to the Problem instance
        self.alg = alg          # Algorithm to use for solving
        self.maxt = maxt        # Maximum time for the simulation
        self.dx = dx            # Spatial step size
        self.dt = dt            # Time step size
        self.precision = precision  # Precision of computation

    def run(self):
        lenx = self.problem.lenx
        dx = self.dx
        dt = self.dt
        alpha = self.problem.alpha
        nx = int(lenx / dx)     # Number of spatial points
        nt = int(self.maxt / dt) # Number of time steps
        
        # Initialize the temperature array
        u = np.zeros(nx)
        u_new = np.zeros(nx)
        
        # Apply initial condition
        u[:] = self.problem.ic
        
        # Time-stepping loop
        for n in range(nt):
            for i in range(1, nx-1):
                u_new[i] = u[i] + alpha * dt / dx**2 * (u[i+1] - 2*u[i] + u[i-1])
            u[:] = u_new[:]
        
        return u
