// Keep this private
#include "cm_params.h"


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
 



namespace cm
{

static float stringToFloat( const std::string & str )
{
	return (float)atof(str.c_str()); 
}

static int	stringToInt( const std::string & str )
{
	return (int)atoi(str.c_str());
}

static void stdPrintf( std::string & str, const char *pFormat,... )
{
	static char txt[4096];
	
	if( strlen(pFormat) > 4096 )
		return;
	
	va_list	parameter;
	va_start(parameter,pFormat);
	vsnprintf(txt,4096,pFormat,parameter);
	va_end(parameter);
	str = txt;
}

static bool testSeparators( char c, const std::string & separators )
{
	for( int i = 0; i < separators.length(); i++ )
		if(c == separators[i])
			return true;
	return false;
}

static std::vector <std::string> tokenize( const std::string & str , const std::string & separators  )
{
	std::vector <std::string> out;

	char token[512];

	char * c = (char*)&str[0];

	while( *c != '\0' ){
		int ind = 0;	

		while( testSeparators(*c, separators) ) //*c==separator || (*c==' ') )
			c++;

		while( !testSeparators(*c, separators) && *c!='\0' )
		{
			if(testSeparators(*c, separators))
				break;
			token[ind++] = *c++;
		}
		token[ind] = '\0';

		out.push_back(token);

		while( testSeparators(*c, separators)  && *c!='\0' )
			c++;
	}

	return out;
}

void XmlBase::saveXml( const std::string & fname )
{
	XMLDocument doc;
	
	XMLElement * root = createXML(doc) ;
	doc.LinkEndChild(root);
		
	doc.SaveFile( fname.c_str() );
}

/// load from xml file
bool XmlBase::loadXml( const std::string & fname )
{
	XMLDocument doc;
	
    doc.LoadFile(fname.c_str());
    int err = doc.ErrorID();
    if(err)
    {
        printf("Could not load %s\n", fname.c_str());
        return false;
    }
    
	XMLElement * pRoot = doc.FirstChildElement();//getName());
	if(!pRoot)
		return false;

	return readXML(pRoot);
}




#define CALL_LISTENERS( func )  for( int i = 0; i < listeners.size(); i++ ) listeners[i]->func(this);
#define DEFAULT_STR_SIZE	256

static const char* names[NUM_PARAM_TYPES+1] = 
{
	"FLOAT",
	"DOUBLE",
	"BOOL",
	"STRING",
	"CSTRING",
	"EVENT",
	"SELECTION",
	"INT",
	"VECTORf",
	"VECTORd",
	"COLOR",
	"UNKNOWN"
};

static const int sizes[NUM_PARAM_TYPES+1] =
{
	4,
	8,
	4,
	sizeof(std::string),
	DEFAULT_STR_SIZE,
	4,
	4,
	0
};

	
Param::Param()
{
	_type = PARAM_UNKNOWN;
	_addr = 0;
	_min = 0.0f;
	_max = 1.0f;
	_name = "unknown";
	_data = 0;
	
	_numElements = 1;
	
	refCount = 0;
	
	oscAddress = "/";
	
	options = "";
	
}

Param::~Param()
{
	CALL_LISTENERS(onParamDeleted)
	
	if(_data)
	{
		if(_type == PARAM_STRING)
		{
			std::string * s = (std::string*)_data;
			delete s;
		}
		/*else if(_type == PARAM_COLOR)
		{
			Color * c = (Color*)_data;
			delete c;
		}*/
		else
		{
			delete [] _data;
		}
	}
	_data = 0;
}

void Param::clone( const Param & param )
{
	_type = param._type;
	_addr = param._addr;
	_name = param._name;
	_min = param._min;
	_max = param._max;
	_default = param._default;
	
	switch( getType() )
	{
		case PARAM_BOOL:
		case PARAM_EVENT:
			setBool( getBool() );
			break;
			
		case PARAM_FLOAT:
			setFloat( getFloat() );
			break;
		case PARAM_DOUBLE:
			setDouble( getDouble() );
			break;
		case PARAM_INT:
		case PARAM_SELECTION:
			setInt( getInt() );
			break;
		case PARAM_STRING:
		case PARAM_CSTRING:
			setString( getString() );
			break;
		break;
		
	}
}

Param * Param::clone( bool keepAddress )
{
	Param * p = new Param();
	
	if( keepAddress )
	{
		p->_addr = _addr;
	}
	else
	{
		if( getType() == PARAM_STRING )
			p->_data = (char*)new std::string();
		else
			p->_data = new char[sizes[getType()]];
		
		p->_addr = p->_data;
	}
	
	switch( getType() )
	{
		case PARAM_BOOL:
		case PARAM_EVENT:
			p->setBool( getBool() );
			break;
			
		case PARAM_FLOAT:
			p->setFloat( getFloat() );
			break;
		case PARAM_DOUBLE:
			p->setDouble( getDouble() );
			break;
		case PARAM_INT:
		case PARAM_SELECTION:
			p->setInt( getInt() );
			break;
		case PARAM_STRING:
		case PARAM_CSTRING:
			p->setString( getString() );
			break;
	}
	
	p->_type = _type;
	p->_min = _min;
	p->_max = _max;
	p->_default = _default;
	p->_numElements = _numElements;
	p->_name = _name;
	return p;
}

void Param::initDouble( const std::string& name, double * addr, double min , double max  )
{
	_type = PARAM_DOUBLE;
	_addr = addr;
	if(!_addr)
	{
		_data = new char[8];
		_addr = _data;
		setDouble( 0.0 );
	}
	
	
	_name = name;

	_min = min;
	_max = max;
	_default = getDouble();
}


void Param::initFloat( const std::string& name, float * addr, float min , float max  )
{
	_type =  PARAM_FLOAT;
	_addr = addr;
	if(!_addr)
	{
		_data = new char[4];
		_addr = _data;
		setFloat( 0.0 );
	}

	_name = name;

	_min = min;
	_max = max;
	_default = getFloat();
}

void Param::initBool( const std::string& name, bool * addr )
{
	_addr = addr;
	
	if(!_addr)
	{
		_data = new char[sizeof(bool)];
		_addr = _data;
		setBool(false);
	}
	
	_type = PARAM_BOOL;
	_name = name;
}

void Param::initInt( const std::string& name, int * addr )
{
	_addr = addr;
		
	if(!_addr)
	{
		_data = new char[sizeof(int)];
		_addr = _data;
		setInt(0);
	}
	
	_type = PARAM_INT;
	_name = name;
}

void Param::initEvent( const std::string& name, bool * addr )
{
	_addr = addr;
	
	if(!_addr)
	{
		_data = new char[sizeof(bool)];
		_addr = _data;
		setBool(false);
	}
	
	_type = PARAM_EVENT;
	_name = name;
}

void Param::initCString( const std::string& name, char * addr )
{
	_addr = addr;
	
	if(!_addr)
	{
		_data = new char[DEFAULT_STR_SIZE]; 
		_addr = _data;
		setString("");
	}
	
	_type = PARAM_CSTRING;
	_name = name;
}

void Param::initString( const std::string& name, std::string * addr )
{
	_addr = addr;
	
	if(!_addr)
	{
		_data = (char*)new std::string();
		_addr = _data;
		setString("");
	}
	
	_type = PARAM_STRING;
	_name = name;
}


void Param::initSelection(  const std::string& name, std::string * names, int * addr, int numSelections )
{
	_selectionNames = names;
	//_numSelections = numSelections;
	_numElements = numSelections;
	_addr = addr;
	_type = PARAM_SELECTION;
	_name = name;
	_default = *addr;
}

void Param::initFloatArray( const std::string& name, float * addr, int numElements, float min , float max  )
{
	_addr = addr;
	
	if(!_addr)
	{
		_data = new char[numElements*sizeof(float)]; 
		_addr = _data;
		memset(_data,0,numElements*sizeof(float));
	}
	
	_type = PARAM_FLOAT;
	_name = name;
	_min = min;
	_max = max;
	_default = 0.0f;
	_numElements = numElements;
}


void Param::setDouble( double val, int index, bool bInformListeners )
{
	if( _type == PARAM_FLOAT )
	{
		setFloat(val,index);
		return;
		
	}
	if( _type != PARAM_DOUBLE || index > _numElements )
		return;
		
	((double*)_addr)[index] = val;
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}


void Param::setFloat( float val, int index, bool bInformListeners )
{
	if( _type == PARAM_DOUBLE)
	{
		setDouble(val,index);
		return;
		
	}
	
	if( _type != PARAM_FLOAT || index > _numElements )
		return;
		
	((float*)_addr)[index] = val;
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

void Param::setBool( bool val, bool bInformListeners )
{
	if( _type != PARAM_BOOL && _type != PARAM_EVENT )
		return;

	*((bool*)_addr) = val;
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

void Param::setInt( int val , bool bInformListeners)
{
	if( _type != PARAM_INT && _type != PARAM_SELECTION )
		return;

	*((int*)_addr) = val;
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

void Param::setString( const std::string& val, bool bInformListeners )
{
	if( _type != PARAM_STRING && _type != PARAM_CSTRING )
		return;

	if( _type == PARAM_CSTRING )
	{
		if(_data)
		{
			strncpy((char*)_addr, val.c_str(),256);
			_data[255] = '\0';
		}
		else	
			strcpy((char*)_addr, val.c_str());
	}
	else
	{
		std::string * s = (std::string*)_addr;
		(*s) = val;
	}
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

void Param::setVectorf( const std::vector<float>& v, bool bInformListeners )
{
	if( _type != PARAM_VECTORf )
		return;

	*((std::vector<float>*)_addr) = v;

	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

void Param::setVectord( const std::vector<double>& v, bool bInformListeners )
{
	if( _type != PARAM_VECTORd )
		return;

	*((std::vector<double>*)_addr) = v;

	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}

#ifdef IN_COLORMOTOR
void Param::initColor( const std::string& name, V4* addr )
{
	_addr = addr;
	_numElements = 4;
	if(!_addr)
	{
		_data = (char*)new V4(0,0,0,0);//char[4*sizeof(float)]; 
		_addr = _data;
		//memset(_data,0,4*sizeof(float));
	}

	_type = PARAM_COLOR;
	_name = name;
}

void Param::setColor( const V4& clr, bool bInformListeners )
{
	if( _type != PARAM_COLOR )
		return;

	V4 * c = (V4*)_addr;
	*c = clr;
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}


V4  Param::getColor()
{
	if( _type != PARAM_COLOR)
		return V4(0,0,0,0);
	V4 * c =  (V4*)_addr;
	return *c;
}

#endif

void Param::setFloatArray( float * buf, bool bInformListeners )
{
	if( _type != PARAM_FLOAT || _numElements <= 1 )
		return;
		
	memcpy( _addr, buf, sizeof(float)*_numElements );
	
	CALL_LISTENERS(onParamChanged)
}

void Param::setFloatArray01( float * buf, bool bInformListeners )
{
	if( _type != PARAM_FLOAT || _numElements <= 1 )
		return;
		
	float * faddr = (float*)_addr;
	for( int i = 0; i < _numElements; i++ )
	{
		faddr[i] = _min + buf[i]*(_max-_min);;
	}
	
	if(bInformListeners)
	{
		CALL_LISTENERS(onParamChanged)
	}
}
	
double Param::getDouble() const
{
	if( _type != PARAM_DOUBLE )
		return 0.0;
	
	return *((double*)_addr);
}

float Param::getFloat() const
{
	if( _type != PARAM_FLOAT )
		return 0.0f;
	
	return *((float*)_addr);
}

bool  Param::getBool() const
{
	if( _type != PARAM_BOOL && _type != PARAM_EVENT )
		return false;
	
	return *((bool*)_addr);
}

int  Param::getInt()  const
{
	if( _type != PARAM_INT && _type != PARAM_SELECTION )
		return false;
	
	return *((int*)_addr);
}

const char* Param::getString() const
{
	switch( _type )
	{
		case PARAM_CSTRING:
			return (const char*)_addr;
		case PARAM_STRING:
			return ((std::string*)_addr)->c_str();
			
		case PARAM_SELECTION:
			return _selectionNames[ getInt() ].c_str();
			
		default:
			return 0;// this could return a string version of parameter
	}
}

const std::vector<float> &Param::getVectorf() const
{
	static std::vector<float> none;
	if( _type != PARAM_VECTORf )
		return none;
	return *((std::vector<float>*)_addr);
}

const std::vector<double> &Param::getVectord() const
{
	static std::vector<double> none;
	if( _type != PARAM_VECTORd )
		return none;
	return *((std::vector<double>*)_addr);
}

bool Param::isName( const std::string& str ) const
{
	return str == getName();
}



///   Get var type  given a string
PARAM_TYPE  Param::getTypeFromString( const char * str  )
{
	
	for( int i = 0; i < NUM_PARAM_TYPES; i++ )
		if(strcmp(names[i],str)==0)
			return (PARAM_TYPE)i;

	return PARAM_UNKNOWN;
}


const char * Param::getTypeName( PARAM_TYPE type )
{
	if(type == PARAM_UNKNOWN)
		return names[NUM_PARAM_TYPES];
		
	return names[type];
}	


Param* Param::addListener( ParamListener * listener )
{
    for( auto it = listeners.begin(); it != listeners.end(); it++ )
    {
        if( *it == listener )
        {
            return this;
        }
    }
    
    listeners.push_back(listener);
	return this;
}

void Param::removeListener( ParamListener * listener )
{
    for( auto it = listeners.begin(); it != listeners.end(); it++ )
    {
        if( *it == listener )
        {
            listeners.erase(it);
            return;
        }
    } 
}

void Param::clearListeners()
{
	listeners.clear();
}

void Param::valueChanged( int flags )
{
	CALL_LISTENERS(onParamChanged)
}

void Param::setOscAddress( const std::string & str )
{
	oscAddress = str;
}

std::string Param::getOscAddress() const
{
	return oscAddress+getName();
}

XMLElement	*Param::createXML(XMLDocument& doc)
{
	if(getType()==PARAM_UNKNOWN)
		return 0;

	if( hasOption("n") )
		return 0;
	
	XMLElement * node = doc.NewElement("var");
	std::string value;

	node->SetAttribute("type",Param::getTypeName(getType()));

	switch(this->getType())
	{
	case PARAM_FLOAT:
	case PARAM_DOUBLE:
		node->SetAttribute("name",getName());
		stdPrintf(value,"%g",getFloat());
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
	case PARAM_SELECTION:
	case PARAM_INT:
		node->SetAttribute("name",getName());
		stdPrintf(value,"%d",getInt());
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
 	case PARAM_BOOL:
	case PARAM_EVENT:
		node->SetAttribute("name",getName());
		stdPrintf(value,"%d",getBool());
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
	case PARAM_STRING:
		node->SetAttribute("name",getName());
		stdPrintf(value,"%s",getString());
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
	case PARAM_CSTRING:
		node->SetAttribute("name",getName());
		stdPrintf(value,"%s",getString());
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
	case PARAM_VECTORf:
	case PARAM_VECTORd:
		assert(false);
		// TODO
		break;

#ifdef IN_COLORMOTOR
	case PARAM_COLOR:
	{
		node->SetAttribute("name",getName());
		V4 clr = getColor();
		stdPrintf(value,"%g %g %g %g",clr.x, clr.y, clr.z, clr.w);
		node->LinkEndChild( doc.NewText(value.c_str()) );
		break;
	}
#endif

	}
	
	if( getData() )
		node->LinkEndChild( getData()->createXML(doc) );
	return node;

}

bool	Param::readXML( XMLElement * elem )
{
	std::string name = elem->Attribute("name");
	std::string type = elem->Attribute("type");

	PARAM_TYPE t = Param::getTypeFromString(type.c_str());
	if(t==PARAM_UNKNOWN)
		return false;
	if( hasOption("n") )
		return false;
	
	std::string val = "";
	if (elem->GetText())
		val = elem->GetText();
	
	//_name = name;

	std::vector <float> floats;

	switch(t)
	{
		case PARAM_FLOAT:
		case PARAM_DOUBLE:
			setFloat(atof(val.c_str()));
			break;
		case PARAM_INT:
			setInt((int)atoi(val.c_str()));
			break;
		case PARAM_SELECTION:
			setInt((int)atoi(val.c_str()));
			break;
		case PARAM_BOOL:
		//case PARAM_EVENT:
			setBool((int)atoi(val.c_str()));
			break;
		case PARAM_STRING:
		case PARAM_CSTRING:
			setString(val.c_str());
			break;	
		case PARAM_VECTORf:
		case PARAM_VECTORd:
			assert(false);
			// TODO
			break;
	#ifdef IN_COLORMOTOR
		case PARAM_COLOR:
		{
			std::vector<std::string> toks = split(val," ");
			assert(toks.size()==4);
			V4 c;
			for( int i = 0; i < 4; i++ )
				c[i] = atof(toks[i].c_str());
			setColor(c);
		}
			break;
	#endif
	}

	if(getData())
	{
		XMLElement * elm = elem->FirstChildElement();
		if(elm)
			getData()->readXML(elm);
	}
	
	return true;
} 

bool Param::hasOption( const std::string & opt )
{
	return options.find( opt ) != std::string::npos;
}

void Param::setPath( const std::string & path )
{
	this->_path = path;
}



ParamList::ParamList( const std::string & name )
:
name(name),
current(this),
parent(0)
{
}

ParamList::ParamList( const ParamList & mom )
:
name(mom.name)
{
	release();
	for( int i = 0; i < mom.getNumParams(); i++ )
	{
		addParam( getParam(i)->clone(true) );
	}
}


ParamList::~ParamList()
{
	release();
}

void ParamList::release()
{
	for( int i = 0; i < params.size(); i++ )
	{
		if( params[i]->refCount <= 1 )
			delete params[i];
		else
			params[i]->refCount--;
	}
			
	params.clear();
	paramMap.clear();
	paramsThatHaveChanged.clear();
	children.clear();
}

void ParamList::addParam( Param * param )
{
	param->refCount++;
	current->params.push_back(param);
	current->paramMap[param->getName()] = param;

	std::string path = current->getPath() + ".";
	path += param->getName();
	param->setPath(path);
}

std::string ParamList::getPath() const
{
	std::string path="";
	if(parent)
	{
		path = parent->getPath();
		if(path.length())
			path += ".";
		path += name;
	}

	return path;
}

ParamList * ParamList::getChild( const std::string & childName )
{
	for( int i = 0; i < children.size(); i++ )
		if( children[i]->getName() == childName )
			return children[i];
	return 0;
}

void ParamList::addChild( ParamList * params ) 
{ 
	children.push_back(params); 
	params->parent = this; 
}

void ParamList::newChild( const std::string & name )
{
	ParamList * plist = new ParamList(name);
	addChild(plist);
	current = plist;
}

void ParamList::setCurrent( const std::string & name )
{
	ParamList * plist = getChild(name);
	if(plist)
		current = plist;
	else
		current = this;
}

void ParamList::resetCurrent() 
{ 
	current = this; 
}


void ParamList::addSpacer()
{
	Param * p = new Param();
	addParam(p);
}

void ParamList::addParams( const ParamList & plist )
{
	for( int i = 0; i < plist.getNumParams(); i++ )
		addParam(plist.getParam(i));
}

Param* ParamList::addFloatArray( const std::string& name, float * address, int numElements, float min , float max  )
{
	Param * p = new Param();
	p->initFloatArray(name,address,numElements,min,max);
	addParam(p);
	return p;
}

Param* ParamList::addFloat( const std::string& name, float * address, float min , float max )
{
	Param * p = new Param();
	p->initFloat(name,address,min,max);
	addParam(p);
	return p;
}

Param* ParamList::addDouble( const std::string& name, double * address, double min , double max  )
{
	Param * p = new Param();
	p->initDouble(name,address,min,max);
	addParam(p);
	return p;
}

Param* ParamList::addBool( const std::string& name, bool * address )
{
	Param * p = new Param();
	p->initBool(name,address);
	addParam(p);
	return p;
}

Param* ParamList::addInt( const std::string& name, int * address )
{
	Param * p = new Param();
	p->initInt(name,address);
	addParam(p);
	return p;
}

Param* ParamList::addEvent( const std::string& name, Trigger<bool> & trigger  )
{
	return addEvent(name,&trigger.val);
}


Param* ParamList::addEvent( const std::string& name, bool * address )
{
	Param * p = new Param();
	p->initEvent(name,address);
	addParam(p);
	return p;
}


Param* ParamList::addCString( const std::string& name, char * address )
{
	Param * p = new Param();
	p->initCString(name,address);
	addParam(p);
	return p;
}

Param* ParamList::addString( const std::string& name, std::string * address )
{
	Param * p = new Param();
	p->initString(name,address);
	addParam(p);
	return p;
}

Param* ParamList::addSelection( const std::string& name, std::string * names, int * addr, int numSelections )
{
	Param * p = new Param();
	p->initSelection(name,names,addr,numSelections);
	addParam(p);
	return p;
}

void ParamList::addSeparator()
{
	addSpacer();
}

Param*  ParamList::addFloat( const std::string& name, float value, float min, float max )
{
	Param * p = addFloat(name,(float*)0,min,max);
	p->setFloat(value);
	return p;
}

Param*  ParamList::addBool( const std::string& name, bool value )
{
	Param * p = addBool(name,(bool*)0);
	p->setBool(value);
	return p;
}

Param*  ParamList::addInt( const std::string& name, int value )
{
	Param * p = addInt(name,(int*)0);
	p->setInt(value);
	return p;
}

Param*  ParamList::addString( const std::string& name, const std::string & value )
{
	Param * p = addString(name,(std::string*)0);
	p->setString(value);
	return p;
}

bool ParamList::setString( const std::string& name, const std::string & str, bool inform )
{
	Param * p = find( name );
	if(!p)
		return false;
	
	p->setString(str,inform);
	return true;
}
	
bool ParamList::setFloat( const std::string& name, float v, bool inform )
{
	Param * p = find( name );
	if(!p || !p->isReal() )
		return false;
		
	p->setFloat(v,0,inform);
	return true;
}

bool ParamList::setDouble( const std::string& name, double v, bool inform )
{
	Param * p = find( name );
	if(!p || !p->isReal() )
		return false;
		
	p->setDouble(v,inform);
	return true;
}

bool ParamList::setBool( const std::string& name, bool v, bool inform )
{
	Param * p = find( name );
	if(!p || p->getType() != PARAM_BOOL )
		return false;
		
	p->setBool(v,inform);
	return true;
}

bool ParamList::setInt( const std::string& name, int v, bool inform )
{
	Param * p = find( name );
	if(!p || p->getType() != PARAM_INT )
		return false;
		
	p->setInt(v,inform);
	
	return true;
}

#ifdef IN_COLORMOTOR
Param* ParamList::addColor( const std::string& name, V4* address )
{
	Param * p = new Param();
	p->initColor(name, address);
	addParam(p);
	return p;
}

Param* ParamList::addColor( const std::string& name, const V4& v )
{
	Param * p = addColor(name,(V4*)0);
	p->setColor(v);
	return p;
}
#endif


bool ParamList::remove( Param * v )
{
	// remove from map
	{
		ParamMap::iterator it = paramMap.find(v->getName());
		if( it != paramMap.end() )
		{
			paramMap.erase(it);
		}
	}
	
	// remove param
	{
		std::vector<Param*>::iterator it;

		for( it = params.begin(); it != params.end(); it++ )
		{
			if( *it == v )
			{
				params.erase(it);
				delete v;
				return true;
			}
		} 
	}
	
	return false;
}

static std::string untokenizeRest( const std::vector<std::string> & toks )
{
	std::string res = "";
	for( int i = 1; i < toks.size(); i++ )
	{
		res += toks[i];
		if( i != (toks.size()-1) )
			res += ".";
	}

	return res;
}

Param * ParamList::find( const std::string& name_ )
{
	/* Must find another way since when parameter name changes paramMap must be updated!
	ParamMap::iterator it = paramMap.find(name);
	
	if( it != paramMap.end() )
		return it->second;
	
	return 0;
	*/
	std::vector<std::string> toks = tokenize(name_,".");
	
	if(!toks.size())
	{
		assert(0);
		return 0;
	}

	if(toks.size() == 1)
	{
		std::string name = toks[0];
		for( int i = 0; i < getNumParams(); i++ )
		{
			if( params[i]->isName(name) ) 
				return params[i];
		}
        

        // TODO figure this out properly
        for( int i = 0; i < getNumChildren(); i++ )
        {
            Param * p = children[i]->find(name);
            if(p)
                return p;
        }
	}
	else
	{
		std::string childName = toks[0];
		ParamList * child = getChild(childName);
		if(child)
			return child->find( untokenizeRest(toks) );
		else
		{
			printf("Could not find child %s\n", childName.c_str());
			return 0;
		}
	}
	return 0;
}

Param * ParamList::findSafe( const std::string& name_ )
{
    static Param safey;
    Param * p = find(name_);
    if(p)
        return p;
    return &safey;
}

    
//////////////

XMLElement	*ParamList::createXML(XMLDocument& doc)
{

	XMLElement * root = doc.NewElement("Params");//getName());
	root->SetAttribute("name",name.c_str());
	XMLElement * paramsNode = doc.NewElement("Params");
	root->LinkEndChild(paramsNode);
	for( int i = 0; i < getNumParams(); i++ ) 
	{
	//	if(getVar(i)->getSerialize())
		{
			XMLElement * velem = getParam(i)->createXML(doc);
			if(velem)
				paramsNode->LinkEndChild(velem);
		}
	}

	XMLElement * childrenNode = doc.NewElement("Children");
	root->LinkEndChild(childrenNode);
	for( int i = 0; i < getNumChildren(); i++ )
	{
		XMLElement * velem = getChild(i)->createXML(doc);
		if(velem)
			childrenNode->LinkEndChild(velem);
		
	}

	return root;
}


bool		ParamList::readXML( XMLElement * elem )
{
	XMLElement * ctrlelem;

	// compatibility route
	XMLElement * testElem = elem->FirstChildElement();

    {
    XMLElement * paramElement = elem->FirstChildElement("Params");
    if(!paramElement)
    {
        assert(0);
        return false;
    }

    int numElements = 0;
    for ( ctrlelem = paramElement->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
    {
        numElements ++;
    }

    // figure out if we have same number of params in xml
    // in that case ignore names and read sequentially
    bool match = false; //numElements == getNumParams();
    int i = 0;
    for ( ctrlelem = paramElement->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
    {
        std::string name = ctrlelem->Attribute("name");
        Param * v;
        
        if(match)
            v = params[i++]; 
        else
            v = find(name.c_str());
            
        if(v)
        {
            v->readXML(ctrlelem);
        }
    }
    }

    {
    XMLElement * childrenElement = elem->FirstChildElement("Children");
    if(!childrenElement)
    {
        assert(0);
        return false;
    }

    int numElements = 0;
    for ( ctrlelem = childrenElement->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
    {
        numElements ++;
    }
    

    // figure out if we have same number of params in xml
    // in that case ignore names and read sequentially
    bool match = false; //numElements == getNumChildren();
    int i = 0;
    for ( ctrlelem = childrenElement->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
    {
        const char * name = ctrlelem->Attribute("name");
        if(!name)
            continue;
        
        ParamList * v = getChild(name);
        
        if(v)
        {
            v->readXML(ctrlelem);
        }
    }
    }

	return true;
} 


void ParamList::addListener( ParamListener * l )
{
	for( int i = 0; i < getNumParams(); i++ )
		getParam(i)->addListener(l);
}
	
ParamList* ParamList::clone( bool keepAddresses )
{
	ParamList * d = new ParamList();
	
	for( int i = 0; i < getNumParams(); i++ )
	{
		d->addParam( getParam(i)->clone( keepAddresses ) );
	}

	for( int i = 0; i < getNumChildren(); i++ )
	{
		ParamList * l = getChild(i)->clone( keepAddresses );
		d->addChild(l);
	}

	return d;
}



}


