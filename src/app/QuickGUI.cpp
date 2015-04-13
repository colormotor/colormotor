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

#include "QuickGUI.h"

namespace cm
{

void blitImage( cm::Texture * tex, float x, float y, float w, float h, float imgx, float imgy )
{
	tex->drawPart(x,y,w,h,imgx/tex->getWidth(),imgy/tex->getHeight(),w/tex->getWidth(),h/tex->getHeight());
}


QuickGUI::QuickGUI()
:
mouse( locator[0] ),
curLayout(0),
layoutIndex(-1),
dirty(true)
{	
	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		uiState[i].activeItem = NOITEM;
		uiState[i].hotItem = NOITEM;
		uiState[i].prevItem = NOITEM;

		
		locator[i].oldX = locator[i].oldY = 0;
		locator[i].buttons[0] = 0;
		locator[i].buttons[1] = 0;
		locator[i].buttons[2] = 0;
	}

	for( int i = 0; i < MAX_LAYOUT_DEPTH; i++ )
	{
		if( i > 0 )
			layouts[i].parent = &layouts[i-1];
		else
			layouts[i].parent = 0;
	}
	
	popup.x = 0;
	popup.y = 0;
	popup.selection = 0;
	popup.oldSelection = -1;
	popup.changed = false;
	popup.selections.clear();
	popup.enabled.clear();
	popup.cid = NOITEM;
	popup.active = false;
	popup.numItems = 0;

}

QuickGUI::~QuickGUI()
{
}

void	QuickGUI::begin()
{
	curLayout = 0;
	layoutIndex = -1;
	
	std::string str = "";
	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		uiState[i].hotItem = NOITEM;
		str+=intToString(locator[i].buttons[0]);
		
	}
}

void	QuickGUI::end()
{
	if ( popup.active )
		doPopupMenu();

	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		uiState[i].prevItem = uiState[i].activeItem;
			
		if (!locator[i].buttons[LEFT_BUTTON])
		{
			//uiState[i].prevItem = uiState[i].activeItem;
			uiState[i].activeItem = NOITEM;
		}
		else
		{
			if (uiState[i].activeItem == NOITEM)
				uiState[i].activeItem = ITEMCLICKED;
		}
		
		locator[i].oldX = locator[i].x;
		locator[i].oldY = locator[i].y;
	
	}

	layoutIndex = -1;
	curLayout = 0;

	dirty = false;
}





void QuickGUI::beginLayout( int x, int y, LAYOUT type, int padding )
{
	int curx = 0;
	int cury = 0;
	
	layoutIndex++;
	if( layoutIndex >= MAX_LAYOUT_DEPTH )
	{
		printf("Maximum layout depth exceeded!\n");
		return;
	}

	if( curLayout )
	{
		curx = curLayout->curX;
		cury = curLayout->curY;
	}
	 
	//layouts.push_back( Layout() );
	curLayout = &layouts[layoutIndex];
	
	curLayout->type = type;
	curLayout->startX = curLayout->curX = x + curx;
	curLayout->startY = curLayout->curY = y + cury;
	curLayout->padding = padding;
	curLayout->maxWidth = style.gridSize;
	curLayout->maxHeight = style.gridSize;
	//curLayout->parent = parent;
}

void QuickGUI::endLayout()
{
	if( !curLayout )
		return;
		
	int type = curLayout->type;
	
	layoutIndex--;
	
	if( layoutIndex > -1 )
	{
		curLayout = &layouts[layoutIndex];
	
		switch( type )
		{
			case LAYOUT_HORIZONTAL:
				curLayout->curX = curLayout->startX;
				break;
				
			case LAYOUT_VERTICAL:
				curLayout->curY = curLayout->startY;
				break;
			
		}
		
		// next element
		Frame r(0,0,curLayout->maxWidth,curLayout->maxHeight);
		nextLayoutElement( &r );
		
	}
	else 
	{
		curLayout = 0;
		layoutIndex = -1;
	}
}


void	QuickGUI::nextLayoutElement( Frame * rect )
{
	// create a default vertical layout if necessary
	if( !curLayout )
	{
		beginVerticalLayout(0,0);
	}
	
	if( rect )
	{
		curLayout->maxWidth = max( rect->w, curLayout->maxWidth );
		curLayout->maxHeight = max( rect->h, curLayout->maxHeight );
	}

	// increment parent bounds
	Layout * l = curLayout;
	int m = 0;
	int count  = 0;
	while( l->parent )
	{
		m = ( l->curX + l->maxWidth ) - l->parent->curX;
		l->parent->maxWidth = max( m , l->parent->maxWidth );
		m = ( l->curY + l->maxHeight ) - l->parent->curY;
		l->parent->maxHeight = max( m , l->parent->maxHeight );
		l = l->parent;
		count++;
	}


	switch( curLayout->type )
	{
		case LAYOUT_HORIZONTAL:
			curLayout->curX += curLayout->maxWidth + curLayout->padding;
			break;
		
		case LAYOUT_VERTICAL_INV:
			curLayout->curY -= curLayout->maxHeight + curLayout->padding;
			break;
			
		case LAYOUT_VERTICAL:
		default:
			curLayout->curY += curLayout->maxHeight + curLayout->padding;
			break;

	}
	
}
	
void	QuickGUI::label( const char * str, int align, const QuickGUI::Frame & rect )
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize;
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		//curFrame.w = style.defaultWidth; 
		//curFrame.h = style.defaultHeight;
		
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int w = gfx::getFont()->getStringSize(str);
	curFrame.y = curFrame.y + (curFrame.h/2) + ((int)gfx::getFont()->getSize()/2) ;
	
	switch( align )
	{
		case ALIGN_LEFT:
			break;
			
		case ALIGN_CENTER:
			curFrame.x = curFrame.x+curFrame.w/2 - w/2;
			break;
			
		case ALIGN_RIGHT:
			curFrame.x = curFrame.x+curFrame.w-w;
			break;
		
	}
	
	gfx::color( style.labelColor );
	drawText( curFrame.x, curFrame.y, str );
	
}
void QuickGUI::label(std::string str, int align, const Frame& rect)
{
    label(str.c_str(), align, rect);
}

void QuickGUI::textf( const QuickGUI::Frame &  rect, int align, const char * fmt, ... )
{	
	char str[2048];
	va_list	parameter;
	va_start(parameter,fmt);
	vsprintf(str,fmt,parameter);
	va_end(parameter);

	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
	}
	
	int w = gfx::getFont()->getStringSize(str);
	curFrame.y = curFrame.y + (curFrame.h/2) + ((int)gfx::getFont()->getSize()/2) ;
	
	switch( align )
	{
		case ALIGN_LEFT:
			break;
			
		case ALIGN_CENTER:
			curFrame.x = curFrame.x+curFrame.w/2 - w/2;
			break;
			
		case ALIGN_RIGHT:
			curFrame.x = curFrame.x+curFrame.w-w;
			break;
		
	}
	
	gfx::color(style.controlTextColor);
	gfx::drawText( curFrame.x, curFrame.y, str );
}


bool	QuickGUI::textureToggleButton( cm::Texture * tex, bool * val, const QuickGUI::Frame &  rect)
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
		curFrame.w = tex->getWidth() >> 1;
		curFrame.h = tex->getHeight();
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = tex->getWidth() >> 1; 
		curFrame.h = tex->getHeight();
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}
			
	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	bool res = false;
	
	if( itemHasBeenPressed(cid,loc) )
	{
		*val = !(*val);
		res = true;
	}
	

	if( *val )
		blitImage(tex,curFrame.x,curFrame.y,curFrame.w,curFrame.h,curFrame.w,0);
	else
		blitImage(tex,curFrame.x,curFrame.y,curFrame.w,curFrame.h,0,0);
		
	return res;
}

bool	QuickGUI::toggleButton( const char * text, bool * val, const QuickGUI::Frame & rect   )
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	
	
	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	gfx::color( (const float*)style.controlTextColor);
	
	bool otherval = !(*val);

	Color ledClr( style.controlHighlightColor.r*0.5,
					  style.controlHighlightColor.g*0.5,
					  style.controlHighlightColor.b*0.5,
					  style.controlHighlightColor.a );
		
	Frame ledFrame( curFrame.x+1,curFrame.y+1,curFrame.h/2 ,curFrame.h/2 );
	
	if( isItemHot( cid,loc ) )
	{
		if( isItemActive(cid,loc) )
		{
			// press over checkbox
			if(otherval)
			{
				drawRectDown(style.controlForeColor,curFrame);
				drawRect(style.controlHighlightColor,ledFrame);
			}
			else
			{
				drawRectUp(style.controlForeColor,curFrame);
				drawRect(ledClr,ledFrame);
			}
			
		}
		else
		{
			if(*val)
			{
				drawRectUp(style.controlForeColor,curFrame);
				drawRect(style.controlHighlightColor,ledFrame);
			}
			else
			{
				drawRectUp(style.controlForeColor,curFrame);
				drawRect(ledClr,ledFrame);
			}
		}
	}
	else
	{
			if(*val)
			{
				drawRectUp(style.controlForeColor,curFrame);
				drawRect(style.controlHighlightColor,ledFrame);
			}
			else
			{
				drawRectUp(style.controlBackColor,curFrame);
				drawRect(ledClr,ledFrame);
			}
	}

	// label
	textf(curFrame,1,"%s",text);

	/*if (	loc > -1 &&
			!locator[loc].buttons[0] && 
			uiState[loc].hotItem == cid ) // && 
//			uiState[loc].prevItem == cid) */
			
	//if( loc > -1 && uiState[loc].activeItem == cid )
	if( itemHasBeenPressed(cid,loc) )
	{
		*val = otherval;
		return true;
	}


  // Otherwise, no clicky.
  return false;

}

bool QuickGUI::comboBox( std::string *selections, int * selected, int numItems, const QuickGUI::Frame & rect  )
{ 
	

	Frame curFrame;
	if( !rect.autopos )
	{
		curFrame = rect;
	} else {
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );

	gfx::color((const float*)style.controlTextColor);
	
	int x = curFrame.x;
	int y = curFrame.y;
	
	int hh = (style.gridSize>>1);
	int th = (int)gfx::getFont()->getSize()>>1;
	int texty = y + th + hh;

	/// Find selected string.
	std::string text;

	
	// array of strings case
	int sel = *selected;
	if( sel >= 0 && sel < numItems )
		text = selections[*selected].c_str();
	else
		text = "";
		
	Frame r (x,y,style.bigSize,style.gridSize );

		
	if( isItemHot(cid,loc) && !popup.active)
	{
		if( isItemActive(cid,loc) )
		{
			drawRectDown(style.controlHighlightColor,r);

			color(style.controlTextColor);
			drawText(x+hh+3,texty, "%s", text.c_str());
			popup.selections.clear();
			popup.enabled.clear();
			
			popup.selection = selected;
			popup.oldSelection = *selected;
			popup.changed = false;
			popup.numItems = numItems;
			for( int i = 0; i < popup.numItems; i++ )
			{
				popup.selections.push_back( selections[i] );
				popup.enabled.push_back(true);
			}

			popup.cid = cid;
			popup.active = true;
			popup.x = x;
			popup.y = y;//+style.defaultHeight;
			
			uiState[loc].activeItem = NOITEM;
			uiState[loc].hotItem = NOITEM;
		}
		else
		{
			drawRectDown(style.controlHighlightColor,r);
			color(style.controlTextColor);
			drawText(x+hh+3,texty,"%s",text.c_str());
		}
	}
	else
	{
		if(popup.active && popup.cid == cid)
		{
			drawRectUp(style.controlForeColor,r);
			color(style.controlBackColor);
		}
		else
		{
			drawRectUp(style.controlBackColor,r);
			color(style.controlTextColor);
		}
		
		drawText(x+hh+3,texty,"%s",text.c_str());
	}

	
	// this means that a new selection has been made so we will return true
	if( popup.changed && popup.cid == cid )
	{
		popup.changed = false;
		return true;
	}

	return false; 
}




/*
	Frame curFrame;
	if( !rect.autopos )
	{
		curFrame = rect;
	} else {
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame,true );

	gfx::color((const float*)style.controlTextColor);
	
	int x = curFrame.x;
	int y = curFrame.y;
	
	int hh = (style.gridSize>>1);
	int th = (int)gfx::getFont()->getSize()>>1;
	int texty = y + th + hh;

	const char * text = selections[*selected].c_str();

	Frame r (x,y,style.bigSize,style.gridSize );

		
	if( isItemHot(cid,loc) && !popup.active)
	{
		if( isItemActive(cid,loc) )
		{
			drawRectDown(style.controlHighlightColor,r);

			color(style.controlTextColor);
			drawText(x+hh+3,texty,"%s",text);
			popup.selections = selections;
			popup.selection = selected;
			popup.oldSelection = *selected;
			popup.changed = false;
			popup.numItems = numItems;
			popup.cid = cid;
			popup.active = true;
			popup.x = x;
			popup.y = y;//+style.defaultHeight;
			
			uiState[loc].activeItem = NOITEM;
			uiState[loc].hotItem = NOITEM;
		}
		else
		{
			drawRectDown(style.controlHighlightColor,r);
			color(style.controlTextColor);
			drawText(x+hh+3,texty,"%s",text);
		}
	}
	else
	{
		if(popup.active && popup.cid == cid)
		{
			drawRectUp(style.controlForeColor,r);
			color(style.controlBackColor);
		}
		else
		{
			drawRectUp(style.controlBackColor,r);
			color(style.controlTextColor);
		}
		
		drawText(x+hh+3,texty,"%s",text);
	}

	
	// this means that a new selection has been made so we will return true
	if( popup.changed && popup.cid == cid )
	{
		popup.changed = false;
		return true;
	}

	return false; 
}
*/

bool	QuickGUI::doPopupMenu()
{
	int x = popup.x;
	int y = popup.y;

	int h = style.gridSize * (popup.numItems); 
	
	Frame rect( x,y,style.bigSize, h+style.gridSize );
	
	int loc = testLocator(popup.cid,rect );// we consider the combo box too ( OLD check for popup menus )

	gfx::color((const float *)style.controlTextColor);
	
	int selection = -1;
	
	y+=style.gridSize;

	if( loc > -1 && popup.cid == uiState[loc].hotItem)
	{
		if(mouse.y>(y))
		{
			selection = mouse.y-y;
			selection /= style.gridSize;
			
			if( selection >= popup.numItems )
			{
				debugPrint("doPopupMenu(): something went wrong TODO fixme\n");
				selection = popup.numItems-1;
			}

			if( !popup.enabled[selection] )
				selection = -1;
		}
	}

	int texty = y + ((int)gfx::getFont()->getSize()>>1) + (style.gridSize>>1);
	Frame r( x,y,style.bigSize,style.gridSize );
	
	for( int i = 0; i < popup.numItems; i++ )
	{
		/// Find selected string.
		const std::string & text = popup.selections[i];
		bool enabled = popup.enabled[i];

		if( i == selection )
		{
			drawRectDown(style.controlForeColor,r);
			color(style.controlBackColor);
		}
		else 
		{
			drawRectUp(style.controlBackColor,r);
			color(style.controlTextColor);
		}

		if( !enabled )
			color(style.controlBackColor.withAlpha(0.3));
		
		drawText(x+4, texty, "%s", text.c_str());
		texty += style.gridSize;
		r.y += style.gridSize;
	}
	/*
	// item has been selected.
	if( locator[loc].buttons[LEFT_BUTTON] )
	{
		if ( loc > -1 ) 
		{
			if( selection >= 0 && uiState[loc].hotItem)
			{
				*popup.selection = selection;
				if( selection != popup.oldSelection )
					popup.changed = true;
					
				popup.active = false;
			}			
			
			if( uiState[loc].hotItem != popup.cid )
				popup.active = false;
		}	
		else 
		{
			popup.active = false;
		}
	}
	
	*/
	
	// 
	if( locator[loc].buttons[LEFT_BUTTON] &&
		uiState[loc].hotItem != popup.cid )
	{
		//Click out of popup?
		popup.changed = false;
		popup.active = false;
		return false;
	}
	else
	if( itemHasBeenReleased(popup.cid,loc,true) )
	{
		if( selection >= 0 && uiState[loc].hotItem)
		{
			*popup.selection = selection;
			if( selection != popup.oldSelection )
			{
				popup.changed = true;
				
			}
			popup.active = false;
		}			
		
		if( uiState[loc].hotItem != popup.cid )
			popup.active = false;
	}
	

	return true;
}



bool	QuickGUI::button( const std::string& label, const QuickGUI::Frame & rect  )
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}
	
	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	gfx::color((const float *)style.controlTextColor);
	
	int w = gfx::getFont()->getStringSize(label.c_str());
	int textx = curFrame.x+curFrame.w/2 - w/2;
	int texty = curFrame.y + ((int)gfx::getFont()->getSize()>>1) + (style.gridSize>>1);
	
	if( isItemHot(cid,loc) )
	{
		if( isItemActive(cid,loc) )
		{
			drawRectDown(style.controlHighlightColor,curFrame);
			gfx::color(style.controlTextColor);
			drawText(textx,texty,"%s",label.c_str());
		}
		else
		{
			drawRectUp(style.controlBackColor,curFrame);
			gfx::color(style.controlTextColor);
			drawText(textx,texty,"%s",label.c_str());
		}

	}
	else
	{
		drawRectUp(style.controlBackColor,curFrame);
		gfx::color(style.controlTextColor);
		drawText(textx,texty,"%s",label.c_str());
	}

	if( itemHasBeenPressed(cid,loc) )
	{
		return true;
	}
	  // Otherwise, no clicky.
	return false;

}


bool	QuickGUI::textureTogglesHorizontal( Texture ** textures, int nItems, int * selection, const Frame & rect )
{
	Frame curFrame;
	
	Texture * tex = textures[0];
	
	int w = tex->getWidth() >> 1;
	
	if( !rect.autopos )
	{
		curFrame = rect;
		curFrame.w = w*nItems;
		curFrame.h = tex->getHeight();
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = w*nItems; 
		curFrame.h = tex->getHeight();
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	bool res = false;
	
	if( isItemActive(cid,loc) )//itemHasBeenPressed(cid,loc) )
	{
		int clix = locator[loc].x;
		clix -= curFrame.x;
		*selection = clix/w;
		res = true;
	}
	
	int x = curFrame.x;
	int sel = *selection;
	
	for( int i = 0; i < nItems; i++ )
	{
		tex = textures[i];
		
		if( i == sel )
			blitImage(tex,x,curFrame.y,w,curFrame.h,w,0);
		else
			blitImage(tex,x,curFrame.y,w,curFrame.h,0,0);
		
		x+=w;
	}
	
	

	return res;
}

bool	QuickGUI::textureButton( Texture * tex, const Frame & rect )
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
		curFrame.w = tex->getWidth() >> 1;
		curFrame.h = tex->getHeight();
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = tex->getWidth() >> 1; 
		curFrame.h = tex->getHeight();
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	if( isItemHot(cid,loc) )
	{
		if( isItemActive(cid,loc) )
		{
			blitImage(tex,curFrame.x,curFrame.y,curFrame.w,curFrame.h,curFrame.w,0);
		}
		else
		{
			blitImage(tex,curFrame.x,curFrame.y,curFrame.w,curFrame.h,0,0);
		}

	}
	else
	{
		blitImage(tex,curFrame.x,curFrame.y,curFrame.w,curFrame.h,0,0);
	}

	if( itemHasBeenPressed(cid,loc) )
	{
		return true;
	}
	  // Otherwise, no clicky.
	return false;
}


bool	QuickGUI::textureStateButton( Texture * tex, int * state, int nStates, const QuickGUI::Frame &  rect  )
{
	Frame curFrame;
	
	int w = tex->getWidth()/nStates;
	int h = tex->getHeight();
	
	if( !rect.autopos )
	{
		curFrame = rect;
		curFrame.w = w;
		curFrame.h = h;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = w; 
		curFrame.h = h;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}
	
	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	int s = *state;
	
	blitImage(tex,curFrame.x,curFrame.y,w,h,w*s,0);

 	/*if (	loc > -1 &&
			!locator[loc].buttons[0] && 
			uiState[loc].hotItem == cid && 
			uiState[loc].prevItem == cid) 
	//if( loc > -1 && uiState[loc].activeItem == cid )
	{
		debugPrint("Button released\n");
		int s = *state;
		s = (s+1)%nStates;
		*state = s;
		return true;
	}*/
	
	if( itemHasBeenPressed(cid,loc) )
	{
		s = (s+1)%nStates;
		*state = s;
		return true;
	}

	return false;
}

bool	QuickGUI::stateButton( int * state, const char ** stateNames, int nStates, const QuickGUI::Frame & rect  )
{
	Frame curFrame;
	std::string str;
	
	if( stateNames == 0 )
		str = intToString(*state);
	else
	{
		str = stateNames[*state];
	}
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.gridSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	
	int w = gfx::getFont()->getStringSize(str.c_str());
	int textx = curFrame.x+curFrame.w/2 - w/2;
	int texty = curFrame.y + ((int)gfx::getFont()->getSize()>>1) + (style.gridSize>>1);
	
	if( isItemHot(cid,loc) ) 
	{
		if( cid == uiState[loc].activeItem )
		{
			drawRectDown(style.controlHighlightColor,curFrame);
			gfx::color((const float *)style.controlTextColor);
			drawText(textx,texty,"%s",str.c_str());
		}
		else
		{
			drawRectUp(style.controlBackColor,curFrame);
			gfx::color((const float *)style.controlTextColor);
			drawText(textx,texty,"%s",str.c_str());
		}

	}
	else
	{
		drawRectUp(style.controlBackColor,curFrame);
		gfx::color((const float *)style.controlTextColor);
		drawText(textx,texty,"%s",str.c_str());
	}

 	/*if (	loc > -1 &&
			!locator[loc].buttons[0] && 
			uiState[loc].hotItem == cid && 
			uiState[loc].prevItem == cid) 
	//if( loc > -1 && uiState[loc].activeItem == cid )
	{
		debugPrint("Button released\n");
		int s = *state;
		s = (s+1)%nStates;
		*state = s;
		return true;
	}*/
	
	if( itemHasBeenPressed(cid,loc) )
	{
		int s = *state;
		s = (s+1)%nStates;
		*state = s;
		return true;
	}
	
	
	  // Otherwise, no clicky.
	return false;

}



bool QuickGUI::floatSliderH(float * val, float min, float max, float defaultValue, const QuickGUI::Frame& rect)
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.bigSize; 
		curFrame.h = style.gridSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}

	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	float range = max-min;

	drawRect( style.controlBackColor, curFrame );

	int padding = 1;
	int padx = curFrame.x+padding;
	int pady = curFrame.y+padding;
	int padlen = curFrame.w-padding*2;
	
	int offset;

	bool res;
	
	if( isButtonDown(2,loc) )
		*val = defaultValue;
		
	// we are manipulating the slider
	if( isItemActive(cid,loc) ) 
	{
		offset = locator[loc].x-padx;
		if(offset<0)
			offset = 0;
		if(offset>padlen)
			offset = padlen;
		*val = ((float)offset/padlen)*range + min;
		Frame r(padx, pady, offset, curFrame.h-padding*2);
		drawRect( style.controlForeColor, r);
				
		res =  true;
	}
	else 
	{
		float v = (*val-min)/range;
		offset = v*padlen;
		Frame r( padx, pady, offset, curFrame.h-padding*2 );
		drawRect( style.controlForeColor, r );

		res =  false;	
	}
	
	gfx::color((const float *)style.controlHighlightColor);
	int lx = padx+offset;
	int ly = pady;
	gfx::drawLine( lx,ly,lx,ly+curFrame.h-padding*2 );

	return res;
}






bool	QuickGUI::floatSliderV( const char * name, float * val, float min, float max, float defaultValue, const QuickGUI::Frame & rect  )
{
	Frame curFrame;
	
	if( !rect.autopos )
	{
		curFrame = rect;
	}
	else 
	{
		curFrame.x = curLayout->curX; 
		curFrame.y = curLayout->curY;
		curFrame.w = style.gridSize; 
		curFrame.h = style.bigSize;
		curFrame.autopos = false;
		// increment layout element
		nextLayoutElement(&curFrame);
	}


	int cid = getIdFromPosition( curFrame.x, curFrame.y );
	
	 // Check whether the button should be hot
	int loc = testLocator(cid, curFrame );
	
	float range = max-min;

	drawRect( style.controlBackColor, curFrame );

	int padding = 1;
	int padx = curFrame.x+padding;
	int pady = curFrame.y+padding;
	int padlen = curFrame.h-padding*2;
	
	int offset;

	bool res;
	
	if( isButtonDown(2,loc) )
		*val = defaultValue;
		
	// we are manipulating the slider
	if( isItemActive(cid,loc) ) 
	{
		offset = locator[loc].y-pady;
		if(offset<0)
			offset = 0;
		if(offset>padlen)
			offset = padlen;
			
		offset = padlen - offset;
		*val = ((float)(offset)/padlen)*range + min;
		Frame r( padx, pady+padlen-offset, curFrame.w-padding*2, offset );//, curFrame.h-padding*2 );
		drawRect( style.controlForeColor, r);
				
		res =  true;
	}
	else 
	{
		float v = (*val-min)/range;
		offset = v*padlen;
		//offset = padlen - offset;
		Frame r( padx, pady+padlen-offset, curFrame.w-padding*2,offset );
		drawRect( style.controlForeColor, r );

		res =  false;	
	}
	
	
	color((const float*)style.controlHighlightColor);
	int lx = padx;
	int ly = pady+padlen-offset;
	drawLine( lx,ly,lx+curFrame.w-padding*2,ly );//,ly+curFrame.h-padding*2 );

	//color(style.controlTextColor);
	//drawTextVertical( padx,pady+4,name );
	
	return res;
}




bool	QuickGUI::checkBox( const char * text, bool * val, const QuickGUI::Frame & rect  )
{
	return false;
}

void	QuickGUI::incrementLayoutIndex()
{
	
}

bool	QuickGUI::isLocatorIn( int locatorId, const QuickGUI::Frame & rect  )
{
	 if ( locator[locatorId].x < rect.x ||
		  locator[locatorId].y < rect.y ||
		  locator[locatorId].x >= rect.x + rect.w ||
		  locator[locatorId].y >= rect.y + rect.h)
	return false;
	 
	return true;

}

bool QuickGUI::isButtonDown( int btn, int locatorId ) const
{
	return ( locatorId > -1 && locator[locatorId].buttons[btn] );
}

bool QuickGUI::isItemActive( int cid, int locatorId ) const
{
	return (locatorId > -1 && cid > -1 && cid == uiState[locatorId].activeItem );
}

bool QuickGUI::isItemHot( int cid, int locatorId ) const
{
	return (locatorId > -1 && cid > -1 && cid == uiState[locatorId].hotItem );
}

bool  QuickGUI::itemHasBeenReleased( int cid, int locatorId, bool isPopup  )
{
	if( popup.active && !isPopup )
		return false;
		
	if (	!locator[locatorId].buttons[0] && 
			uiState[locatorId].hotItem == cid && 
			uiState[locatorId].activeItem == cid)
			return true;
	return false;
}

bool  QuickGUI::itemHasBeenPressed( int cid, int locatorId, bool isPopup  )
{
	if( popup.active && !isPopup )
		return false;

	if( cid != -1 &&
		locator[locatorId].buttons[0] &&
		uiState[locatorId].activeItem == cid &&
		uiState[locatorId].prevItem != cid )
			return true;
	return false;
}


int QuickGUI::testLocatorId( int locatorId, long cid, const QuickGUI::Frame & rect )
{
	// if popup menu is active ignore the rest....
	if( popup.active && cid != popup.cid )
		return -1;

	if (isLocatorIn(locatorId,rect))
	{
		uiState[locatorId].hotItem = cid;

		//debugPrint("Button state is %d res is %d\n",Mouse::buttonState[0],Mouse::isLeftDown(0));
		if(locator[locatorId].buttons[LEFT_BUTTON])
		{
			if (uiState[locatorId].activeItem == NOITEM )
			{
				uiState[locatorId].activeItem = cid;
				return locatorId;
			}
		}
		
		return locatorId;
	}

	return -1;

}


int	QuickGUI::testLocator( long cid, const QuickGUI::Frame & rect )
{
	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		// test if item is allready active or hot with another locator
		if( uiState[i].activeItem == cid ) // || uiState[i].hotItem == cid )
		{
			testLocatorId( i, cid, rect ); // set hotness
			return i;
		}
	}
	
	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		if( testLocatorId( i, cid, rect ) > -1 )
			return i;
	}
	
	return -1;
}

#define MORE 1.5
#define LESS 0.5

void QuickGUI::drawRectDown( const Color & color, const QuickGUI::Frame & rect  )
{
	gfx::color((const float*)color);
	fillRect(rect.x,rect.y,rect.w,rect.h);
	gfx::color(color.r*LESS,color.g*LESS,color.b*LESS,1.0);
	gfx::drawRect(rect.x,rect.y,rect.w,rect.h);
}

void QuickGUI::drawRectUp(  const Color & color, const QuickGUI::Frame & rect  )
{
	gfx::color((const float*)color);
	gfx::fillRect(rect.x,rect.y,rect.w,rect.h);
	gfx::color(color.r*MORE,color.g*MORE,color.b*MORE,1.0);
	gfx::drawRect(rect.x,rect.y,rect.w,rect.h);
}

void QuickGUI::drawRect(  const Color & color, const QuickGUI::Frame & rect  )
{
	gfx::color((const float*)color);
	gfx::fillRect(rect.x,rect.y,rect.w,rect.h);
}

bool QuickGUI::isAnyItemActive()
{
	for( int i = 0; i < MAX_LOCATORS; i++ )
	{
		if( uiState[i].activeItem > 0 || uiState[i].prevItem > 0 )
			return true;
	}
	
	return false;
}

bool QuickGUI::doParams(cm::ParamList& params, int x, int y )
{
	bool result = false;
    
	beginVerticalLayout(x, y);
    
    for (int p = 0; p < params.getNumParams(); p++)
    {								
        Param* param = params.getParam(p);
        if (param->getType() == PARAM_BOOL) 
        {        
            bool value = param->getBool();
            if(toggleButton(param->getName(), (bool*)param->getAddress()))
            {
                result = true;
            }
        
        } else if (param->getType() == PARAM_FLOAT) {     
            result = doFloatParam(param);
        } else if (param->getType()==PARAM_SELECTION) {

            comboBox(param->getSelectionNames(),(int*)param->getAddress(),param->getNumElements());
		} else if ( param->getType() == PARAM_EVENT ) {
			if( button(param->getName()) )
					param->setBool(!param->getBool());
        } else {
            //unsupported Param type
            //at least show a label with the name
			label( std::string( "X: " ).append( param->getName() ) );
        }
		
	}

	endLayout();
	
	return result;
}


bool QuickGUI::doFloatParam(Param* param, bool showLabel, const Frame& rect)
{
	bool res = false;

	beginHorizontalLayout();
            
    float curx = curLayout->curX;
    float cury = curLayout->curY;

    float value = param->getFloat();
    if(floatSliderH(&value,
                    param->getMin(),
                    param->getMax(),
                    param->getDefault()))
    {
        param->setFloat(value);
        res = true;
    }
          
	if (showLabel)
	{
		gfx::color(style.controlTextColor);
		float val = param->getFloat();
		drawText(curx+5, cury+(style.gridSize / 4)*3, "%g", val);
		label(param->getName(), 0);
	}       
    endLayout();

	return res;
}

void	QuickGUI::mouseMoved(int x, int y)
{
	mouse.x = x;
	mouse.y = y;
}

void	QuickGUI::mouseDragged(int x, int y, int btn)
{
	mouse.x = x;
	mouse.y = y;
}

void	QuickGUI::mousePressed( int x, int y, int btn )
{
	mouse.buttons[btn] = 1;
}

void	QuickGUI::mouseReleased( int x, int y, int btn )
{
	mouse.buttons[btn] = 0;
}


/*
bool QuickGUI::doParam(ParamRange<Vec3>* param, const Frame &  rect)
{
	bool res = false;

	beginHorizontalLayout();

		label(param->getName().c_str(), 0);
        
		beginVerticalLayout();     
			Vec3 value = param->getValue();
			if(floatSliderH(&value.x,
                            param->getMinMaxRange().min.x,
                            param->getMinMaxRange().max.x,
                            param->defaultValue.x))
			{
				param->setValue(value);
				res = true;
			}
			if(floatSliderH(&value.y,
                            param->getMinMaxRange().min.y,
                            param->getMinMaxRange().max.y,
                            param->defaultValue.y))
			{
				param->setValue(value);
				res = true;
			}
			if(floatSliderH(&value.z,
                            param->getMinMaxRange().min.z,
                            param->getMinMaxRange().max.z,
                            param->defaultValue.z))
			{
				param->setValue(value);
				res = true;
			}
		endLayout();
	endLayout();

	return res;
}
*/

}