//(directors="1")
%module cm
#pragma SWIG nowarn=322,362
%{

#include "colormotor.h"
#include "cm_params.h"
// Safe defines
typedef cm::V2 ForceV2;

#define SWIG_FILE_WITH_INIT
#include "cm_imgui.h"

#ifdef check
	#undef check
#endif
    
%}

%include "armanpy.i"

%include <typemaps.i>
%include <stl.i>
%include <std_vector.i>
%include <std_string.i>
%include <std_pair.i>

#define V2 arma::vec
#define V3 arma::vec
#define V4 arma::vec
#define M44 arma::mat
#define M33 arma::mat
#define M22 arma::mat

%template(FloatVector) std::vector< float >;
%template(VecVector) std::vector<arma::vec>;
%template(MatVector) std::vector<arma::mat>;
%template(StringVector) std::vector<std::string>;
%template(ConstCharVector) std::vector<const char*>;

%include "wrappers/py_cm.h"
%include "wrappers/py_utils.h"
%include "wrappers/py_math.h"
%include "wrappers/py_gfx.h"
%include "wrappers/py_geom.h"
%include "wrappers/py_ui.h"

%extend cm::Shape
{
%insert("python") %{
    def __iter__(self):
    	for i in range(self.size()):
    		yield self.getContour(i)

    def __getitem__(self, i):
    	return self.getContour(i)

    def __len__(self):
    	return self.size()

%}
}

%extend cm::Contour
{
%insert("python") %{
    def __iter__(self):
    	for i in range(self.size()):
    		yield self.getPoint(int(i))

    def __getitem__(self, i):
    	return self.getPoint(int(i))

    def __len__(self):
    	return self.size()
    	
%}

#undef V2
#undef V3
#undef V4
#undef M44
#undef M33
#undef M22


}

