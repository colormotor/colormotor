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
 

#include "cmParamList.h"


namespace cm
{

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

Param* ParamList::addColor( const std::string& name, Color * address )
{
	Param * p = new Param();
	p->initColor(name,address);
	addParam(p);
	return p;
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


// FLOAT AND DOUBLE SHOULD PROBABLY BE INTERCHANGABLE?
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
            return children[i]->find(name);
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
			debugPrint("Could not find child %s\n", childName.c_str());
			return 0;
		}
	}
	return 0;
}

#ifdef USING_TINYXML

//////////////


TiXmlElement	*ParamList::createXML()
{

	TiXmlElement * root = new TiXmlElement("Params");//getName());
	root->SetAttribute("name",name.c_str());
	TiXmlElement * paramsNode = new TiXmlElement("Params");
	root->LinkEndChild(paramsNode);
	for( int i = 0; i < getNumParams(); i++ ) 
	{
	//	if(getVar(i)->getSerialize())
		{
			TiXmlElement * velem = getParam(i)->createXML();
			if(velem)
				paramsNode->LinkEndChild(velem);
		}
	}

	TiXmlElement * childrenNode = new TiXmlElement("Children");
	root->LinkEndChild(childrenNode);
	for( int i = 0; i < getNumChildren(); i++ )
	{
		TiXmlElement * velem = getChild(i)->createXML();
		if(velem)
			childrenNode->LinkEndChild(velem);
		
	}

	return root;
}


bool		ParamList::readXML( TiXmlElement * elem )
{
	TiXmlElement * ctrlelem;

	// compatibility route
	TiXmlElement * testElem = elem->FirstChildElement();

	if( testElem->ValueTStr() == "var")
	{
		int numElements = 0;
		for ( ctrlelem = elem->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
		{
			numElements ++;
		}
		
		// figure out if we have same number of params in xml
		// in that case ignore names and read sequentially
		bool match = numElements == getNumParams();
		int i = 0;
		for ( ctrlelem = elem->FirstChildElement(); ctrlelem != 0; ctrlelem = ctrlelem->NextSiblingElement()) 
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

	else
	// new method
	{
		{
		TiXmlElement * paramElement = elem->FirstChildElement("Params");
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
		TiXmlElement * childrenElement = elem->FirstChildElement("Children");
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

#endif

}


