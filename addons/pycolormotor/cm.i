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


%pythoncode %{

import numpy

def list_to_shape(L, close=True):
    ''' Converts a list of contours (specified as 2xN matrices) to a shape'''
    S = Shape()
    for c in L:
        S.add(Contour(c, close))
    return S

def shape_to_list(S):
    ''' Converts a shape to a list of contours
        if specified, performs a "safe" closing operation by first closing and then cleaning up '''
    L = []
    for c in S:
        P = numpy.array(c.points)
        L.append(P)
    return L


def shapeUnion( a, b, fillType=_cm.CLIP_NONZERO ):
    if type(a) == list:
        a = list_to_shape(a)
        b = list_to_shape(b)
        return shape_to_list( _cm.shapeUnion(a, b, fillType) )
    else:
        return _cm.shapeUnion(a, b, fillType)

def shapeDifference( a, b, fillType=_cm.CLIP_NONZERO ):
    if type(a) == list:
        a = list_to_shape(a)
        b = list_to_shape(b)
        return shape_to_list( _cm.shapeDifference(a, b, fillType) )
    else:
        return _cm.shapeDifference(a, b, fillType)

def shapeIntersection( a, b, fillType=_cm.CLIP_NONZERO ):
    if type(a) == list:
        a = list_to_shape(a)
        b = list_to_shape(b)
        return shape_to_list( _cm.shapeIntersection(a, b, fillType) )
    else:
        return _cm.shapeIntersection(a, b, fillType)

def shapeXor( a, b, fillType=_cm.CLIP_NONZERO ):
    if type(a) == list:
        a = list_to_shape(a)
        b = list_to_shape(b)
        return shape_to_list( _cm.shapeXor(a, b, fillType) )
    else:
        return _cm.shapeXor(a, b, fillType)

def shapeOffset( shape, offset, joinType=_cm.JOIN_ROUND, miterLimit=1., fillType=_cm.CLIP_NONZERO):
    if type(shape) == list:
        shape = list_to_shape(shape)
        return shape_to_list( _cm.shapeOffset(shape, offset, joinType, miterLimit, fillType) )
    else:
        return _cm.shapeOffset(shape, offset, joinType, miterLimit, fillType)

def fill(S):
    if type(S) == list:
        _cm.fill(list_to_shape(S))
    elif type(S) == numpy.ndarray:
        _cm.fill(Contour(S, True))
    else:
        _cm.fill(S)
        
def draw( S, closed=False ):
    if type(S) == list:
        _cm.draw(list_to_shape(S, closed))
    elif type(S) == numpy.ndarray:
        _cm.draw(S, closed)
    else:
        _cm.draw(S)

def drawRect(*args):
    if len(args) == 1:
        rect = args[0]
        if type(rect) == list or type(rect) == numpy.ndarray or type(rect) == tuple:
            size = rect[1] - rect[0]
            _cm.drawRect(rect[0][0], rect[0][1], size[0], size[1])
        else:
            _cm.drawRect(rect)
    else:
        _cm.drawRect(*args)

def fillRect(*args):
    if len(args) == 1:
        rect = args[0]
        if type(rect) == list or type(rect) == numpy.ndarray or type(rect) == tuple:
            size = rect[1] - rect[0]
            _cm.fillRect(rect[0][0], rect[0][1], size[0], size[1])
        else:
            _cm.fillRect(rect)
    else:
        _cm.fillRect(*args)



clrconv = {'r':[1,0,0,1], 
            'g':[0,1,0,1], 
            'b':[0,0,1,1], 
            'c':[0,0.5,1,1], 
            'm':[1,0,1,1], 
            'k':[0,0,0,1],
            'y':[1,1,0,1]}

def color(*args):
    if len(args) == 1:
        clr = args[0]
        if type(clr) == str:
            _cm.color(clrconv[clr])
        else:
            _cm.color(clr)
    else:
        _cm.color(*args)

%}