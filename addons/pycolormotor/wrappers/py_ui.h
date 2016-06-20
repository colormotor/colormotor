namespace ui
{

%rename("ui_%s") "";

    /// Returns true if the IMGUI/gfx_ui have focus
    /// Since gfx_ui creates a "full screen" window to handle interactions through IMGUI,
    /// you should use this rather than ImGui::GetIO().WantCaptureMouse to check weather
    /// your application should handle mouse events
    bool hasFocus();

    /// The UI should be called withing these begin end blocks
    void begin( const std::string& name="ui" );
    void end();
    
    /// Will return true if the previous widget call has caught an interaction
    bool modified();
    
    /// Dragger widget
    arma::vec dragger( int index, const arma::vec& pos, bool selected=false, float size=-1. );

    /// Highlights a dragger
    void highlightDragger( const arma::vec &pos, float size=-1.  );
    
    /// Draw a line from a to b
    void line( const arma::vec& a, const arma::vec& b );

    /// Angle handle widget
    float handle( int index, float ang, const arma::vec& pos, float length, bool selected=false );
    
    /// Simple affine transform widget (two axes, centered)
    arma::mat affineSimple( int index, const arma::mat& m, bool selected=false );
    
    /// Creates a draggable rect for selection, see demo() for example usage
    cm::Box selector();

    /// Returns current mouse delta
    arma::vec dragDelta();
    
    /// Creates a font-icon toolbar (not necessary to call between begin and end)
    int toolbar( const std::string& title, const std::string& items, int selectedItem, bool horizontal=false, bool showAscii=false );
    
    void demo();   

%rename("%s") "";
}