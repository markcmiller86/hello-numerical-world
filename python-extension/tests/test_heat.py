import unittest
import numpy as np
from src.heat import Problem

class TestHeatEquationSolver(unittest.TestCase):
    def test_solver(self):
        # Create an instance of the Problem class with specified parameters
        prob = Problem(lenx=1, alpha=0.01, ic=np.ones(100), bc0=0, bc1=0)
        
        # Set up the solution using the solve method of the Problem instance
        sol = prob.solve(alg='ftcs', maxt=1, dx=0.01, dt=0.0001, precision='single')
        
        # Run the solver to compute the temperature distribution
        result = sol.run()
        
        # Check that the result is not None
        self.assertIsNotNone(result)
        
        # Check that the length of the result array is 100
        self.assertEqual(len(result), 100)

# Run the test cases if the script is executed directly
if __name__ == '__main__':
    unittest.main()
