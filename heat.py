import pyheat # type: ignore
import sys

# Initialize the problem with material properties and boundary conditions
prob_index = pyheat.init_problem(0.25, 8.2e-8, 1, 0, "const(1)")
print(f"Initialized Problem Index: {prob_index}")

# Initialize the solution for the problem with numerical model parameters
sol_index = pyheat.init_solution(prob_index, 0.01, 0.0001, 0.05, 100)
print(f"Initialized Solution Index: {sol_index}")

# Run the simulation with the initialized solution and save settings
run_index = pyheat.run_simulation(sol_index, "wood-at-const-1", 100, 100)
if run_index < 0:
    print("Problem running simulation")
    sys.exit(1)

# Assuming return_simulation_results is another function you might implement
results = pyheat.return_simulation_results(run_index)
print(f"Simulation results at t=0.0004: {results}")