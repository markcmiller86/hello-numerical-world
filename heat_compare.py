import pyheat # type: ignore
import sys
import matplotlib.pyplot as plt

# Example of using the pyheat module to run a heat transfer simulation with two different numerical models.
# The simulation is run for a 1D heat transfer problem with a constant heat source and no heat loss.
# Run with the FTCS and Dufrank numerical models.
# The results are plotted to compare the temperature profiles predicted by the two.

# Initialize the problem with material properties and boundary conditions
prob_index = pyheat.problem(1, 0.2, 0, 0, "const(3)")
print(f"Initialized Problem Index: {prob_index}")

# Initialize the solution for the problem with numerical model parameters
ftcs_index = pyheat.solution(prob_index, 0.01, 0.00004, 0.04, 100, "ftcs")
print(f"Initialized Solution Index: {ftcs_index}")

# Initialize the solution for the problem with numerical model parameters
dufrank_index = pyheat.solution(prob_index, 0.01, 0.00004, 0.04, 100, "dufrank")
print(f"Initialized Solution Index: {dufrank_index}")

# Run the simulation with the initialized solution and save settings
ftcs_run_index = pyheat.run(ftcs_index, "heat_results", 100, 100)
if ftcs_run_index < 0:
    print("Problem running simulation")
    sys.exit(1)

dufrank_run_index = pyheat.run(dufrank_index, "heat_results", 100, 100)
if dufrank_run_index < 0:
    print("Problem running simulation")
    sys.exit(1)

# Return the simulation results
ftcs_results = pyheat.results(ftcs_run_index)
dufrank_results = pyheat.results(dufrank_run_index)
print(f"Simulation results: {ftcs_results[4]}") # Temperature values at 4th index in array
print(f"Simulation results: {dufrank_results[4]}") # Temperature values at 4th index in array

# Plot the results
x = [] # x values are the distance values
for i in range (0,100): 
    x.append(i*0.01) # iterates through the x values
y1 = ftcs_results[4] # y values are the temperature values
y2 = dufrank_results[4] # y values are the temperature values

plt.xlabel('Distance (meters)')
plt.ylabel('Temperature (Kelvin)')
plt.plot(x,y1)
plt.plot(x,y2)
plt.show()
sys.exit(0)

