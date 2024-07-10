import math

def sin_initial_equation(x, t, A, k, alpha):
    """
    Compute the value of u(x, t) for sinusoidal initial conditions.

    Parameters:
    x (float): Spatial coordinate
    t (float): Time coordinate
    A (float): Amplitude of the sinusoidal function
    k (float): Wave number
    alpha (float): Diffusion coefficient

    Returns:
    float: Value of u at (x, t)
    """
    return A * math.sin(k * x) * math.exp(-alpha * k**2 * t)