import math
 
def exact_solution_impulse(alpha, A, x0, x, t):
    """
    Computes the temperature distribution of a 1D heat equation at time t
    for an initial impulse of amplitude A at position x0, using vanilla Python.
 
    Parameters:
    - x: position at which to evaluate the temperature distribution.
    - t: time at which the temperature is evaluated.
    - A: amplitude of the initial heat impulse.
    - x0: position of the initial heat impulse.
    - alpha: thermal diffusivity of the medium.
 
    Returns:
    - u: temperature at position x and time t.
    """
    if t == 0:
        # Return an ideal impulse at t=0, which is theoretically infinite at x0 and 0 elsewhere.
        return A if x == x0 else 0.0
    else:
        # Calculate the Gaussian spread for t > 0
        sqrt_term = math.sqrt(4.0 * math.pi * alpha * t)
        exp_term = -((x - x0)**2.0 / (4.0 * alpha * t))
        exp_term = math.exp(exp_term)
        return A * exp_term / sqrt_term
 
# Example usage for a all points, x
 
# set parameters
dt = 0.00004
t = dt * 100 # at time t=0.004
#A = 7000 / 9.973557010035817     # amplitude of the impulse
A = 1
alpha = 0.2 # thermal diffusivity of wood
lenx = 1.0  # width of wall
dx = 0.01   # spacing in x
x0 = 50*dx  # impulse located at middle
 
#
# Loop to produce “solution” for all x and given t
#
x = 0
while x < lenx:
    y = exact_solution_impulse(alpha, A, x0, x, t)
    print(x, y)
    x += dx
