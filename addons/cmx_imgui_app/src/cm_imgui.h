#pragma once
#include "cm_gfx.h"
#include "cm_params.h"
#include "colormotor.h"
#include "gfx_ui.h"
#include "imgui.h"
#include "spectrum.h"

namespace ImGui {
void BeginToolbar(const std::string& label, float x, float y);
void BeginHighlightButton();
void EndHighlightButton();
void Title(const std::string& title);
void SetupStyle(ImVec4 col_text,
                ImVec4 col_main,
                ImVec4 col_back,
                ImVec4 col_area);
void SetupStyleFromHue();
bool IconButton(const std::string& str);
bool IconSelectable(const std::string& str, bool active);
bool ColorSelector(const std::string& label, cm::V4* clr);

bool Knob(const char* label,
          float*      p_value,
          float       v_min,
          float       v_max,
          float       angle_padding = 1.);
bool StringCombo(const char*                    label,
                 int*                           current_item,
                 const std::vector<std::string> items);

bool StringInputText(const char* label, std::string& str, int maxLen = 1024);
bool StdListBox(const char*               label,
                int*                      current_item,
                std::vector<std::string>& items,
                int                       height_in_items = -1);
void ShowDemoWindow(bool* p_open);
}  // namespace ImGui

namespace cm {
void imgui(Param* p, std::string label = "");
void imgui(ParamList& plist, float cursorPos = -1);

class Mouse {
 public:
  static bool down(int btn = 0) {
    if (!hasFocus()) return false;

    return ImGui::IsMouseDown(btn);
  }

  static bool clicked(int btn = 0) {
    if (!hasFocus()) return false;

    return ImGui::IsMouseClicked(btn);
  }

  static bool doubleClicked(int btn = 0) {
    if (!hasFocus()) return false;

    return ImGui::IsMouseDoubleClicked(btn);
  }

  static bool released(int btn = 0) {
    if (!hasFocus()) return false;

    return ImGui::IsMouseReleased(btn);
  }

  static float x() {
    ImVec2 p = ImGui::GetMousePos();
    return p.x;
  }

  static float y() {
    ImVec2 p = ImGui::GetMousePos();
    return p.y;
  }

  static V2 pos() {
    ImVec2 p = ImGui::GetMousePos();
    return V2(p.x, p.y);
  }

  static float dx() { return _delta.x; }

  static float dy() { return _delta.y; }

  static V2 delta() { return V2(_delta.x, _delta.y); }

  static V2 dragDelta() {
    ImVec2 d = ImGui::GetMouseDragDelta();
    return V2(d.x, d.y);
  }

  /// Returns true if App has mouse focus (and not the UI)
  static bool hasFocus() {
    // ImGuiIO& io = ImGui::GetIO();
    // return !io.WantCaptureMouse;
    return !ui::hasFocus();
  }

  static ImVec2 _oldp;
  static ImVec2 _delta;
};

class Keyboard {
 public:
  static int getKeyIndex(int key_index) {
    if (key_index < ImGuiKey_COUNT) return ImGui::GetIO().KeyMap[key_index];
    return key_index;
  }

  static bool down(int key_index) {
    return ImGui::IsKeyDown(getKeyIndex(key_index));
  }
  static bool pressed(int key_index, bool repeat = true) {
    return ImGui::IsKeyPressed(getKeyIndex(key_index), repeat);
  }
  static bool released(int key_index) {
    return ImGui::IsKeyReleased(getKeyIndex(key_index));
  }
};

}  // namespace cm
