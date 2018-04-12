struct ImColor
{
    ImColor(float r, float g, float b, float a = 1.0f);
};



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
    
    //void textColor( const arma::vec &clr );
    void text( const arma::vec &p, const std::string& str, ImColor clr=ImColor(-1., -1., -1., 1.)  );

    /// Will return true if the previous widget call has caught an interaction
    bool modified();
    
    bool modifierShift();
    bool modifierAlt();
    
    /// Dragger widget
    arma::vec dragger( int index, const arma::vec& pos, bool selected=false, float size=-1. );

    /// Highlights a dragger
    void highlightDragger( const arma::vec &pos, float size=-1.  );
    
    /// Draw a line from a to b
    void line( const arma::vec& a, const arma::vec& b, ImColor clr=ImColor(-1., -1., -1., 1.) );

    /// Angle and length handle widget
    arma::vec lengthHandle( int index, arma::vec thetaLen, float startTheta, const arma::vec& pos, const arma::vec& minThetaLen, const arma::vec& maxThetaLen, bool selected=false );

    /// Angle handle widget
    float handle( int index, float ang, const arma::vec& pos, float length, float startTheta=0.0f, float minTheta=0.0f, float maxTheta=0.0f, bool selected=false );
    
    /// Simple affine transform widget (two axes, centered)
    arma::mat affineSimple( int index, const arma::mat& m, bool selected=false, float scale=1. );
    
    /// Creates a draggable rect for selection, see demo() for example usage
    cm::Box selector();

    /// Returns current mouse delta
    arma::vec dragDelta();
    
    /// Creates a font-icon toolbar (not necessary to call between begin and end)
    int toolbar( const std::string& title, const std::string& items, int selectedItem, bool horizontal=false, bool showAscii=false );
    
    void demo();   



%rename("%s") "";
}

%inline %{
    bool ui_beginChild(const char * strid)
    {
        ImGui::BeginChild(strid);
    }

    void ui_endChild()
    {
        ImGui::EndChild();
    }

    float ui_sliderFloat(const char* label, float v, float min, float max)
    {
        ImGui::SliderFloat(label, &v, min, max);
        return v;
    } 

    bool ui_button(const char*label)
    {
        return ImGui::Button(label);
    }

    bool ui_checkbox(const char*label, bool v)
    {
        ImGui::Checkbox(label, &v);
        return v;
    }
%}
