#include <Python.h>

// Define a new Python function
static PyObject* foo_func(PyObject *self, PyObject *args) {
	return PyUnicode_FromString("hello world");
}

// Declare methods in the module
static PyMethodDef FooMethods[] = {
	{"foo_func", foo_func, METH_NOARGS, "Return the string 'hello world'"},
	{NULL, NULL, 0, NULL} /* Sentinel */
};

// Define the module structure
static struct PyModuleDef foomodule = {
	PyModuleDef_HEAD_INIT,
	"foo", /* name of module */
	NULL,  /* module documentation, may be NULL */
	-1,    /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	FooMethods
};

// Initialize the module
PyMODINIT_FUNC PyInit_helloPy(void) {
	return PyModule_Create(&foomodule);
}
