#!/usr/bin/env python3
import math
import sys

def sin_initial_equation(x, dt, A, B, alpha):
    """
    Compute the value of u(x, t) for sinusoidal initial conditions.

    Parameters:
    x (float): Spatial coordinate
    t (float): Time coordinate
    A (float): Amplitude of the sinusoidal function
    B (float): Frequency multiplier
    alpha (float): Diffusion coefficient

    Returns:
    float: Value of u at (x, t)
    """
    t = dt * 100
    # Calculate wave number k based on frequency multiplier B
    k = math.pi * B

    # Compute the sinusoidal function value with diffusion over time
    return A * math.sin(k * x) * math.exp(-alpha * k**2 * t)

#./heat dx=0.01 dt=0.00004 alpha=0.2 ic="sin(10,2)"
# Example usage
# A = 10 sys.argv
# B = 2 sys.argv

rdfile = sys.argv[1]
dx = float(sys.argv[2])
dt = float(sys.argv[3])
alpha = float(sys.argv[4])
A,B = float(sys.argv[5]), float(sys.argv[6])

def main(rdfile, dx, dt, A, B, alpha):
    errbnd = 1e-3
    comparison_arr = []
    x = 0 # constant
    relerr = False

    while x < 1:
        y = sin_initial_equation(x, dt, A, B, alpha)
        comparison_arr.append((x, y))
        print(x,y)
        x += dx
        

    if len(sys.argv) < 2:
        print("Specify input file as 1st argument")
        sys.exit(1)

    rdfile = sys.argv[1]
    try:
        with open(rdfile, 'r') as file:
            index = 0
            for line in file:
                if '#' in line:
                    continue
                if index < len(comparison_arr):
                    parts = line.split()
                    if len(parts) < 2:
                        continue

                    xval = float(parts[0])
                    yval = float(parts[1])
                    comp_y = comparison_arr[index][1]
                
                    if relerr:
                        diffr = abs((yval - comp_y) / comp_y)
                        diffl = diffr <= errbnd
                    else:
                        diffr = abs(yval - comp_y)
                        diffl = diffr <= errbnd

                    if not diffl:
                        print(f"Check failed at x={xval} y={yval} yexp={comp_y}, diff={diffr}")
                        sys.exit(1)
                break

        print("All checks passed successfully.")
        sys.exit(0)
    except FileNotFoundError:
        print(f"File not found: {rdfile}")
        sys.exit(1)

if __name__ == "__main__":
    main(rdfile, dx, dt, A, B, alpha)