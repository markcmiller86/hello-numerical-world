import pyheat

# Initialize the problem
pyheat.init_problem(0.25, 8.2e-8, 100)

# Solve the heat equation
result = pyheat.solve_heat_equation(0.01, 100, 55800, 100, 233.15, 294.261)

# Print the result
print(result)
