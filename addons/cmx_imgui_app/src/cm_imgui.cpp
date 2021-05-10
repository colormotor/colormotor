//
//  cm_imgui.cpp
//  basic
//
//  Created by Daniel Berio on 09/04/16.
//  Copyright (c) 2016 Daniel Berio. All rights reserved.
//

#include "cm_imgui.h"

#include "imgui_internal.h"

namespace cm {
extern ImFont* iconFont;
}

namespace ImGui {
void BeginToolbar(const std::string& label, float x, float y) {
  static bool show = true;
  ImGui::SetNextWindowPos(ImVec2(x, y));
  ImGui::Begin(label.c_str(),
               &show,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
}

void BeginHighlightButton() {
  float h = 34.0;
  ImGui::PushStyleColor(ImGuiCol_Button, (ImU32)ImColor::HSV(h, 0.8f, 0.7f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        (ImU32)ImColor::HSV(h, 0.7f, 0.9f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                        (ImU32)ImColor::HSV(h, 0.8f, 0.9f));
}

void EndHighlightButton() { ImGui::PopStyleColor(3); }

void Title(const std::string& title) {
  static bool alwaystrue = true;
  ImGui::PushStyleColor(ImGuiCol_Header, (ImU32)ImColor::HSV(0.0, 0.0, 0.3));
  ImGui::Selectable(title.c_str(), &alwaystrue);
  ImGui::PopStyleColor(1);
  alwaystrue = true;
}

bool Knob(const char* label,
          float*      p_value,
          float       v_min,
          float       v_max,
          float       angle_padding) {
  ImGuiIO&    io    = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();

  float       radius_outer = 20.0f;
  ImVec2      pos          = ImGui::GetCursorScreenPos();
  ImVec2      center       = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
  float       line_height  = ImGui::GetTextLineHeight();
  ImDrawList* draw_list    = ImGui::GetWindowDrawList();

  float ANGLE_MIN = 0.;             // 3.141592f * (angle_padding);
  float ANGLE_MAX = 3.141592f * 2;  // 3.141592f * (1-angle_padding);

  ImGui::InvisibleButton(
      label,
      ImVec2(radius_outer * 2,
             radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
  bool value_changed = false;
  bool is_active     = ImGui::IsItemActive();
  bool is_hovered    = ImGui::IsItemActive();
  if (is_active && io.MouseDelta.x != 0.0f) {
    float step = (v_max - v_min) / 100.0f;
    *p_value += io.MouseDelta.x * step;
    if (*p_value < v_min) *p_value = v_min;
    if (*p_value > v_max) *p_value = v_max;
    value_changed = true;
  }

  float  t         = (*p_value - v_min) / (v_max - v_min);
  float  angle     = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;
  float  angle_cos = cosf(angle), angle_sin = sinf(angle);
  float  radius_inner = radius_outer * 0.40f;
  ImVec2 tick_pos     = ImVec2(center.x + angle_cos * (radius_outer - 2),
                           center.y + angle_sin * (radius_outer - 2));
  draw_list->AddCircleFilled(center,
                             radius_outer,
                             ImGui::GetColorU32(ImGuiCol_FrameBg),
                             16);
  draw_list->AddLine(ImVec2(center.x + angle_cos * radius_inner,
                            center.y + angle_sin * radius_inner),
                     tick_pos,
                     ImGui::GetColorU32(ImGuiCol_SliderGrabActive),
                     2.0f);
  draw_list->AddCircleFilled(
      center,
      radius_inner,
      ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive
                                   : is_hovered ? ImGuiCol_FrameBgHovered
                                                : ImGuiCol_Header),
      16);
  draw_list->AddCircleFilled(tick_pos,
                             radius_inner * 0.3,
                             ImGui::GetColorU32(ImGuiCol_CheckMark),
                             12);

  draw_list->AddText(
      ImVec2(pos.x, pos.y + radius_outer * 2 + style.ItemInnerSpacing.y),
      ImGui::GetColorU32(ImGuiCol_Text),
      label);

  if (is_active || is_hovered) {
    ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x,
                                   pos.y - line_height -
                                       style.ItemInnerSpacing.y -
                                       style.WindowPadding.y));
    ImGui::BeginTooltip();
    ImGui::Text("%.3f", *p_value);
    ImGui::EndTooltip();
  }

  return value_changed;
}

void SetupStyle(ImVec4 col_text,
                ImVec4 col_main,
                ImVec4 col_back,
                ImVec4 col_area) {
  ImGuiStyle& style = ImGui::GetStyle();

  // style.Colors[ImGuiCol_Text]                  = ImVec4(col_text.x,
  // col_text.y, col_text.z, 1.00f); style.Colors[ImGuiCol_TextDisabled] =
  // ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
  // style.Colors[ImGuiCol_WindowBg]              = ImVec4(col_back.x,
  // col_back.y, col_back.z, 1.00f); style.Colors[ImGuiCol_ChildBg] =
  // ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  // style.Colors[ImGuiCol_Border]                = ImVec4(col_text.x,
  // col_text.y, col_text.z, 0.30f); style.Colors[ImGuiCol_BorderShadow] =
  // ImVec4(0.00f, 0.00f, 0.00f, 0.00f); style.Colors[ImGuiCol_FrameBg] =
  // ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  // style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.68f); style.Colors[ImGuiCol_FrameBgActive] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_TitleBg]               = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.45f); style.Colors[ImGuiCol_TitleBgCollapsed] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
  // style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.78f); style.Colors[ImGuiCol_MenuBarBg] =
  // ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
  // style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(col_area.x,
  // col_area.y, col_area.z, 1.00f); style.Colors[ImGuiCol_ScrollbarGrab] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
  // style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.78f); style.Colors[ImGuiCol_ScrollbarGrabActive]
  // = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_PopupBg]               = ImVec4(col_area.x,
  // col_area.y, col_area.z, 1.00f); style.Colors[ImGuiCol_CheckMark] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
  // style.Colors[ImGuiCol_SliderGrab]            = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.24f); style.Colors[ImGuiCol_SliderGrabActive] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_Button]                = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.44f); style.Colors[ImGuiCol_ButtonHovered] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
  // style.Colors[ImGuiCol_ButtonActive]          = ImVec4(col_main.x,
  // col_main.y, col_main.z, 1.00f); style.Colors[ImGuiCol_Header] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
  // style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.86f); style.Colors[ImGuiCol_HeaderActive] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_Separator]                = ImVec4(col_text.x,
  // col_text.y, col_text.z, 0.32f); style.Colors[ImGuiCol_SeparatorHovered] =
  // ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
  // style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(col_text.x,
  // col_text.y, col_text.z, 1.00f); style.Colors[ImGuiCol_ResizeGrip] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
  // style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(col_main.x,
  // col_main.y, col_main.z, 0.78f); style.Colors[ImGuiCol_ResizeGripActive] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_CloseButton]           = ImVec4(col_text.x,
  // col_text.y, col_text.z, 0.16f); style.Colors[ImGuiCol_CloseButtonHovered]
  // = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
  // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(col_text.x,
  // col_text.y, col_text.z, 1.00f); style.Colors[ImGuiCol_PlotLines] =
  // ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  // style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(col_main.x,
  // col_main.y, col_main.z, 1.00f); style.Colors[ImGuiCol_PlotHistogram] =
  // ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  // style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(col_main.x,
  // col_main.y, col_main.z, 1.00f); style.Colors[ImGuiCol_TextSelectedBg] =
  // ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);

  style.Colors[ImGuiCol_Text] =
      ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
  style.Colors[ImGuiCol_WindowBg] =
      ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
  style.Colors[ImGuiCol_ChildBg] =
      ImVec4(col_back.x * 0.7, col_back.y * 0.7, col_back.z * 0.7, 1.00f);
  style.Colors[ImGuiCol_Border] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] =
      ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
  style.Colors[ImGuiCol_FrameBgActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_TitleBg] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
  style.Colors[ImGuiCol_TitleBgActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_MenuBarBg] =
      ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
  style.Colors[ImGuiCol_ScrollbarBg] =
      ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_PopupBg] =
      ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  style.Colors[ImGuiCol_CheckMark] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
  style.Colors[ImGuiCol_SliderGrab] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
  style.Colors[ImGuiCol_SliderGrabActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_Button] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
  style.Colors[ImGuiCol_ButtonHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
  style.Colors[ImGuiCol_ButtonActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_Header] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
  style.Colors[ImGuiCol_HeaderActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_Separator] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
  style.Colors[ImGuiCol_SeparatorHovered] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
  style.Colors[ImGuiCol_SeparatorActive] =
      ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
  style.Colors[ImGuiCol_ResizeGripHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  // style.Colors[ImGuiCol_CloseButton]           = ImVec4(col_text.x,
  // col_text.y, col_text.z, 0.16f); style.Colors[ImGuiCol_CloseButtonHovered] =
  // ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
  // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(col_text.x,
  // col_text.y, col_text.z, 1.00f);
  style.Colors[ImGuiCol_PlotLines] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] =
      ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] =
      ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);

  //        style.Colors[ImGuiCol_TooltipBg]             = ImVec4(col_main.x,
  //        col_main.y, col_main.z, 0.92f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void SetupStyleFromHue() {
#if 1
  // FIXME: those should become parameters to the function
  static int   hue          = 140;
  static float col_main_sat = 180.f / 255.f;
  static float col_main_val = 161.f / 255.f;
  static float col_area_sat = 124.f / 255.f;
  static float col_area_val = 100.f / 255.f;
  static float col_back_sat = 59.f / 255.f;
  static float col_back_val = 40.f / 255.f;

  ImGui::Begin("Hue Style");
  ImGui::SliderInt("master hue", &hue, 0, 255);

  float  dummy;
  ImVec4 rgb;
  // ImGui::ColorEditMode(ImGuiColorEditMode_HSV);

  ImGui::ColorConvertHSVtoRGB(hue / 255.f,
                              col_main_sat,
                              col_main_val,
                              rgb.x,
                              rgb.y,
                              rgb.z);
  ImGui::ColorEdit3("main", &rgb.x);
  ImGui::ColorConvertRGBtoHSV(rgb.x,
                              rgb.y,
                              rgb.z,
                              dummy,
                              col_main_sat,
                              col_main_val);

  ImGui::ColorConvertHSVtoRGB(hue / 255.f,
                              col_area_sat,
                              col_area_val,
                              rgb.x,
                              rgb.y,
                              rgb.z);
  ImGui::ColorEdit3("area", &rgb.x);
  ImGui::ColorConvertRGBtoHSV(rgb.x,
                              rgb.y,
                              rgb.z,
                              dummy,
                              col_area_sat,
                              col_area_val);

  ImGui::ColorConvertHSVtoRGB(hue / 255.f,
                              col_back_sat,
                              col_back_val,
                              rgb.x,
                              rgb.y,
                              rgb.z);
  ImGui::ColorEdit3("back", &rgb.x);
  ImGui::ColorConvertRGBtoHSV(rgb.x,
                              rgb.y,
                              rgb.z,
                              dummy,
                              col_back_sat,
                              col_back_val);

  ImGui::End();
#endif

  ImGuiStyle& style = ImGui::GetStyle();

  ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
  ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
  ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
  ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

  SetupStyle(col_text, col_main, col_back, col_area);
}

bool IconButton(const std::string& str) {
  ImGui::PushFont(cm::iconFont);
  bool res = ImGui::Button(str.c_str());
  ImGui::PopFont();
  return res;
}

bool IconSelectable(const std::string& str, bool active) {
  ImGui::PushFont(cm::iconFont);
  active = ImGui::Selectable(str.c_str(), active, 0, ImVec2(20., 0));
  ImGui::PopFont();
  return active;
}

bool ColorSelector(const std::string& label, cm::V4* clr) {
  float imclr[4] = {(float)clr->x, (float)clr->y, (float)clr->z, (float)clr->w};

  bool res = ColorEdit4(label.c_str(), imclr);
  *clr     = cm::V4(imclr[0], imclr[1], imclr[2], imclr[3]);
  return res;
}

// Combo box helper allowing to pass an array of strings.
bool StringCombo(const char*                    label,
                 int*                           current_item,
                 const std::vector<std::string> items) {
  std::string str = "";
  for (int i = 0; i < items.size(); i++) {
    str += items[i];
    str += (char)0;
  }
  return ImGui::Combo(label, current_item, str.c_str());
}

bool StringInputText(const char* label, std::string& str, int maxLen) {
  char tmpStr[1024];
  strcpy(tmpStr, str.c_str());

  bool dirty = ImGui::InputText(label, tmpStr, 1024);
  str        = tmpStr;
  return dirty;
}

static bool items_getter(void* pdata, int idx, const char** out_text) {
  std::vector<std::string>* pitems = (std::vector<std::string>*)pdata;
  if (idx < 0 || idx >= pitems->size()) return false;
  *out_text = pitems->at(idx).c_str();
  return true;
}

bool StdListBox(const char*               label,
                int*                      current_item,
                std::vector<std::string>& items,
                int                       height_in_items) {
  return ImGui::ListBox(label,
                        current_item,
                        items_getter,
                        &items,
                        items.size(),
                        height_in_items);
}

}  // namespace ImGui

namespace cm {

ImVec2 Mouse::_oldp;
ImVec2 Mouse::_delta;
static std::string cur_param_path;

static std::string make_entry_str(const std::string& path, const std::string& name)
{
	std::string entrystr = "{\"";
	entrystr += path + "\" : \"";
	entrystr += name + "\"}";
	return entrystr;
}
	
void imgui(Param* p, std::string label) {
  static char tmpStr[1024];

  bool found = true;
  if (label == "") label = p->getName();

  switch (p->getType()) {
    case PARAM_FLOAT:
      if (!p->hasOptions()) {
        p->dirty = ImGui::SliderFloat(label.c_str(),
                                      (float*)p->getAddress(),
                                      p->getMin(),
                                      p->getMax());
      } else if (p->hasOption("v") || p->hasOption("field")) {
        p->dirty = ImGui::InputFloat(label.c_str(), (float*)p->getAddress());
      } else if (p->hasOption("knob")) {
        p->dirty = ImGui::Knob(label.c_str(),
                               (float*)p->getAddress(),
                               p->getMin(),
                               p->getMax());
      } else if (p->hasOption("angle")) {
        p->dirty = ImGui::SliderAngle(label.c_str(),
                                      (float*)p->getAddress(),
                                      p->getMin(),
                                      p->getMax());
      } else if (p->hasOption("vslider")) {
        // quite arbitrary size
        p->dirty = ImGui::VSliderFloat(label.c_str(),
                                       ImVec2(30, 60),
                                       (float*)p->getAddress(),
                                       p->getMin(),
                                       p->getMax());
      } else {
        p->dirty = ImGui::SliderFloat(label.c_str(),
                                      (float*)p->getAddress(),
                                      p->getMin(),
                                      p->getMax());
      }
      break;

    case PARAM_INT:
      p->dirty = ImGui::InputInt(label.c_str(), (int*)p->getAddress());
      break;

    case PARAM_EVENT:
      if (ImGui::Button(label.c_str())) {
        p->dirty = true;
        p->setBool(!p->getBool());
      }
      break;

      /*case PARAM_COLOR:
          ImGui::ColorEdit3(label.c_str(),
         (float*)p->getAddress()); break;
      */

    case PARAM_BOOL:
      p->dirty = ImGui::Checkbox(label.c_str(), (bool*)p->getAddress());
      break;

    case PARAM_UNKNOWN:
      ImGui::Separator();
      break;

    case PARAM_SELECTION:
      p->dirty = ImGui::StringCombo(label.c_str(),
                                    (int*)p->getAddress(),
                                    p->getSelectionNames());
      break;

    case PARAM_STRING: {
      strcpy(tmpStr, p->getString());
      p->dirty = ImGui::InputText(label.c_str(), tmpStr, 50);  // HACK!
      if (p->dirty) p->setString(tmpStr);
      break;
    }

    case PARAM_CSTRING:
      p->dirty = ImGui::InputText(label.c_str(),
                                  (char*)p->getAddress(),
                                  50);  // HACK!
      break;

    case PARAM_COLOR:
      p->dirty = ImGui::ColorSelector(label.c_str(), (V4*)p->getAddress());
      break;

    default:
      found = false;
      break;
  }

  if (found && p->description.length()) {
    if (ImGui::IsItemHovered()) ImGui::SetTooltip(p->description.c_str());
  }
  
	std::string popupid = label + "_copypopup";
	if (ImGui::BeginPopupContextItem(popupid.c_str()))
	{
		if (ImGui::Selectable("Copy path"))
		{
			std::string s = make_entry_str(cur_param_path, p->getName());
			ImGui::SetClipboardText(s.c_str()); //cur_param_path.c_str());
			std::cout << "Copying: " << s << std::endl;
		}
		ImGui::EndPopup();
	}
}


void imgui(ParamList& plist, float cursorPos) {
  static bool opendemo = true;

  std::string id = plist.getPath() + "_ID";
  ImGui::PushID(id.c_str());


	
  // ImGui::BeginChild(id.c_str()); //"content");

  float cposyPrev = ImGui::GetCursorPosY();

  bool vis;
  if (plist.hasOption("hidden"))
    vis = ImGui::CollapsingHeader(
        plist.getName().c_str(),
        ImGuiTreeNodeFlags_AllowItemOverlap);  //, NULL, true,
                                               // true);
  else
    vis = ImGui::CollapsingHeader(
        plist.getName().c_str(),
        ImGuiTreeNodeFlags_AllowItemOverlap |
            ImGuiTreeNodeFlags_DefaultOpen);  //, NULL, true,
                                              // true);

  std::string popupid = id + "_copyallpopup";
	if (ImGui::BeginPopupContextItem(popupid.c_str()))
	{
		if (ImGui::Selectable("Copy path"))
		{
			std::string s = "{\"" + plist.getName() + "\":[\n";
			
			for (int i = 0; i < plist.getNumParams(); i++)
			{
				Param* p = plist.getParam(i);
				s += "\t" + make_entry_str(plist.getPath() + "." + p->getName(), p->getName());
				if (i < plist.getNumParams()-1)
					s += ",\n";
				else
					s += "\n";
			}

			s += "]}";
			ImGui::SetClipboardText(s.c_str());  //cur_param_path.c_str());
			std::cout << "Copying: " << s << std::endl;
		}
		ImGui::EndPopup();
	}
	
  float cposyNext = ImGui::GetCursorPosY();
  // ImGUi::SetCursorPosY(cposyPrev);
  // SameLine();

  // ImGui::PushItemWidth(-10);
  // in cm_imgui_app
  if (!plist.hasOption("child")) {
    extern ImFont* iconFont;
    ImGui::PushFont(iconFont);
    ImGui::PushID("btns");
    ImGui::SameLine(ImGui::GetWindowWidth() - 100);

    if (ImGui::ButtonEx("l",
                        ImVec2(0, 0),
                        ImGuiButtonFlags_PressedOnClick))  // Hack default
                                                           // onRelease with
                                                           // Button does
                                                           // not seem to
                                                           // work
    {
      std::string path;
      if (openFileDialog(path, "xml")) plist.loadXml(path.c_str());
    }
    // ImGui::NextColumn();
    ImGui::SameLine();
    if (ImGui::ButtonEx("s",
                        ImVec2(0, 0),
                        ImGuiButtonFlags_PressedOnClick))  //,
                                                           // ImVec2(100,20)))
    {
      std::string path;
      if (saveFileDialog(path, "xml")) plist.saveXml(path.c_str());
    }
    ImGui::PopID();
    ImGui::PopFont();
  }
  // ImGui::PopItemWidth();

  if (!vis) {
    // ImGui::EndChild();
    ImGui::PopID();
    return;
  }

  // if(!)
  // ImGui::OpenNextNode(true);
  // if(!ImGui::TreeNode(plist.getName().c_str()))

  for (int i = 0; i < plist.getNumParams(); i++) {
    Param* p = plist.getParam(i);
    if (p->hasOption("h")) continue;
    if (p->hasOption("sameline")) ImGui::SameLine();

	cur_param_path = plist.getPath() + "." + p->getName();
			
    imgui(p);
  }

	if (false) //cursorPos < 0)
	{
		
	}else{
		for (int i = 0; i < plist.getNumChildren(); i++) {
			ImGui::Indent();
			imgui(*plist.getChild(i), cursorPos + 10.0);
			ImGui::Unindent();
  }

	}
  // ImGui::TreePop();
  ImGui::PopID();
  // ImGui::EndChild();
}

}  // namespace cm
