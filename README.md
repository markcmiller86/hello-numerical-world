# Hello Numerical World

In this repo is a very simple implementation of an application for solving the one dimensional
heat conduction equation.

In general, heat [conduction](https://en.wikipedia.org/wiki/Thermal_conduction) is governed
by the partial differential (PDE)...

| | |
|:---:|:---:|
|![](http://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%20u%7D%7B%5Cpartial%20t%7D%20-%20%5Cnabla%20%5Ccdot%20%5Calpha%20%5Cnabla%20u%20%3D%200)|(1)|

where _u_ is the temperature at spatial positions, _x_, and times, _t_,
![](http://latex.codecogs.com/gif.latex?%5Calpha) is the _thermal diffusivity_
of the homogeneous material through which heat is flowing. This partial differential equation (PDE)
is known as the _Diffusion Equation_ and also the [_Heat Equation_](https://en.wikipedia.org/wiki/Heat_equation).

### Simplifying Assumptions

To make the problem tractable for this lesson, we make some simplifying assumptions...

1. The thermal diffusivity, ![](http://latex.codecogs.com/gif.latex?%5Calpha),
   is constant for all _space_ and _time_.
1. The only heat _source_ is from the initial and/or boundary conditions.
1. We will deal only with the _one dimensional_ problem in _Cartesian coordinates_.

In this case, the PDE our application needs to solve simplifies to...

| | |
|:---:|:---:|
|![](http://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%20u%7D%7B%5Cpartial%20t%7D%20%3D%20%5Calpha%20%5Cfrac%7B%5Cpartial%5E2%20u%7D%7B%5Cpartial%20x%5E2%7D)|(2)|

Currently, three different numerical algorithms are implemented

* [Foward Time Centered Space (FTCS)](https://en.wikipedia.org/wiki/FTCS_scheme), an
[explicit](https://en.wikipedia.org/wiki/Explicit_and_implicit_methods) method
* [Crank-Nicholson](https://en.wikipedia.org/wiki/Crank–Nicolson_method),
an [implicit](https://en.wikipedia.org/wiki/Explicit_and_implicit_methods) method
* [Upwind-15](https://en.wikipedia.org/wiki/Upwind_scheme), another
[explicit](https://en.wikipedia.org/wiki/Explicit_and_implicit_methods) method
with higher spatial order than FTCS.

The technical details are described more fully in this [ATPESC](https://extremecomputingtraining.anl.gov)
[Hands-On Lesson](https://xsdk-project.github.io/MathPackagesTraining2020/lessons/hand_coded_heat/)
