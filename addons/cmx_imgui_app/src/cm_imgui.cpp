//
//  cm_imgui.cpp
//  basic
//
//  Created by Daniel Berio on 09/04/16.
//  Copyright (c) 2016 Daniel Berio. All rights reserved.
//

#include "cm_imgui.h"



namespace ImGui
{
    void BeginToolbar( const std::string & label, float x, float y )
    {
        static bool show = true;
        ImGui::Begin(label.c_str(),&show, ImVec2(x,y), 0.7f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
    }
    
    void BeginHighlightButton()
    {
        float h = 0.0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(h, 0.8f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(h, 0.7f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(h, 0.8f, 0.9f));
    }
    
    void EndHighlightButton()
    {
        ImGui::PopStyleColor(3);
    }
    
    void Title( const std::string & title )
    {
        static bool alwaystrue = true;
        ImGui::PushStyleColor(ImGuiCol_Header, ImColor::HSV(0.0,0.0,0.3));
        ImGui::Selectable(title.c_str(), &alwaystrue);
        ImGui::PopStyleColor(1);
        alwaystrue = true;
    }
    
}


// Combo box helper allowing to pass an array of strings.
static bool stringCombo(const char* label, int* current_item, const std::string * item_names, int items_count )
{
    std::string str = "";
    for( int i = 0; i < items_count; i++ )
    {
        str += item_names[i];
        str += (char)0;
    }
    return ImGui::Combo(label, current_item, str.c_str());
}

namespace cm
{

ImVec2 Mouse::_oldp;
ImVec2 Mouse::_delta;
    
void imgui( ParamList& plist, float cursorPos  )
{
    std::string id = plist.getPath() + "_ID";
    ImGui::PushID(id.c_str());
    
    float cposyPrev = ImGui::GetCursorPosY();
    
    bool vis = ImGui::CollapsingHeader(plist.getName().c_str(), NULL, true, true);
    
    float cposyNext = ImGui::GetCursorPosY();
    //ImGUi::SetCursorPosY(cposyPrev);
    //SameLine();
    
    //ImGui::PushItemWidth(-10);
    ImGui::SameLine(ImGui::GetWindowWidth()-100);
    if(ImGui::Button("Load")) //, ImVec2(100,2)))
    {
        std::string path;
        if(openFileDialog(path,"xml"))
            plist.loadXml(path.c_str());
    }
    //ImGui::NextColumn();
    ImGui::SameLine();
    if(ImGui::Button("Save"))//, ImVec2(100,20)))
    {
        std::string path;
        if(saveFileDialog(path,"xml"))
            plist.saveXml(path.c_str());
    }
    //ImGui::PopItemWidth();
    
    if(!vis)
        return;
    
    //if(!)
    //ImGui::OpenNextNode(true);
    //if(!ImGui::TreeNode(plist.getName().c_str()))
    
    for( int i = 0; i < plist.getNumParams(); i++ )
    {
        Param * p = plist.getParam(i);
        if(p->hasOption("h"))
            continue;
        if(p->hasOption("sameline"))
            ImGui::SameLine();
        
        switch(p->getType())
        {
            case PARAM_FLOAT:
                if(p->hasOption("v"))
                {
                    p->dirty = ImGui::InputFloat(p->getName(),(float*)p->getAddress());
                }
                else
                {
                    p->dirty = ImGui::SliderFloat(p->getName(), (float*)p->getAddress(), p->getMin(), p->getMax());
                }
                break;
                
            case PARAM_INT:
                p->dirty = ImGui::InputInt(p->getName(), (int*)p->getAddress());
                break;
                
            case PARAM_EVENT:
                if(ImGui::Button(p->getName()))
                {
                    p->dirty = true;
                    p->setBool(!p->getBool());
                }
                break;
                
            /*case PARAM_COLOR:
                ImGui::ColorEdit3(p->getName(), (float*)p->getAddress());
                break;
            */
                
            case PARAM_BOOL:
                p->dirty = ImGui::Checkbox(p->getName(), (bool*)p->getAddress());
                break;
                
            case PARAM_UNKNOWN:
                ImGui::Separator();
                break;
                
            case PARAM_SELECTION:
                p->dirty = stringCombo(p->getName(),(int*)p->getAddress(), p->getSelectionNames(), p->getNumElements() );
                break;
                
            case PARAM_CSTRING:
                p->dirty = ImGui::InputText(p->getName(),(char*)p->getAddress(), 50); // HACK!
                break;
                
        }
    }
    
    for( int i = 0; i < plist.getNumChildren(); i++ )
    {
        ImGui::Indent();
        imgui(*plist.getChild(i), cursorPos+20.0 );
        ImGui::Unindent();
    }
    
    //ImGui::TreePop();
    ImGui::PopID();
}

}