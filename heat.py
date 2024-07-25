import pyheat

# Initialize the problems
prob_index = pyheat.init_problem(0.25, 8.2e-8)
print(f"Initialized Problem Index: {prob_index}")

# Initialize the solutions using the problem index
sol_index1 = pyheat.init_solution(prob_index, 0.01, 0.0001, 100)
sol_index2 = pyheat.init_solution(prob_index, 0.02, 0.0002, 100)
print(f"Initialized Solution Indices: {sol_index1}, {sol_index2}")

# Solve the heat equations using the solution indices
result1 = pyheat.solve_heat_equation(sol_index1, prob_index, 10.0, 100, 233.15, 294.261)
result2 = pyheat.solve_heat_equation(sol_index2, prob_index, 10.0, 100, 233.15, 294.261)

# Print the results
print(f"Results: {result1}, {result2}")