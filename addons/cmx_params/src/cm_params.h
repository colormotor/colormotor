/********************************************************************
 --------------------------------------------------------------------
 --           _,(_)._
 --      ___,(_______).      ____
 --    ,'__.           \    /\___\-.
 --   /,' /             \  /  /     \
 --  | | |              |,'  /       \
 --   \`.|                  /    ___|________
 --    `. :           :    /     COLORMOTOR
 --      `.            :.,'        Graphics and Multimedia Framework
 --        `-.________,-'          © Daniel Berio
 --								   http://www.enist.org
 --								   drand48@gmail.com
 --
 --------------------------------------------------------------------
 ********************************************************************/

#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
#include <errno.h>
#include <functional>

#include "tinyxml2/tinyxml2.h"
using namespace cmtinyxml2;

#define IN_COLORMOTOR

#ifdef IN_COLORMOTOR
	#include "cm.h"
	#include "cm_utils.h"
#endif

namespace cm
{

/// Trigger, a class that checks if a value has changed.
template <class Type> struct Trigger
{
	Trigger( Type v=0 ) { oldVal = val = v; }
	
	/// Will return true if the value has changed
	bool isTriggered()
	{
		bool res = false;
		if( oldVal != val )
		{
			res = true;
			oldVal = val;
		}
			
		return res;
	}

	Type operator = ( Type v ) { val = v; }

	// hacky will work mostly with bools
	void trigger() { if(val!=0) val = 0; else val = 1; }
	
	void reset( Type v ) 
	{
		oldVal = val = v;
	}
	
	void reset()
	{
		oldVal = val;
	}
	
	Type oldVal;
	Type val;
};

enum PARAM_TYPE
{
	PARAM_UNKNOWN  = -1,
	PARAM_FLOAT = 0,
	PARAM_DOUBLE = 1,
	PARAM_BOOL,
	PARAM_STRING,
	PARAM_CSTRING,
	PARAM_EVENT,
	PARAM_SELECTION,
	PARAM_INT,
	PARAM_VECTORf,
	PARAM_VECTORd,
	PARAM_COLOR,
	NUM_PARAM_TYPES
};

class Param;

class ParamListener
{
public:
	virtual ~ParamListener() {}
	
	virtual void onParamChanged( Param * p ) = 0;
	virtual void onParamDeleted( Param * p ) {}
	
};

/// A base type for custom data contained in a param
class ParamData
{
public:
	virtual ~ParamData(){ data = 0; }
	
	XMLElement	*createXML(XMLDocument& doc) { return doc.NewElement("EmptyValueData"); }
	bool	readXML( XMLElement * elem ) { return false; }
			
	void *data;
};

class XmlBase
{
public:
	virtual ~XmlBase() {}
	// implement these to get tyxml functionality
	virtual XMLElement	*createXML(XMLDocument& doc) = 0;
	virtual bool	readXML( XMLElement * elem ) = 0;

	/// save to xml file
	void saveXml( const std::string & fname );
	/// load from xml file
	bool loadXml( const std::string & fname );
};


class Param 
: public XmlBase
{
public:
	Param();
	virtual ~Param();
	
	Param * clone( bool keepAddress = false );
	void clone( const Param & p );
	
	////////////////////////////////////////////
	// initialization funcs
	void initFloatArray( const std::string& name, float * addr, int numElements, float min = 0.0f, float max = 1.0f );
	void initDouble( const std::string& name, double * addr, double min = 0.0, double max = 1.0 );
	void initFloat( const std::string& name, float * addr, float min = 0.0f, float max = 1.0f );
	void initBool( const std::string& name, bool * addr );
	void initInt( const std::string& name, int * addr );
	void initEvent( const std::string& name, bool * addr );
	void initString( const std::string& name, std::string * addr );
	void initCString( const std::string& name, char * addr );


	// note. this could be better done by handling arrays of values...
	void initSelection(  const std::string& name, std::string* names, int * addr, int numSelections );
	
	////////////////////////////////////////////
	// setters
	void setFloatArray( float * buf, bool bInformListeners=true );
	void setFloatArray01( float * buf, bool bInformListeners=true );
	
	void setDouble( double val, int index = 0, bool bInformListeners=true );
	void setFloat( float val, int index = 0, bool bInformListeners=true );
	void setBool( bool val, bool bInformListeners=true );
	void setString( const std::string& val, bool bInformListeners=true );
	void setInt( int val, bool bInformListeners=true );
	
	void setVectorf( const std::vector<float> &v, bool bInformListeners=true );
	void setVectord( const std::vector<double> &v, bool bInformListeners=true );
	
#ifdef IN_COLORMOTOR
	void initColor( const std::string& name, V4* addr );
	void setColor( const V4& clr, bool bInformListeners = true );
	V4 getColor();
#endif
	////////////////////////////////////////////
	// getters 

	double getDouble() const;
	float getFloat() const;
	bool  getBool() const;
	int	  getInt() const;
	const char* getString() const;
	const std::vector<float>& getVectorf() const;
	const std::vector<double>& getVectord() const;
	
	// float param specific
	float getMin() const { return _min; }
	float getMax() const { return _max; }
	float getDefault() const { return _default; }
	
    bool isDirty() { bool o=dirty; dirty=false; return o; }
    
	// selector param specific
	std::string * getSelectionNames() const { return _selectionNames; }
	
	/// Is parameter an Array
	bool isArray() const { return _numElements > 1; }
	
	/// Is parameter a real number
	bool isReal() const { return _type == PARAM_FLOAT || _type == PARAM_DOUBLE; }
	
	/// Get number of elements in param. 1 unless param is an Array
	int		getNumElements() const { return _numElements; }
	
	void *getAddress() { return _addr; }
	
	PARAM_TYPE getType() const { return _type; }
	
	const char * getName() const { return _name.c_str(); }
	bool isName( const std::string& str ) const;
	
	static PARAM_TYPE  getTypeFromString( const char * str  );
	static const char * getTypeName( PARAM_TYPE type );
	
	int refCount;
	
	Param* addListener( ParamListener * listener );
	void removeListener( ParamListener * listener );
	void clearListeners();
	void valueChanged( int flags = 0 );
	
	virtual ParamData* getData() { return 0; }
	
	XMLElement	*createXML(XMLDocument& doc);
	bool	readXML( XMLElement * elem );
	
	void setOscAddress( const std::string & str );
	std::string getOscAddress() const;
	
	const std::string & getOptions() const { return options; }
	Param* setOptions( const std::string & opt ) { options = opt; return this; }
	Param* appendOption( const std::string & opt ) { options += opt; return this; }
	bool hasOption( const std::string & opt );
	
    Param* sameLine() { return appendOption("sameline"); }
    Param* noGui() { return appendOption("h"); }
    
	const std::string & getPath() const { return _path; }
	void setPath( const std::string & path );
    
    bool dirty=false;
protected:
	std::string oscAddress;
	std::string options;
	
	
	
	std::vector <ParamListener*> listeners;
	std::string _name;
	std::string _path;
	char *_data;
	void * _addr;
	
	// just for float parameters
	double _min;
	double _max;
	double _default;
	
	int _numElements;
	
	// just for selector param
	std::string * _selectionNames;
	
	PARAM_TYPE _type;
    

};


class ParamList
:
public XmlBase
{
public:
	ParamList( const std::string & name = "Params" );
	ParamList( const ParamList & mom );

	~ParamList();
	
	void release();	

	ParamList * clone(  bool keepAddresses = false  );
	
	int	getNumParams() const { return params.size(); }
	Param * getParam( int index ) const { if( index < (int)params.size() ) return params[index]; else return 0; }
	
	void addParams( const ParamList & list );
	void addParam( Param * param );
	
	std::string getPath() const;
	ParamList* getChild( const std::string & name );
	ParamList * getChild( int i ) { return children[i]; }
	ParamList * getParent() { return parent; }

	void newChild( const std::string & name );
	void setCurrent( const std::string & name );
	void resetCurrent();
	
	void addChild( ParamList * params );
	int getNumChildren() const { return children.size(); }
	
	Param* addFloatArray( const std::string& name, float * address, int numElements, float min = 0.0f, float max = 1.0f );
	Param* addDouble( const std::string& name, double * address, double min = 0.0f, double max = 1.0f );
	Param* addFloat( const std::string& name, float * address, float min = 0.0f, float max = 1.0f );
	Param* addEvent( const std::string& name, bool * address );
	Param* addEvent( const std::string& name, Trigger<bool> & trigger );
	Param* addBool( const std::string& name, bool * address );
	Param* addInt( const std::string& name, int * address );
	Param* addCString( const std::string& name, char * address );
	Param* addString( const std::string& name, std::string * address );
	Param* addSelection( const std::string& name, std::string * names, int * addr, int numSelections );

	void addSpacer();
	void addSeparator();
	
	Param*  addFloat( const std::string& name, float value, float min = 0.0f, float max = 1.0f );
	Param*  addBool( const std::string& name, bool value );
	Param*  addInt( const std::string& name, int value );
	Param*  addString( const std::string& name, const std::string & value );

	bool setString( const std::string &name, const std::string & v, bool inform = true );
	bool setFloat( const std::string& name, float v, bool inform = true );
	bool setDouble( const std::string& name, double v, bool inform = true );
	bool setBool( const std::string& name, bool v, bool inform = true );
	bool setInt( const std::string& name, int v, bool inform = true );

#ifdef IN_COLORMOTOR
	Param* addColor( const std::string& name, V4* address );
	Param* addColor( const std::string& name, const V4& v );
#endif

	void addListener( ParamListener * l );
	
	bool isDirty();

	const std::string &getName() const { return name; }
	void setName( const std::string & name ) { this->name = name; }

	// TODO addSpacer
	bool remove( Param * v );
	Param * find(const std::string& name );
	Param * findSafe(const std::string& name );
	
	XMLElement	*createXML(XMLDocument& doc);
	bool	readXML( XMLElement * elem );
	
	std::vector<Param*> params;
	std::vector<Param*> paramsThatHaveChanged;
	
	typedef std::map<std::string,Param*> ParamMap;
	ParamMap paramMap; /// < TODO: use hash_map?
	
	std::string name;

	std::vector<ParamList*> children;
	ParamList * parent;


	ParamList * current;
};

}


