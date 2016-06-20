const char* pytype_string(PyObject* py_obj);
const char* typecode_string(int typecode);
int type_match(int actual_type, int desired_type);
void free_cap(PyObject * cap);
PyArrayObject* obj_to_array_no_conversion(PyObject* input,
                                            int typecode);
PyArrayObject* obj_to_array_allow_conversion(PyObject* input,
                                               int       typecode,
                                               int*      is_new_object);
PyArrayObject* make_contiguous(PyArrayObject* ary,
                                 int*           is_new_object,
                                 int            min_dims,
                                 int            max_dims);
PyArrayObject* make_fortran(PyArrayObject* ary,
                              int*           is_new_object);
PyArrayObject* obj_to_array_contiguous_allow_conversion(PyObject* input,
                                                          int       typecode,
                                                          int*      is_new_object);
PyArrayObject* obj_to_array_fortran_allow_conversion(PyObject* input,
                                                       int       typecode,
                                                       int*      is_new_object);
int require_contiguous(PyArrayObject* ary);
int require_c_or_f_contiguous(PyArrayObject* ary);
int require_native(PyArrayObject* ary);
int require_dimensions(PyArrayObject* ary,
                         int            exact_dimensions);
 int require_dimensions_n(PyArrayObject* ary,
   int*           exact_dimensions,
   int            n);
 int require_size(PyArrayObject* ary,
                   npy_intp*      size,
                   int            n);
 int require_fortran(PyArrayObject* ary);
  