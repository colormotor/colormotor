//////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//	� Daniel Berio 2008													
//	  http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "engine/engineIncludes.h"
#include "engine/renderSys/Node.h"
#include "engine/renderSys/Renderable.h"

namespace cm {


/// \addtogroup Gfx
/// @{

/// \addtogroup RenderSystem
/// @{


///////////////////////////////////////////////////////////////////////////
/// render chunk interface
/// groups a number of renderable objects and nodes for rendering
class RenderChunk : public Base
{
public:
	RenderChunk()
	{
	}

	virtual ~RenderChunk()
	{
		release();
	};

	void release()
	{
	}

	/// Ad a renderable to chunk
	virtual bool add(  Renderable * r, Node * node )
	{
		for( int i = 0; i < r->getNumMaterials(); i++ )
		{
			CM_TEST( add(r,node,i) );
		}

		return true;
	}


	/// Add a Renderable object to chunk
	virtual bool				add( Renderable * r, Node * node, int materialIndex ) { return false; }

	/// Find a meterial in a chunk \todo: ?? 
	virtual const Material *	findMaterial( const Material * m ) { return 0; }

	virtual bool				render( float msecs )  { return false; }

	/// \todo RenderTarget

};




/// render element
/// represents one render operation and links a scene graph node to a Material-Renderable couple
class RenderElement
{
public:
	/// Renderable object + material
	Renderable * renderable;

	/// Index of material in renderable
	int		 materialIndex;

	/// a scene node
	/// holds transformations for this render
	Node *	 node;

	/// Get renderable object material
	Material *	 getMaterial() const { return renderable->getMaterial(materialIndex); }
	
	/// render this render element
	/// does not set material
	void	render();
	
	RenderElement * prev;
	RenderElement * next;
};



/// A list of RenderElement objects to be rendered
class RenderElementList : public TPtrList< RenderElement >
{
public:
	Material * mtl;
};


///////////////////////////////////////////////////////////////////////////
/// render chunk that groups renderable objects by material
/// maps every material found to a list of RenderElement objects 
class MaterialRenderChunk : public RenderChunk
{
public:
	/// Map of every material and list of elements to render for it
	typedef std::map<Material*,RenderElementList*> MaterialMap;

public:

	MaterialRenderChunk()
	{
	}

	virtual ~MaterialRenderChunk()
	{
		release();
	};

	void release()
	{
		MaterialMap::iterator p;
		for(p = materialMap.begin(); p!=materialMap.end(); ++p)
		{
			delete (*p).second;
		}

		materialMap.clear();
	}

	bool	add( Renderable * r, Node * node )
	{
		return RenderChunk::add(r,node);
	}

	bool	add( Renderable * r, Node * node, int materialIndex )
	{
		RenderElement *elem = new RenderElement();
		elem->node = node;
		elem->renderable = r;
		elem->materialIndex = materialIndex;

		RenderElementList * list;
		Material * m = r->getMaterial(materialIndex);
		if( findMaterial( m ) )
		{
			list = materialMap[m]; // \todo optimize this we are searching map twice
		}
		else
		{
			list = new RenderElementList();
			list->mtl = m;
			materialMap[m] = list;
		}

		list->insert(elem);
		return true;
	}

	inline bool	hasMaterial( Material * m )
	{
		if(materialMap.find(m)==materialMap.end())
			return false;
		return true;
	}
	
	void renderWithMaterial( Material * mtl );

	void render()
	{
		MaterialMap::iterator p;

		for(p = materialMap.begin(); p!=materialMap.end(); ++p )
		{
			Material * m = (*p).first;
			m->begin();
			//int n = m->begin();
			//for( int i = 0; i < n; i++ )
			//{
				//m->pass(i);
				render((*p).second);
				//m->endPass();
			//}
			m->end();
		}
	}

	/// render specified material
	void render( Material * mtl)
	{
		MaterialMap::iterator r = materialMap.find(mtl);
		if(r == materialMap.end())
			return;

		RenderElementList * rlist = r->second;
		return render(rlist);
	}

	// render all elements in list
	void render( RenderElementList * rlist)
	{
		Material * mtl = rlist->mtl;
		
		mtl->begin();
		RenderElement * elem = rlist->head;
		while(elem)
		{
			elem->render();
			elem = elem->next;
		}
		mtl->end();
	}


protected:
	/// \todo maybe material name is better
	MaterialMap materialMap; 
};

/// @}

/// @}


}