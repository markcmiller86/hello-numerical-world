import pyheat # type: ignore
import sys
import matplotlib.pyplot as plt

# Example of using the pyheat module to run a heat transfer simulation
# CLI command that would be used in C application: 
# ./heat dx=0.01 dt=0.00004 bc1=0 ic="spikes(0,100,50)" maxt=0.04 outi=100 savi=100

# Initialize the problem with material properties and boundary conditions
prob_index = pyheat.init_problem(1, 0.2, 0, 0, "spikes(0,100,50)")
print(f"Initialized Problem Index: {prob_index}")

# Initialize the solution for the problem with numerical model parameters
sol_index = pyheat.init_solution(prob_index, 0.01, 0.00004, 0.04, 100)
print(f"Initialized Solution Index: {sol_index}")

# Run the simulation with the initialized solution and save settings
run_index = pyheat.run_simulation(sol_index, "heat_results", 100, 100)
if run_index < 0:
    print("Problem running simulation")
    sys.exit(1)

# Return the simulation results
results = pyheat.return_simulation_results(run_index)
print(f"Simulation results: {results[4]}") # Temperature values at 4th index in array

# Plot the results
x = [] # x values are the distance values
for i in range (0,100): 
    x.append(i*0.01) # iterates through the x values
y = results[4] # y values are the temperature values

plt.xlabel('Distance (meters)')
plt.ylabel('Temperature (Kelvin)')
plt.plot(x,y)
plt.show()
sys.exit(0)

