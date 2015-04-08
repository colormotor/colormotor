/// ***FILEHEADER

#pragma once

// This is some really "CLEAN" code....

// use one struct for style 
// can parse a css to set it
// keep it simple


// background
// controlColor

// text is mad slow... takes about 3-4 fps for some sliders on iphone
// could render all labels to a render target ( in first pass )and then just render the texture...
// make a staticLabel func? and add a dirty flag to gui.begin( ) for updating the buffer

#pragma once

#include "colormotor.h"


#define AUTOPOS	99999
#define MAX_LOCATORS	10

namespace cm
{

class QuickGUI : public cm::MouseListener
{
public:
	enum LAYOUT
	{
		LAYOUT_NONE = 0,
		LAYOUT_HORIZONTAL,
		LAYOUT_VERTICAL,
		LAYOUT_VERTICAL_INV,
		LAYOUT_DEFAULT = LAYOUT_HORIZONTAL
	};
	
	enum 
	{
		ALIGN_LEFT = 0,
		ALIGN_CENTER = 1,
		ALIGN_RIGHT
	};
		
	/// Control rect
	struct Frame
	{
		// defining a blank rects indicates an automatic rect
		Frame()
		:
		autopos(true)
		{
		}
		
		Frame( int x, int y, int w, int h ) 
		: x(x),y(y),w(w),h(h),
		autopos(false)
		{
		}
		
		const Frame & operator () (int x, int y, int w, int h ) 
		{
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
			autopos = false;
			return *this;
		}
		
		int x;
		int y;
		int w;
		int h;
		bool autopos;
	};
	
	/// Default style for GUI
	class Style
	{
	public:
		
		Style()
		: //defaults
		controlBackColor(31.0/255,37.0/255,71.0/255,0.8),
		controlForeColor(7.0/255,150.0/255,255.0/255.0,0.8),
		controlHighlightColor(110.0/255,171.0/255,204.0/255.0,0.7),
		controlTextColor(110.0/255,171.0/255,255.0/255.0,0.9),
		labelColor(0.8,0.8,0.9,0.9),
		gridSize(14),
		bigSize(100),
		bDrawSliderValues(true)
		{
		}
		
		Color controlBackColor;
		Color controlForeColor;
		Color controlHighlightColor;
		Color controlTextColor;
		Color labelColor;
		
		int	gridSize;
		// size for non square controls
		int bigSize;
		bool bDrawSliderValues;
	};


	QuickGUI();
	virtual ~QuickGUI();
	
	void	begin();
	void	end();
		
	inline void beginHorizontalLayout( int x = 0, int y = 0, int padding = 4 ) { beginLayout(x,y,LAYOUT_HORIZONTAL,padding); }
	inline void beginVerticalLayout( int x = 0, int y = 0, int padding = 4 ) { beginLayout(x,y,LAYOUT_VERTICAL,padding); }
	
	void	beginLayout( int x, int y, LAYOUT type = LAYOUT_DEFAULT, int padding = 4 );
	void	endLayout();
	void	nextLayoutElement( Frame * rect = 0 );

public:
	bool	comboBox( std::string *selections, int * selected, int numItems, const Frame &  rect = Frame() );
    
	bool	button( const std::string& label, const Frame &  rect = Frame() );
	
	/// expects a texture with two states horizontaly
	bool	textureButton( cm::Texture * tex, const Frame & rect = Frame() );
	bool	textureTogglesHorizontal( cm::Texture ** textures, int nItems, int * selection, const Frame & rect = Frame() );
	bool	textureStateButton( cm::Texture * tex, int * state, int nStates, const Frame &  rect = Frame() );
	bool	textureToggleButton( cm::Texture * tex, bool * val, const Frame &  rect = Frame() );
	
	bool	stateButton( int * state, const char ** stateNames, int nStates, const Frame &  rect = Frame() );
	bool	toggleButton( const char * text, bool * val, const Frame &  rect = Frame() );
	void	label( const char * str, int align = ALIGN_LEFT, const Frame &  rect = Frame() );
    void	label( std::string str, int align = ALIGN_LEFT, const Frame &  rect = Frame() );
	bool	floatSliderH( float * val, float min, float max, float defaultValue = 0.0f, const Frame &  rect = Frame() );
	bool	floatSliderV( const char * name, float * val, float min, float max, float defaultValue = 0.0f, const Frame &  rect = Frame() );
	bool	checkBox( const char * text, bool * val, const Frame &  rect = Frame() );

	void	textf( const Frame &  rect, int align, const char * fmt, ... );
		
	bool isAnyItemActive();
	
	bool doFloatParam(Param* param, bool showLabel = true, const Frame&  rect = Frame());
	bool doParams(cm::ParamList& params, int x, int y);
	
	void		mouseMoved(int x, int y);
	void		mouseDragged(int x, int y, int btn);
	void		mousePressed( int x, int y, int btn );
	void		mouseReleased( int x, int y, int btn );

	Style style;
	
	///////////////////////////////////////////////////
	// Mouse info ( TODO use flags? )
	enum
	{
		LEFT_BUTTON = 0,
		MIDDLE_BUTTON,
		RIGHT_BUTTON
	};
	
	struct Locator
	{
		int x;
		int y;
		int oldX;
		int oldY;
		int buttons[3];
	} locator[MAX_LOCATORS];//[MAX_LOCATORS];
	
	Locator & mouse; // hack this is for using gui without worrying about multiple locators ( multitouch )
	
	/// the UI state
	struct
	{
		long prevItem;
		
		long activeItem;
		long hotItem;
		
		// TODO
		long keyboardItem;
		char charKey;
		bool doubleClicked;
		
	} uiState[MAX_LOCATORS];
	

	/////////////////////////////////////////////////
	//
	// Layout info
	struct Layout
	{
		LAYOUT type;
		int startX;
		int startY;
		int curX;
		int curY;
		int padding;
		int maxWidth;
		int maxHeight;
		
		Layout * parent;
	};

	enum 
	{
		MAX_LAYOUT_DEPTH = 15
	};
	
	/// layout stack
	Layout layouts[MAX_LAYOUT_DEPTH];
	Layout * curLayout;
	int layoutIndex;
	
	// combo box 
	struct
	{
		bool active;
		int x;
		int y;
		int numItems;
		std::vector< std::string > selections;
		std::vector< bool > enabled;
		int internalSelection;
		int	*selection;
		int	oldSelection;
		bool changed;
		int	cid;
	} popup;
	
	/// flags for active and hot item
	enum
	{
		NOITEM = 0,
		ITEMCLICKED = -1
	};
	
	void	setDirty() { dirty = true; }
	
protected:
	virtual void drawRectDown( const Color & color, const Frame & rect );
	virtual void drawRectUp(  const Color & color, const Frame & rect );
	virtual void drawRect(  const Color & color, const Frame & rect );

	bool	isLocatorIn( int locatorId, const Frame & rect );
	
	// TODO pop up menu?
	bool	doPopupMenu();

	void	incrementLayoutIndex();
	
	long	getIdFromPosition( int x, int y )
	{
		return (x << 16 | y)+1;
	}
	
	/// Returns index of locator
	int	testLocator( long cid, const Frame & rect );
	/// Test specific locator 
	int testLocatorId( int locatorId, long cid, const Frame & rect );
	
	bool isButtonDown( int btn, int locatorId = 0) const;
	bool isItemActive( int cid, int locatorId = 0 ) const;
	bool isItemHot( int cid, int locatorId = 0 ) const;
	
	bool itemHasBeenReleased( int cid, int locatorId = 0, bool isPopup = false );
	bool itemHasBeenPressed( int cid, int locatorId, bool isPopup = false );

	bool	dirty;
	
	int	dx( int locatorId = 0 ) const { return locator[locatorId].x-locator[locatorId].oldX; }
	int dy( int locatorId = 0 ) const { return locator[locatorId].y-locator[locatorId].oldY; }
	
		
	inline int getMultipleOf( int val, int ref )
	{
		int n = val/ref;
		n*=val;
		if( n < val )
			n+=val;
		return n;
	} 
	
};

}