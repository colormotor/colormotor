# pyColormotor
Python bindings for the [Colormotor](https://github.com/colormotor/colormotor) framework,
built with [SWIG](http://www.swig.org) and a modified version of [ArmanPy](https://sourceforge.net/projects/armanpy/) for interoperability with NumPy. 
 
## Prerequisites

To compile, you will need to install:

- [Colormotor](https://github.com/colormotor/colormotor)
- [SWIG](http://www.swig.org)  
- [NumPy](http://www.scipy.org/install.html)

Ideally the repository directory should be located at the same level as colormotor, then you can generate the binding code with
```
sh swig.sh
```

This will generate the Python and C++ code necessary to call Colormotor functions from Python.

## Notes
### Armadillo/NumPy
The wrapper utilises a modified version of [ArmanPy](https://sourceforge.net/projects/armanpy/) to allow transparent conversion between Armadillo and NumPy arrays/matrices. The provided version of ArmanPy automatically converts NumPy arrays to the same memory layout as Armadillo (columns linear in memory, in NumPy terms FORTRAN-contiguous format), which may result in slower performance due to the memory copy, but simplifies integration and binding of Armadillo code in Python.
