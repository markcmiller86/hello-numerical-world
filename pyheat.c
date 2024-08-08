#include <Python.h> // For Python C API
#include <stdlib.h> // For calloc
#include <string.h> // For strdup
#include "heat.h"  // Header for the heat equation solver


// declare set_initial_condition function from utils.c
extern void
set_initial_condition(int n, Number *a, Number dx, char const *ic);


// Declare copy function from utils.c
extern void
copy(int n, Number *dst, Number const *src);


// Declare update_solution_ftcs function from ftcs.c
extern int
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

// Maximum number of problems, solutions, and runs that can be handled
#define MAX_PROBLEMS 10
#define MAX_SOLUTIONS 10
#define MAX_RUNS 10
#define MAX_NX 1000    // Maximum number of samples for solutions
#define MAX_TIMES 1000 // Maximum number of times stored

// Structure to hold problem data
typedef struct 
{
    double lenx;    // Material length (meters)
    double alpha;   // Material thermal diffusivity (sq-meters/second)
    double bc0;     // Boundary condition at x=0
    double bc1;     // Boundary condition at x=lenx
    char* ic;       // Initial condition (e.g., "const(1)")
} HeatProblem;

// Structure to hold solution data
typedef struct 
{
    double dx;          // x (lenx) increment (meters)
    double dt;          // t-increment (seconds)
    double maxt;        // Maximum simulation time
    int nx;             // Number of samples
    int probIndex;      // Index of the associated problem
    double uk[MAX_NX];  // Array of u(x,k) to compute/return
    double uk1[MAX_NX]; // Array u(x,k-1) computed at -1 time index ago
} HeatSolution;

// Structure to hold run data
typedef struct 
{
    char* run_name;     // Name of the run
    int savi;           // How often temperature profile is saved
    int outi;           // How often progress is reported
    double *times;      // Stores times at which results are stored
    double **t_results; // Stores the evolved temp results
    int solIndex;       // Index of the associated solution
} HeatRun;

// Global arrays to hold instances of problem, solution, and run data
static HeatProblem problems[MAX_PROBLEMS];
static HeatSolution solutions[MAX_SOLUTIONS];
static HeatRun runs[MAX_RUNS];

// Global variables to track the next available index for problems, solutions, and runs
static int problemIndex = 0;  
static int solutionIndex = 0; 
static int runIndex = 0; 

// Function to initialize the heat equation problem and return its index
static PyObject* problem(PyObject *self, PyObject *args) 
{
    double lenx, alpha, bc0, bc1;
    char* ic;

    // Parse arguments from Python: lenx, alpha, bc0, bc1, ic
    if (!PyArg_ParseTuple(args, "dddds", &lenx, &alpha, &bc0, &bc1, &ic)) 
    {
        PyErr_SetString(PyExc_TypeError, 
        "\n"
        "INVALID ARGUMENTS\n"
        "Expected parameters: lenx, alpha, bc0, bc1, ic (float, float, float, float, str)\n"
        "\n"
        "Example: prob = pyheat.problem(1, 0.2, 0, 0, 'const(1)')\n"
        "\n"
        "For more info type: help(pyheat.problem)\n"
        );
        return NULL;
    }

    // Check if the maximum number of problems has been exceeded
    if (problemIndex >= MAX_PROBLEMS) 
    {
    PyErr_SetString(PyExc_RuntimeError, "Maximum number of problems exceeded");
    return NULL;
    }

    // Initialize the problem structure
    HeatProblem *prob = &problems[problemIndex]; // Get the next available problem index
    prob->lenx = lenx;
    prob->alpha = alpha;    
    prob->bc0 = bc0;
    prob->bc1 = bc1;
    prob->ic = strdup(ic); 

    // Return the index of the initialized problem
    return PyLong_FromLong((long)problemIndex++);
}

// Function to initialize the heat equation solution and return its index
static PyObject* solution(PyObject *self, PyObject *args) 
{
    int probIndex, nx;
    double dx, dt, maxt;
    
    // Parse arguments from Python: problem index, dx, dt, maxt, nx
    if (!PyArg_ParseTuple(args, "idddi", &probIndex, &dx, &dt, &maxt, &nx)) 
    {
        PyErr_SetString(PyExc_TypeError,
        "\n"
        "INVALID ARGUMENTS\n"
        "Expected parameters: probIndex, dx, dt, maxt, nx (int, float, float, float, int)\n"
        "\n"
        "Example: sol = pyheat.solution(0, 0.01, 0.01, 1.0, 100)\n"
        "\n"
        "For more info type: help(pyheat.solution)\n"
        );
        return NULL;
    }

    // Check if the maximum number of solutions has been exceeded
      if (solutionIndex >= MAX_SOLUTIONS || probIndex >= problemIndex) 
      {
        PyErr_SetString(PyExc_RuntimeError, "Invalid solution or problem index");
        return NULL;
      }

    // Initialize the solution structure
    HeatSolution *sol = &solutions[solutionIndex]; // Get the next available solution index
    sol->dx = dx;
    sol->dt = dt;
    sol->maxt = maxt;
    sol->nx = nx;
    sol->probIndex = probIndex;  // Set the problem index

    // Retrieve the problem stucture using the problem index
    HeatProblem *prob = &problems[probIndex];

    // Set the initial condition using the set_initial_condition function from utils.c
     set_initial_condition(sol->nx, sol->uk1, sol->dx, prob->ic);

    // Return the index of the initialized solution
    return PyLong_FromLong((long)solutionIndex++);
}

// Function to run the heat equation simulation
static PyObject* run(PyObject *self, PyObject *args) 
{
    int solIndex;
    char* run_name;
    int savi, outi;

    // Parse arguments from Python: solution index, run_name, savi, outi
    if (!PyArg_ParseTuple(args, "isii", &solIndex, &run_name, &savi, &outi)) 
    {
        PyErr_SetString(PyExc_TypeError,
        "\n"
        "INVALID ARGUMENTS\n"
        "Expected parameters: solIndex, run_name, savi, outi (int, str, int, int)\n"
        "\n"
        "Example: run = pyheat.run(sol, 'heat_results', 100, 100)\n"
        "\n"
        "For more info type: help(pyheat.run)\n"
        );
        return NULL;
    }

    // Check if the maximum number of runs has been exceeded
    if (runIndex >= MAX_RUNS || solIndex >= solutionIndex) 
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid solution or run index");
        return NULL;
    }

    // Initialize the run structure
    HeatRun *run = &runs[runIndex]; // Get the next available run index
    run->run_name = run_name;
    run->savi = savi;
    run->outi = outi;
    run->times = (double *) calloc(MAX_TIMES, sizeof(double));
    run->t_results = (double **) calloc(MAX_TIMES, sizeof(double*));

    // Retrieve the solution structure using its index
    HeatSolution *sol = &solutions[solIndex];
    
    int probIndex = sol->probIndex; // Get the problem index associated with the solution

    // Retrieve the associated problem structure using the problem index
    HeatProblem *prob = &problems[probIndex];

    // Calculate the number of time steps
    int time_steps = (int)(sol->maxt / sol->dt);
    int stable = 1;

    // Run the simulation using the FTCS method
    for (int t = 0; t < time_steps; t++) 
    {
        stable = update_solution_ftcs(sol->nx, sol->uk, sol->uk1, prob->alpha, sol->dx, sol->dt, prob->bc0, prob->bc1);

        // Stores the temperature profile at regular intervals
        if ((t > 0 && run->savi) && t % (run->savi) == 0) 
        {
           run->times[t] = t*sol->dt; // stores current time
           run->t_results[t] = (double *) calloc(sol->nx, sizeof(double)); // stores current temp

           copy(sol->nx, run->t_results[t], sol->uk); // copy current temp to t_results
        }

        // Copy current results to array using copy function from utils.c
        copy(sol->nx, sol->uk1, sol->uk);
        
        // Check if the solution became unstable
        if (!stable) 
        {
            PyErr_SetString(PyExc_RuntimeError, "Solution became unstable");
            return NULL;
        }
    }

    // Return the index of the run if successful, otherwise return -1
    return PyLong_FromLong((long)(stable ? runIndex++ : -1));
}

// Function to return simulation results
static PyObject* results(PyObject *self, PyObject *args) 
{
    int runIn; // Run index
    
    // Parse arguments from Python: run index
    if (!PyArg_ParseTuple(args, "i", &runIn)) 
    {
        PyErr_SetString(PyExc_TypeError,
        "\n"
        "INVALID ARGUMENTS\n"
        "Expected parameters: runIndex (int)\n"
        "\n"
        "Example: results = pyheat.results(run)\n"
        "\n"
        "For more info type: help(pyheat.results)\n"
        );
        return NULL;
    }

    // Check if the run index is valid
    if (runIn < 0 || runIn >= MAX_RUNS) 
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid run index");
        return NULL;
    }

    HeatRun *run = &runs[runIn]; // Retrieve the associated run structure
    HeatSolution *sol = &solutions[run->solIndex]; // Retrieve the associated solution structure

    // Create a list to hold the results
    PyObject *results = PyList_New(0);

    int i = 0;
    while (i < MAX_TIMES) 
    {
        if (run->t_results[i] == NULL) // Check if the time index is empty
        {
            i++;
            continue;
        }

        // Create a list to hold the temperature results for this time
        PyObject *cur_result = PyList_New(0);
        
        for (int j = 0; j < sol->nx; j++) 
        {
            PyList_Append(cur_result, Py_BuildValue("d", run->t_results[i][j]));
        }
        PyList_Append(results, cur_result);
        i++;
    }

    return results;
}

// Documentation for the problem function
PyDoc_STRVAR(problem_doc,
"\n"
"problem(lenx, alpha, bc0, bc1, ic)\n"
"=======================================\n"
"\n"
"Initializes a heat problem with the given parameters.\n"
"\n"
"Parameters\n"
"----------\n"
"lenx : float\n"
"    Length of the material in meters.\n"
"alpha : float\n"
"    Thermal diffusivity of the material (sq-meters/second).\n"
"bc0 : float\n"
"    Boundary condition at x=0.\n"
"bc1 : float\n"
"    Boundary condition at x=lenx.\n"
"ic : str\n"
"    Initial condition string (e.g., 'const(1)').\n"
"\n"
"Returns\n"
"-------\n"
"int\n"
"    Index of the initialized problem.\n"
"    This index is used to initialize a solution.\n"
"\n"
"Example\n"
"-------\n"
"prob = problem(1.0, 0.02, 0, 0, 'const(1)')\n"
"print(f'Initialized problem with index: {prob}')\n"
);

// Documentation for the solution function
PyDoc_STRVAR(solution_doc,
"\n"
"solution(probIndex, dx, dt, maxt, nx)\n"
"=======================================\n"
"\n"
"Initializes a heat solution with the given parameters.\n"
"\n"
"Parameters\n"
"----------\n"
"probIndex : int\n"
"    Index of the associated problem.\n"
"dx : float\n"
"    x (lenx) increment in meters.\n"
"dt : float\n"
"    t-increment in seconds.\n"
"maxt : float\n"
"    Maximum simulation time in seconds.\n"
"nx : int\n"
"    Number of samples.\n"
"\n"
"Returns\n"
"-------\n"
"int\n"
"    Index of the initialized solution.\n"
"    This index is used to run the simulation.\n"
"\n"
"Example\n"
"-------\n"
"sol = solution(prob, 0.01, 0.00004, 0.04, 100)\n"
"printf(f'Initialized solution with index: {sol}')\n"
);

// Documentation for the run function
PyDoc_STRVAR(run_doc,
"\n"
"run(solIndex, run_name, savi, outi)\n"
"=======================================\n"
"\n"
"Runs the heat simulation with the given parameters.\n"
"\n"
"Parameters\n"
"----------\n"
"solIndex : int\n"
"    Index of the associated solution.\n"
"run_name : str\n"
"    Name of the run.\n"
"savi : int\n"
"    How often the temperature profile is saved.\n"
"outi : int\n"
"    How often the progress is reported.\n"
"\n"
"Returns\n"
"-------\n"
"int\n"
"    Index of the run if successful, otherwise -1.\n"
"    This index is used to retrieve the simulation results.\n"
"\n"
"Example\n"
"-------\n"
"run = pyheat.run(sol, 'heat_results', 100, 100)\n"
"print(f'Initialized run with index: {run}')\n"
);

// Documentation for the results function
PyDoc_STRVAR(results_doc,
"\n"
"results(runIndex)\n"
"=======================================\n"
"\n"
"Returns the simulation results for the given run index.\n"
"\n"
"Parameters\n"
"----------\n"
"runIndex : int\n"
"    Index of the run.\n"
"\n"
"Returns\n"
"-------\n"
"list\n"
"    List of lists containing the temperature results at each time step.\n"
"\n"
"Example\n"
"-------\n"
"results = pyheat.results(run)\n"
"print(f'Simulation results: {results}')\n"
);

// Module-level documentation
PyDoc_STRVAR(module_doc,
"\n"
"Why is this module useful?\n"
"--------------------------\n"
"\n"
"This module provides a Python interface to the heat equation solver.\n"
"In doing so we can define heat problems, solutions, and runs in Python.\n"
"This achieves a high-level interface to the low-level C heat solver." 
"We obtain the following benefits:\n"
"    Interactivity: We can define and run simulations interactively in Python.\n"
"    Flexibility: We can easily change parameters and rerun simulations.\n"
"    Visualization: We can visualize results using Python libraries.\n"
"    Usability: Provides users familar with Python an easy way to use the heat solver.\n"
"\n"
"How does the Python C API work?\n"
"--------------------------------\n"
"\n"
"The Python C API allows us to write C extensions for Python.\n"
"This means we can write C code that can be imported and used in Python.\n"
"The API provides functions and macros to interact with Python objects.\n"
"Using the API we can define functions, classes, and modules in C.\n"
"For more information see: https://docs.python.org/3/c-api/\n"
"\n"
"Structure of the module:\n"
"-----------------------\n"
"\n"
"This module is split into four parts:\n"
"    1. Problem: Initialize a heat problem with given parameters.\n"
"    2. Solution: Initialize a heat solution with given parameters.\n"
"    3. Run: Run the heat simulation with given parameters.\n"
"    4. Results: Retrieve the simulation results for a given run.\n"
"\n"
"Each part takes corresponding function from the heat solver and exposes it to Python\n"
"\n"
"It is designed to be used sequentially: problem -> solution -> run -> results\n"
"We enter the parameters for each part and use the returned index to move to the next part.\n"
"We ensure that the user follows this sequence by checking the indices of problems, solutions, and runs.\n"
"\n"
"Arguments used in each part:\n"
"    Problem:\n" 
"        lenx: Length of the material in meters.\n"
"        alpha: Thermal diffusivity of the material (sq-meters/second).\n"
"        bc0: Boundary condition at x=0.\n"
"        bc1: Boundary condition at x=lenx.\n"
"        ic: Initial condition string (e.g., 'const(1)').\n"
"    Solution:\n"
"        probIndex: Index of the associated problem.\n"
"        dx: x (lenx) increment in meters.\n"
"        dt: t-increment in seconds.\n"
"        maxt: Maximum simulation time in seconds.\n"
"        nx: Number of samples.\n"
"    Run:\n"
"        solIndex: Index of the associated solution.\n"
"        run_name: Name of the run.\n"
"        savi: How often the temperature profile is saved.\n"
"        outi: How often the progress is reported.\n"
"    Results:\n"
"        runIndex: Index of the run.\n"
"\n"
"From here we can then use results to interact with the data and visualize the simulation results.\n"
"\n"
"The user can type the following help() commands to get more information on each part.\n"
"    help(pyheat.problem)\n"
"    help(pyheat.solution)\n"
"    help(pyheat.run)\n"
"    help(pyheat.results)\n"
"\n"
"How the example script heat.py works:\n"
"--------------------------------\n"
"\n"
"The example script heat.py demonstrates how to use this module.\n"
"It initializes a problem, solution, and run, runs the simulation, and retrieves the results.\n"
"The script then visualizes the results using the matplotlib library.\n"
"\n"
"How to use with matlpotlib:\n"
"--------------------------\n"
"\n"
"Once you have the simulation results you can use the matplotlib library to visualize the results.\n"
"[Note: please ensure you import matlopib.pyplot as plt and sys in your script]\n"
"Here is an example script that visualizes the results:\n"
"\n"
"x = []\n # x values are the distance values\n"
"for i in range (0,100): \n"
"    x.append(i*0.01) # iterates through the x values\n"
"y = results[4] # y values are the temperature values"
"\n"
"plt.xlabel('Distance (meters)')\n"
"plt.ylabel('Temperature (Kelvin)')\n"
"plt.plot(x,y)\n"
"plt.show()\n"
"sys.exit(0)\n"
"\n"
"How to integrate with numpy:\n"
"--------------------------\n"
"\n"
"Once you have the simulation results you can use the numpy library to perform numerical operations.\n"
"[Note: please sure you import numpy as np and sys in your script]\n"
"Here is an example script that integrates the temperature profile using numpy:\n"
"\n"
"... \n"
"... \n"
"\n"
);






// Define methods exposed to Python
static PyMethodDef PyHeatMethods[] = 
{
    {"problem", problem, METH_VARARGS,  problem_doc},
    {"solution", solution, METH_VARARGS, solution_doc},
    {"run", run, METH_VARARGS, run_doc},
    {"results", results, METH_VARARGS, results_doc},
    {NULL, NULL, 0, NULL} // Sentinel
};

// Define module
static struct PyModuleDef pyheatmodule = 
{
    PyModuleDef_HEAD_INIT,
    "pyheat",
    module_doc, // Module documentation
    -1, // Size of per-interpreter state of the module
    PyHeatMethods
};

// Initialize module
PyMODINIT_FUNC PyInit_pyheat(void) 
{
    return PyModule_Create(&pyheatmodule);
}