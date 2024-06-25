import numpy as np
import matplotlib.pyplot as plt
import sys

def plot_heat(run_name):
    try:
        initial_curve = np.loadtxt(f"{run_name}/{run_name}_soln_00000.curve")
        final_curve = np.loadtxt(f"{run_name}/{run_name}_soln_final.curve")
    except IOError as e:
        print("Error reading files:", e)
        sys.exit(1)

    # Assuming lenx is the length of the domain
    lenx = initial_curve[-1, 0]

    # Compute positions for pipe
    lenx2 = lenx / 2
    p0 = lenx2 - 0.05
    p1 = lenx2 + 0.05

    plt.figure()
    plt.plot(initial_curve[:, 0], initial_curve[:, 1], label="Initial")
    plt.plot(final_curve[:, 0], final_curve[:, 1], label="Final")
    plt.axhline(y=273.15, color='blue', linestyle='--', label="Freezing Point")
    plt.axvline(x=p0, color='black', linestyle='--', label="Pipe Start")
    plt.axvline(x=p1, color='black', linestyle='--', label="Pipe End")
    plt.xlabel("Position")
    plt.ylabel("Temperature (K)")
    plt.title("Heat Equation Solution")
    plt.legend()
    plt.grid(True)
    plt.savefig(f"{run_name}/temperature_plot.png")
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot_heat.py <run_name>")
        sys.exit(1)
    
    run_name = sys.argv[1]
    plot_heat(run_name)
