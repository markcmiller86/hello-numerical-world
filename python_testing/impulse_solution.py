#!/usr/bin/env python3
import math
import sys

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
    
rdfile = sys.argv[1]
dx = float(sys.argv[2].split('=')[1])
dt = float(sys.argv[3].split('=')[1])
# bc1 = int(sys.argv[4].split('=')[1])
# ic = sys.argv[5].split('=')[1].strip('"')
# maxt = float(sys.argv[6].split('=')[1])
# outi = int(sys.argv[7].split('=')[1])
# savi = int(sys.argv[8].split('=')[1])
# ./heat runame=check_impulse dx=0.01 dt=0.00004 bc1=0 ic="spikes(0,100,50)" maxt=0.04 outi=100 savi=100
def main(rdfile, dx, dt):
    errbnd = 1e-3
    comparison_arr = []
    x = 0 # constant
    t = dt * 100  # at time t=0.004
    A = 1 # constant
    alpha = 0.2  # thermal diffusivity of wood
    lenx = 1.0  # width of wall
    x0 = 50 * dx  # impulse located at middle
    relerr = False

    while x < lenx:
        y = exact_solution_impulse(alpha, A, x0, x, t)
        comparison_arr.append((x, y))
        x += dx

    if len(sys.argv) < 2:
        print("Specify input file as 1st argument")
        sys.exit(1)

    rdfile = sys.argv[1]
    try:
        with open(rdfile, 'r') as file:
            for line in file:
                if '#' in line:
                    continue

                parts = line.split()
                if len(parts) < 2:
                    continue

                xval = float(parts[0])
                yval = float(parts[1])

                # Find the corresponding (x, y) in comparison_arr
                comp_x, comp_y = next(((cx, cy) for cx, cy in comparison_arr if abs(cx - xval) < errbnd), (None, None))

                if comp_x is None:
                    print(f"No matching x value found in comparison array for x={xval}")
                    sys.exit(1)

                if relerr:
                    diffr = abs((yval - comp_y) / comp_y)
                    diffl = diffr <= errbnd
                else:
                    diffr = abs(yval - comp_y)
                    diffl = diffr <= errbnd

                if not diffl:
                    print(f"Check failed at x={xval} y={yval} yexp={comp_y}, diff={diffr}")
                    sys.exit(1)

        print("All checks passed successfully.")
    except FileNotFoundError:
        print(f"File not found: {rdfile}")
        sys.exit(1)

if __name__ == "__main__":
    main(rdfile, dx, dt)
