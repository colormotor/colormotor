#pragma once
#include "colormotor.h"
#include "imgui.h"
#include "cm_params.h"
#include "cm_gfx.h"
#include "gfx_ui.h"

namespace ImGui
{
    void BeginToolbar( const std::string & label, float x, float y );
    void BeginHighlightButton();
    void EndHighlightButton();
    void Title( const std::string & title );
}

namespace cm
{
    void imgui(ParamList& plist, float cursorPos=-1);
    
    class Mouse
    {
    public:
        static bool down( int btn = 0 )
        {
            if(!hasFocus())
                return false;
            
            return ImGui::IsMouseDown(btn);
        }
        
        static bool clicked( int btn = 0 )
        {
            if(!hasFocus())
                return false;
            
            return ImGui::IsMouseClicked(btn);
        }
        
        static bool released( int btn = 0 )
        {
            if(!hasFocus())
                return false;
            
            return ImGui::IsMouseReleased(btn);
        }
        
        static float x()
        {
            ImVec2 p = ImGui::GetMousePos();
            return p.x;
        }
        
        static float y()
        {
            ImVec2 p = ImGui::GetMousePos();
            return p.y;
        }
        
        static V2 pos()
        {
            ImVec2 p = ImGui::GetMousePos();
            return V2(p.x, p.y);
        }
        
        static float dx()
        {
            return _delta.x;
        }	
        
        static float dy()
        {
            return _delta.y;
        }
        
        static V2 delta()
        {
            return V2(_delta.x, _delta.y);
        }
        

        static V2 dragDelta()
        {
            ImVec2 d = ImGui::GetMouseDragDelta();
            return V2(d.x, d.y);
        }
        
        static bool hasFocus()
        {
            //ImGuiIO& io = ImGui::GetIO();
            //return !io.WantCaptureMouse;
            return !ui::hasFocus();
        }
        
        static ImVec2 _oldp;
        static ImVec2 _delta;
    };

    class Keyboard
    {
    public:
        static bool down(int key_index) { return ImGui::IsKeyDown(ImGui::GetIO().KeyMap[key_index]); }
        static bool pressed(int key_index, bool repeat ) { return ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[key_index], repeat); }
        static bool released(int key_index) { return ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[key_index]); }  
    };

}