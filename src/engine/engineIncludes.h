/*
 *  engine/engineIncludes.h
 *  rendererTest
 *
 *  Created by ensta on 7/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "cmCore.h"
#include "cmMath.h"
#include "cmGfx.h"
 
#include <vector>

namespace cm
{

class Base : public RefCounter, public RttiObject, public TinyXmlObject
{
public:
	Base()
	:
	identifier(-1),
	label("Base")
	{
	}

	~Base()
	{
		release();
	}

	virtual void	release()
	{
	}

	virtual	void			setLabel( const char * label_ ) { label = label_; }
	const std::string&		getLabel() const { return label; }

	void					setId( int id_ ) { identifier = id_; }
	int						getId() const { return identifier; }
	
	TiXmlElement	*createXML() { return 0; }
	bool	readXML( TiXmlElement * elem ) { return true; }
	
protected:
	std::string label;
	int identifier;
};

template <class T> class HasChildren
{
public:

	HasChildren()
	{
		clearChildren();
	}

	virtual ~HasChildren()
	{
		clearChildren();
	}


	void	clearChildren() 
	{ 
		children.clear();
	}

	T*					getChild( int i ) { return children[i].ptr; } 
	virtual int			addChild( T * in ) { children.push_back(in); in->setParent((T*)this); return children.size()-1; }

	void				setParent( T * in ) { parent = in; }
	T *					getParent() const { return parent; }

	int					getNumChildren() const { return children.size(); }

	bool				hasChildren() const { return getNumChildren(); }

private:
	std::vector< RefPtr<T> >	children;
	T					*parent;
};


/// A group must be of CMBase derived objects.
/// \todo easy way to swap order
template <class T> class Group : public RefCounter
{
public:
	Group()
	{
	}

	~Group()
	{
		release();
	}


	virtual void	release() 
	{
		for( int i = 0; i  < size(); i++ )
		{
			objects[i] = 0;
		}
		
		objects.clear();
	}


	/// find an object by label in map
	T* find( const char * label )
	{
		// \todo map is dangerous if label of object is gone... \todo use some kind of signaling method or remove.
		// objects could have pointers to who contain them and send signals to tell something has changed...
		// like Base::signal( const char * sig ) ?
		// or Base::signal ( uint32 id ); so a base object can have an Array of parent objects it sends signals to.?
		/*if(objectMap.find(label)==objectMap.end())
			return 0;
		return objectMap[label];*/
		for( int i = 0; i < objects.size(); i++ )
		{
			if( objects[i]->getLabel() == label )
				return objects[i];
		}

		return 0;

	}

	const T *get( int index ) const { return objects[index]; }
	T *get( int index ) { return objects[index]; }
	
	/// get indexed
	const T *operator [] (const unsigned int index) const { return objects[index]; }
	/// get indexed
	T *operator [] (const unsigned int index) { return objects[index]; }
	/// get indexed
	T *operator [] ( const char * label ) const { return find(label); }

	int		size() const { return objects.size(); }

	int		add(T * obj)
	{ 
		int i = find(obj);
		if(i > -1)
			return i;

		objects.push_back(obj);
		
		return objects.size()-1;
	}

	void		set( int index, T* obj )
	{
		while( size() <= index )
			objects.push_back(0);
		objects[index] = obj;
	}

	/// \todo Va ripulita la mappa se la dimensione diminuisce....
	void		resize( int n )
	{
		int oldn = objects.size();
		release();
		objects.resize(n);
		for(int i = n-1; i >= 0; i-- )
			objects.push_back(0);
	}
	/// \todo Testare , 'del' forse e troppo pericoloso
	void		remove( T* obj, bool del = true ) 
	{ 
		if(!obj)
			return;
			
		for( int i = 0; i < objects.size(); i++ )
		{
			if( objects[i].ptr == obj )
			{
				return remove( i );
			}
		}
	}

	/// \todo Testare del forse e troppo pericoloso
	void		remove( int i ) 
	{ 
		objects[i] = 0;
		objects.erase(  objects.begin() + i );
	}

	/// returns index if object else -1
	int find( const T * obj )
	{
		for( int i = 0; i < objects.size(); i++ )
		{
			if( objects[i] == obj )
				return i;
		}

		return -1;
	}
protected:
	
	std::vector < RefPtr<T> > objects;
};

 
 

}