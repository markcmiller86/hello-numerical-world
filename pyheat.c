#include <Python.h>
#include <stdlib.h>
#include "heat.h"  // Includes the header for the heat equation solver

// declaring object name and type
extern int
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

// Maximum number of problems, solutions, and runs that can be handled
#define MAX_PROBLEMS 10
#define MAX_SOLUTIONS 10
#define MAX_RUNS 10
#define MAX_NX 1000  // Maximum number of samples for solutions

// Defines a structure to hold problem data
typedef struct {
    double lenx;    // Material length (meters)
    double alpha;   // Material thermal diffusivity (sq-meters/second)
    double bc0;     // Boundary condition at x=0
    double bc1;     // Boundary condition at x=lenx
    char* ic;       // Initial condition (e.g., "const(1)")
} HeatProblem;

// Defines a structure to hold solution data
typedef struct {
    double dx;      // x (lenx) increment (meters)
    double dt;      // t-increment (seconds)
    double maxt;    // Maximum simulation time
    int nx;         // Number of samples
    int probIndex;  // Index of the associated problem
    double uk[MAX_NX];  // Array of u(x,k) to compute/return
    double uk1[MAX_NX]; // Array u(x,k-1) computed at -1 time index ago
} HeatSolution;

// Defines a structure to hold run data
typedef struct {
    char* run_name; // Name of the run
    int savi;       // How often temperature profile is saved
    int outi;       // How often progress is reported
} HeatRun;

// Global arrays to hold instances of problem, solution, and run data
static HeatProblem problems[MAX_PROBLEMS];
static HeatSolution solutions[MAX_SOLUTIONS];
static HeatRun runs[MAX_RUNS];
static int problemIndex = 0;  // Tracks the next available index for problems
static int solutionIndex = 0; // Tracks the next available index for solutions
static int runIndex = 0;      // Tracks the next available index for runs

// Function to initialize the heat equation problem
static PyObject* init_problem(PyObject *self, PyObject *args) {
    double lenx, alpha, bc0, bc1;
    char* ic;
    // Parse arguments from Python: lenx, alpha, bc0, bc1, ic
    if (!PyArg_ParseTuple(args, "dddds", &lenx, &alpha, &bc0, &bc1, &ic)) {
        return NULL;
    }

    // Initialize the problem structure
    problems[problemIndex].lenx = lenx;
    problems[problemIndex].alpha = alpha;
    problems[problemIndex].bc0 = bc0;
    problems[problemIndex].bc1 = bc1;
    problems[problemIndex].ic = ic;

    // Return the index of the initialized problem
    return PyLong_FromLong((long)problemIndex++);
}

// Function to initialize the heat equation solution
static PyObject* init_solution(PyObject *self, PyObject *args) {
    int probIndex, nx;
    double dx, dt, maxt;
    // Parse arguments from Python: problem index, dx, dt, maxt, nx
    if (!PyArg_ParseTuple(args, "idddi", &probIndex, &dx, &dt, &maxt, &nx)) {
        return NULL;
    }

    // Initialize the solution structure
    solutions[solutionIndex].dx = dx;
    solutions[solutionIndex].dt = dt;
    solutions[solutionIndex].maxt = maxt;
    solutions[solutionIndex].nx = nx;
    solutions[solutionIndex].probIndex = probIndex;  // Set the problem index

    // Initialize uk and uk1 with initial conditions (i.e., zero)
    for (int i = 0; i < nx; i++) {
        solutions[solutionIndex].uk[i] = 0.0;
        solutions[solutionIndex].uk1[i] = 0.0;
    }

    // Return the index of the initialized solution
    return PyLong_FromLong((long)solutionIndex++);
}

// Function to run the heat equation simulation
static PyObject* run_simulation(PyObject *self, PyObject *args) {
    int solIndex;
    char* run_name;
    int savi, outi;
    // Parse arguments from Python: solution index, run_name, savi, outi
    if (!PyArg_ParseTuple(args, "isii", &solIndex, &run_name, &savi, &outi)) {
        return NULL;
    }

    // Initialize the run structure
    runs[runIndex].run_name = run_name;
    runs[runIndex].savi = savi;
    runs[runIndex].outi = outi;

    // Retrieve the solution structure using its index
    HeatSolution *sol = &solutions[solIndex];

    // Retrieve the associated problem structure using the problem index
    int probIndex = sol->probIndex;
    HeatProblem *prob = &problems[probIndex];

    // Calculate the number of time steps
    int time_steps = (int)(sol->maxt / sol->dt);
    int stable = 1;

    // Run the simulation using the FTCS method
    for (int t = 0; t < time_steps; t++) {
        stable = update_solution_ftcs(sol->nx, sol->uk, sol->uk1, prob->alpha, sol->dx, sol->dt, prob->bc0, prob->bc1);
        if (!stable) {
            PyErr_SetString(PyExc_RuntimeError, "Solution became unstable");
            return NULL;
        }
    }

    // Return the index of the run if successful, otherwise return -1
    return PyLong_FromLong((long)(stable ? runIndex++ : -1));
}

// Function to return simulation results
static PyObject* return_simulation_results(PyObject *self, PyObject *args) {
    int runIndex;
    double t;
    // Parse arguments from Python: run index, time t
    if (!PyArg_ParseTuple(args, "id", &runIndex, &t)) {
        return NULL;
    }

    // For now, return a placeholder result
    PyObject *result = PyList_New(0);
    PyList_Append(result, Py_BuildValue("d", t));  // Example: return time t as a result
    return result;
}

// Define methods exposed to Python
static PyMethodDef PyHeatMethods[] = {
    {"init_problem", init_problem, METH_VARARGS, "Initialize a heat problem"},
    {"init_solution", init_solution, METH_VARARGS, "Initialize a heat solution"},
    {"run_simulation", run_simulation, METH_VARARGS, "Run the heat simulation"},
    {"return_simulation_results", return_simulation_results, METH_VARARGS, "Return simulation results"},
    {NULL, NULL, 0, NULL} // Sentinel
};

// Define module
static struct PyModuleDef pyheatmodule = {
    PyModuleDef_HEAD_INIT,
    "pyheat",
    NULL, // Documentation
    -1, // Size of per-interpreter state of the module
    PyHeatMethods
};

// Initialize module
PyMODINIT_FUNC PyInit_pyheat(void) {
    return PyModule_Create(&pyheatmodule);
}