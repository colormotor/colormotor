// ImGui GLFW binding with OpenGL
// https://github.com/ocornut/imgui
#include "colormotor.h"
#include "cm_imgui.h"

namespace cm
{

float appWidth();
float appHeight();
double appFrameTime();
V2 appCenter();
    
enum 
{
    GLYPH_SUPPORT_JAPANESE=1,
    GLYPH_SUPPORT_KOREAN=1<<1,
    GLYPH_SUPPORT_CHINESE=1<<2
};

int imguiApp(int argc, char** argv, 
            const char * name, 
            int w, int h,
            std::function<int(void*,int,char**)> appInit_,
            std::function<void(void)> appExit_,
            std::function<void(void)> appGui_,
            std::function<void(float,float)> appRender_,
            const char* font_path=0,
            int font_size=10,
            unsigned int supported_glyphs=0);
}
