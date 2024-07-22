#include <Python.h>
#include <stdlib.h>
#include "heat.h"  // Include the header for the heat equation solver

// declaring object name and type
extern int
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);


// Defines a structure to hold problem data
typedef struct {
    double lenx;    // material length (meters)
    double alpha;   // material thermal diffusivity (sq-meters/second)
} HeatProblem;

// Defines a structure to hold solution data
typedef struct {
    double dx;      // x (lenx) increment (meters)
    double dt;      // t-increment (seconds)
    int nx;         // number of samples (from ftcs.c)
    double *uk;     // new array of u(x,k) to compute/return (from ftcs.c)
    double *uk1;    // array u(x,k-1) computed @ -1 time index ago (from ftcs.c)
} HeatSolution;

// Defines a structure to hold run data
typedef struct {
    // Placeholder for future run-specific variables if needed
} HeatRun;

// Global arrays to hold instances of problem, solution, and run data
static HeatProblem problems[10]; // array to hold problem "object", maxed at 10
static HeatSolution solutions[10]; // array to hold solution "object", maxed at 10
static HeatRun runs[10]; // array to hold run "object", maxed at 10
static int problemIndex = 0; 
static int solutionIndex = 0;
static int runIndex = 0;

// Function to initialize the heat equation problem
static PyObject* init_problem(PyObject *self, PyObject *args) {
    double lenx, alpha;
    if (!PyArg_ParseTuple(args, "dd", &lenx, &alpha)) {
        return NULL;
    }

    problems[problemIndex].lenx = lenx;
    problems[problemIndex].alpha = alpha;

    return PyLong_FromLong((long)problemIndex++);
}

// Function to initialize the heat equation solution
static PyObject* init_solution(PyObject *self, PyObject *args) {
    int probIndex, nx;
    double dx, dt;
    if (!PyArg_ParseTuple(args, "iddi", &probIndex, &dx, &dt, &nx)) {
        return NULL;
    }

    solutions[solutionIndex].dx = dx;
    solutions[solutionIndex].dt = dt;
    solutions[solutionIndex].nx = nx;
    solutions[solutionIndex].uk = (double *)malloc(nx * sizeof(double));
    solutions[solutionIndex].uk1 = (double *)malloc(nx * sizeof(double));

    if (!solutions[solutionIndex].uk || !solutions[solutionIndex].uk1) {
        return PyErr_NoMemory();
    }

    // Initialize uk and uk1 with initial conditions (i.e., zero)
    for (int i = 0; i < nx; i++) {
        solutions[solutionIndex].uk[i] = 0.0;
        solutions[solutionIndex].uk1[i] = 0.0;
    }

    return PyLong_FromLong((long)solutionIndex++);
}


// Function to solve the heat equation
static PyObject* solve_heat_equation(PyObject *self, PyObject *args) {
    int solIndex, probIndex;
    double maxt, bc0, bc1;
    int nt;

    if (!PyArg_ParseTuple(args, "iididd", &solIndex, &probIndex, &maxt, &nt, &bc0, &bc1)) {
        return NULL;
    }

    HeatProblem *prob = &problems[probIndex];
    HeatSolution *sol = &solutions[solIndex];

    int stable = 1;
    int time_steps = (int)(maxt / sol->dt);

    for (int t = 0; t < time_steps; t++) {
        stable = update_solution_ftcs(sol->nx, sol->uk, sol->uk1, prob->alpha, sol->dx, sol->dt, bc0, bc1);
        if (!stable) {
            PyErr_SetString(PyExc_RuntimeError, "Solution became unstable");
            return NULL;
        }
    }

    return PyFloat_FromDouble(sol->uk[sol->nx - 1]);  // Example return [need to adjust]
}

// Define methods
static PyMethodDef PyHeatMethods[] = {
    {"init_problem", init_problem, METH_VARARGS, "Initialize a heat problem"},
    {"init_solution", init_solution, METH_VARARGS, "Initialize a heat solution"},
    {"solve_heat_equation", solve_heat_equation, METH_VARARGS, "Solve the heat equation"},
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