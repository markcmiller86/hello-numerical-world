#include <Python.h>
#include <stdlib.h>
#include "heat.h"  // Include the header for the heat equation solver


// #include "utils.c" // Ensure utility functions are available

// Include the actual implementation file

// #include "ftcs.c"
// #include "heat.c"

// declaring object name and type
extern int
update_solution_ftcs(int n,
    Number *curr, Number const *back1,
    Number alpha, Number dx, Number dt,
    Number bc_0, Number bc_1);

// Define a structure to hold problem data
typedef struct {
    double lenx;
    double alpha;
    int nx;
    double dx;
    double dt;
    double *uk;
    double *uk1;
} HeatProblem;

// Global variable to hold the problem data
static HeatProblem problem;

// Function to initialize the heat equation problem
static PyObject* init_problem(PyObject *self, PyObject *args) {
    double lenx, alpha;
    int nx;
    if (!PyArg_ParseTuple(args, "ddi", &lenx, &alpha, &nx)) {
        return NULL;
    }
    
    problem.lenx = lenx;
    problem.alpha = alpha;
    problem.nx = nx;
    problem.dx = lenx / (nx - 1);
    problem.dt = 0.0; // Default value, will be set in solve function
    problem.uk = (double *)malloc(nx * sizeof(double));
    problem.uk1 = (double *)malloc(nx * sizeof(double));
    
    if (!problem.uk || !problem.uk1) {
        return PyErr_NoMemory();
    }
    
    // Initialize uk and uk1 with initial conditions (i.e., zero)
    for (int i = 0; i < nx; i++) {
        problem.uk[i] = 0.0;
        problem.uk1[i] = 0.0;
    }
    
    Py_RETURN_NONE;
    // modify to return pyobj of our creation [notes]
}

// Function to solve the heat equation
static PyObject* solve_heat_equation(PyObject *self, PyObject *args) {
    double dx, dt, maxt, bc0, bc1;
    int nt;
    if (!PyArg_ParseTuple(args, "dddi|dd", &dx, &dt, &maxt, &nt, &bc0, &bc1)) {
        return NULL;
    }
    
    problem.dx = dx;
    problem.dt = dt;
    double alpha = problem.alpha;
    double lenx = problem.lenx;
    int nx = problem.nx;
    
    int stable = 1;
    int time_steps = (int)(maxt / dt);
    
    for (int t = 0; t < time_steps; t++) {
        stable = update_solution_ftcs(nx, problem.uk, problem.uk1, alpha, dx, dt, bc0, bc1);
        if (!stable) {
            PyErr_SetString(PyExc_RuntimeError, "Solution became unstable");
            return NULL;
        }
        
        // Swap uk and uk1 for next iteration
        double *temp = problem.uk1;
        problem.uk1 = problem.uk;
        problem.uk = temp;
    }
    
    // Return the final solution as a Python list
    PyObject *result = PyList_New(nx);
    for (int i = 0; i < nx; i++) {
        PyList_SetItem(result, i, PyFloat_FromDouble(problem.uk[i]));
    }
    
    return result;
}

// Declare methods in the module
static PyMethodDef HeatMethods[] = {
    {"init_problem", init_problem, METH_VARARGS, "Initialize the heat equation problem"},
    {"solve_heat_equation", solve_heat_equation, METH_VARARGS, "Solve the heat equation"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

// Define the module structure
static struct PyModuleDef heatmodule = {
    PyModuleDef_HEAD_INIT,
    "pyheat", /* name of module */
    NULL,  /* module documentation, may be NULL */
    -1,    /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    HeatMethods
};

// Initialize the module
PyMODINIT_FUNC PyInit_pyheat(void) {
    // Debugging statement to confirm the function is called
    printf("Initializing pyheat module\n");
    return PyModule_Create(&heatmodule);
}
