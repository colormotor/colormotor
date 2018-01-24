//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
// This file contains compile-time options for ImGui.
// Other options (memory allocation overrides, callbacks, etc.) can be set at runtime via the ImGuiIO structure - ImGui::GetIO().
//-----------------------------------------------------------------------------

#pragma once
//#include <armadillo>
#include "cm.h"
#include <stdio.h>

//---- Define your own ImVector<> type if you don't want to use the provided implementation defined in imgui.h
// #include <vector>
// #define ImVector  std::vector
//#define ImVector  MyVector

//---- Define assertion handler. Defaults to calling assert().
#define IM_ASSERT(_EXPR)  if(!(_EXPR)) assert(0)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define IMGUI_API __declspec( dllexport )
//#define IMGUI_API __declspec( dllimport )

//---- Include imgui_user.inl at the end of imgui.cpp so you can include code that extends ImGui using its private data/functions.
//#define IMGUI_INCLUDE_IMGUI_USER_INL

//---- Include imgui_user.h at the end of imgui.h
// #define IMGUI_INCLUDE_IMGUI_USER_H

//---- Don't implement default handlers for Windows (so as not to link with OpenClipboard() and others Win32 functions)
//#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS
//#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS

//---- Don't implement help and test window functionality (ShowUserGuide()/ShowStyleEditor()/ShowTestWindow() methods will be empty)
//#define IMGUI_DISABLE_TEST_WINDOWS

//---- Don't define obsolete functions names
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//---- Implement STB libraries in a namespace to avoid conflicts
//#define IMGUI_STB_NAMESPACE     ImStb

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.

#define IM_VEC2_CLASS_EXTRA                                                 \
        ImVec2(const cm::V2& f) { x = f.x; y = f.y; }                       \
        ImVec2(const arma::vec& f) { x = f[0]; y = f[1]; }                       \
        operator cm::V2() const { return cm::V2(x,y); } 
       // operator arma::vec() const { return arma::vec({x,y}); }
#define IM_VEC4_CLASS_EXTRA                                                 \
        ImVec4(const cm::V4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }     \
        operator cm::V4() const { return cm::V4(x,y,z,w); }					\
		ImVec4(const arma::vec& f) { x = f[0]; y = f[1]; z = f[2]; w = f[3]; }                           

//---- Freely implement extra functions within the ImGui:: namespace.
//---- Declare helpers or widgets implemented in imgui_user.inl or elsewhere, so end-user doesn't need to include multiple files.
//---- e.g. you can create variants of the ImGui::Value() helper for your low-level math types, or your own widgets/helpers.
/*
namespace ImGui
{
    void    Value(const char* prefix, const MyVec2& v, const char* float_format = NULL);
    void    Value(const char* prefix, const MyVec4& v, const char* float_format = NULL);
}
*/

