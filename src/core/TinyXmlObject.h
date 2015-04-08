

#pragma once

#include "../libs/tinyxml/tinyxml.h"

namespace cm
{

class TinyXmlObject
{
public:
	virtual ~TinyXmlObject() {}
	
	// implement these to get tyxml functionality
	virtual TiXmlElement	*createXML() = 0;
	virtual bool	readXML( TiXmlElement * elem ) = 0;

	/// save to xml file
	void saveXmlFile( const std::string & fname )
	{
		TiXmlDocument doc;
		TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
		doc.LinkEndChild( decl );
		
		TiXmlElement * root = createXML() ;
		doc.LinkEndChild(root);
			
		doc.SaveFile( fname.c_str() );
	}

	/// load from xml file
	bool loadXmlFile( const std::string & fname )
	{
		TiXmlDocument doc( fname.c_str() );
		bool res = doc.LoadFile();

		if(!res)
		{
			return false;
		}
		
		TiXmlElement * pRoot = doc.FirstChildElement();//getName());
		if(!pRoot)
			return false;

		return readXML(pRoot);
	}

	static void saveElementToFile( const std::string & path, TiXmlElement * elem )
	{
		TiXmlDocument doc( path.c_str() );
    	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    	doc.LinkEndChild( decl );
    	doc.LinkEndChild(elem);
    	doc.SaveFile();
	}

};

}