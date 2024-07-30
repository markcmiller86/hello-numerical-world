#include <Python.h>
#include <stdlib.h>
#include "heat.h"  // Header for the heat equation solver


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
typedef struct {
    double lenx;    // Material length (meters)
    double alpha;   // Material thermal diffusivity (sq-meters/second)
    double bc0;     // Boundary condition at x=0
    double bc1;     // Boundary condition at x=lenx
    char* ic;       // Initial condition (e.g., "const(1)")
} HeatProblem;

// Structure to hold solution data
typedef struct {
    double dx;          // x (lenx) increment (meters)
    double dt;          // t-increment (seconds)
    double maxt;        // Maximum simulation time
    int nx;             // Number of samples
    int probIndex;      // Index of the associated problem
    double uk[MAX_NX];  // Array of u(x,k) to compute/return
    double uk1[MAX_NX]; // Array u(x,k-1) computed at -1 time index ago
} HeatSolution;

// Structure to hold run data
typedef struct {
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
static PyObject* init_problem(PyObject *self, PyObject *args) {
    double lenx, alpha, bc0, bc1;
    char* ic;
    // Parse arguments from Python: lenx, alpha, bc0, bc1, ic
    if (!PyArg_ParseTuple(args, "dddds", &lenx, &alpha, &bc0, &bc1, &ic)) {
        return NULL;
    }

    // Check if the maximum number of problems has been exceeded
    if (problemIndex >= MAX_PROBLEMS) {
    PyErr_SetString(PyExc_RuntimeError, "Maximum number of problems exceeded");
    return NULL;
    }

    // Initialize the problem structure
    HeatProblem *prob = &problems[problemIndex]; // Get the next available problem index
    prob->lenx = lenx;
    prob->alpha = alpha;    
    prob->bc0 = bc0;
    prob->bc1 = bc1;
    prob->ic = ic; // may need to dynamically allocate memory to avoid issues related to the scope of the string.

    // Return the index of the initialized problem
    return PyLong_FromLong((long)problemIndex++);
}

// Function to initialize the heat equation solution and return its index
static PyObject* init_solution(PyObject *self, PyObject *args) {
    int probIndex, nx;
    double dx, dt, maxt;
    // Parse arguments from Python: problem index, dx, dt, maxt, nx
    if (!PyArg_ParseTuple(args, "idddi", &probIndex, &dx, &dt, &maxt, &nx)) {
        return NULL;
    }
    // Check if the maximum number of solutions has been exceeded
      if (solutionIndex >= MAX_SOLUTIONS || probIndex >= problemIndex) {
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

    // Check if the maximum number of runs has been exceeded
    if (runIndex >= MAX_RUNS || solIndex >= solutionIndex) {
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

    // Retrieve the associated problem structure using the problem index
    int probIndex = sol->probIndex;
    HeatProblem *prob = &problems[probIndex];

    // Calculate the number of time steps
    int time_steps = (int)(sol->maxt / sol->dt);
    int stable = 1;

    // Run the simulation using the FTCS method
    for (int t = 0; t < time_steps; t++) {
        stable = update_solution_ftcs(sol->nx, sol->uk, sol->uk1, prob->alpha, sol->dx, sol->dt, prob->bc0, prob->bc1);

        // Stores the temperature profile at regular intervals
        if ((t > 0 && run->savi) && t%(run->savi)==0) {
           run->times[t] = t*sol->dt; // stores current time
           copy(sol->nx, run->t_results[t], sol->uk); // stores current temperature
        }

        // Copy current results to array using copy function from utils.c
        copy(sol->nx, sol->uk1, sol->uk);
        
        // Check if the solution became unstable
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
    // Parse arguments from Python: run index
    if (!PyArg_ParseTuple(args, "i", &runIndex)) {
        return NULL;
    }

    // Check if the run index is valid
    if (runIndex < 0 || runIndex >= MAX_RUNS) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid run index");
        return NULL;
    }

    HeatRun *run = &runs[runIndex];
    HeatSolution *sol = &solutions[run->solIndex];

    // Create a list to hold the results
    PyObject *results = PyList_New(0);

    int i = 0;
    while (i < MAX_TIMES && run->t_results[i]) {
        // Create a list to hold the temperature results for this time
        PyObject *cur_result = PyList_New(0);
        for (int j = 0; j < sol->nx; j++) {
            PyList_Append(cur_result, Py_BuildValue("d", run->t_results[i][j]));
        }
        PyList_Append(results, cur_result);
        i++;
    }

    return results;
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