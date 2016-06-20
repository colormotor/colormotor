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
    
int imguiApp(int argc, char** argv, 
            const char * name, 
            int w, int h,
            std::function<int(void*,int,char**)> appInit_,
            std::function<void(void)> appExit_,
            std::function<void(void)> appGui_,
            std::function<void(float,float)> appRender_);
}
