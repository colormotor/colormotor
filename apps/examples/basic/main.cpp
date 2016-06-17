/// Main app
/// Note: it should not be necessary to modify much here
/// except for adding your AppModule's 
/// or changing the window size/name
/// Unless some more complex logic has to happen.

#include "colormotor.h"
#include "cm_imgui_app.h"
#include "app.h"

using namespace cm;
using namespace cv;
using namespace cv::ogl;


std::vector <AppModule*> modules;
int curModule = 0;

namespace cm
{
    
int appInit( void* userData, int argc, char **argv)
{
	// add your app modules here
    modules.push_back( new App() );
    
    // initialize
    for( int i = 0; i < modules.size(); i++ )
		modules[i]->init();
    
    return 1;
}

void appGui()
{
	// Selects current module
	ImGui::BeginToolbar("toolbar",0,0);

	for( int i = 0; i < modules.size(); i++ )
	{
	    //ImGui::PushID();
	    if (ImGui::Selectable(modules[i]->name.c_str(), i==curModule, 0, ImVec2(60,30)))
	    {
			modules[curModule]->deactivated();
			curModule = i;
			modules[curModule]->activated();
	    }
	    
	    //ImGui::PopID();
	    if( i < modules.size() )
			ImGui::SameLine();
       
	}

	ImGui::End();
	modules[curModule]->gui();
}


void appRender( float w, float h )
{
    modules[curModule]->update();

    gfx::clear(0.2,0.2,0.2,1.0);
    gfx::enableDepthBuffer(false);
    gfx::setOrtho(w,h);

    modules[curModule]->render();
}

void appExit()
{
    for( int i = 0; i < modules.size(); i++ )
    {
		modules[i]->exit();
		delete modules[i];
    }

    modules.clear();
}

} // end namespace

int main( int argc, char ** argv )
{
    imguiApp( argc, argv,
	       "GUI Example", 
             1280 , 720 );

}

