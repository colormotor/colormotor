///////////////////////////////////////////////////////////////////////////                                                     
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

#include "RenderChunk.h"


namespace cm
{


void	RenderElement::render()
{
	if( node )
		node->setTransform();

	renderable->render( materialIndex, false );
}

void	MaterialRenderChunk::renderWithMaterial( Material * mtl )
{
	mtl->begin();

	for(MaterialMap::iterator p = materialMap.begin(); p!=materialMap.end(); ++p )
	{
		// must do all rendering manually because we have to flush shader 
		// every time we do a new transform
		RenderElementList * list = (*p).second;
		RenderElement * elem = list->head;

		while(elem)
		{
			// render element
			if(elem->node)
				elem->node->setTransform();
			elem->renderable->render( elem->materialIndex, false );
			elem = elem->next;
		}
	}

	mtl->end();
}
	
}
