//////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//	� Daniel Berio 2008													
//	  http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "engine/engineIncludes.h"
#include "engine/renderSys/Scene.h"
#include "engine/renderSys/Camera.h"
#include "engine/effects/Effect.h"

namespace cm {

class Renderer
{
public:
	Renderer();
	Renderer( int w, int h );

	~Renderer();
	
	bool init( int w, int h );
	
	void release();
	
	void clear();
	void addScene( Scene * scene );
	void addRenderable( Renderable * r, bool addToRoot = true );
	void addNode( Node * r, bool addToRoot = true );
	void removeNode( Node * n );
	RenderableNode* addMesh( Mesh * m, const Color & clr = Color::white());
	RenderableNode* addMeshToNode( Mesh * m, Node * node, const Color & clr = Color::white());
	
	void addEffect( Effect * effect );
	void updateShadows();
	
	//Scene * addScene( const char * path );
	//void setCamera( Camera * cam ) { camera = cam; }
	//Camera * getCamera() const { return camera; }
	
	void begin( Camera & cam, double msecs, bool bDebug = false );
	void begin( const cm::M44 & viewMat, const cm::M44 & projMat, double msecs, bool bDebug = false );
	void end( bool bPostProcess = true  );
	
	void render( const cm::M44 & viewMat, const cm::M44 & projMat, double msecs, bool bDebug, bool bPostProcess = true  );
	void postProcess( Texture * sceneTex = 0 );
	
	const Node * getRootNode() const { return root; }
	Node * getRootNode() { return root; }
	
	void setLightPos( const Vec3 & pos ) { lightPos = pos; }
	void setLightPos( float x, float y, float z ) { lightPos(x,y,z); }
	
	cm::Vec3 lightPos;
	
	bool useHalfEdge;
	bool front;
	bool back;
	bool zfail;
	
	int width;
	int height;
	
	float extrusion;
	float fog;
	float contrast;
	float exposure;
	float ambientAmt;
	cm::Color ambientColor;
	float diffuseMin;
	float shadowVal;
	
	bool initialized;
	ParamList params;
	
	
	bool bShadows;
	bool wireframe;
private:
	enum
	{
		STENCIL_VAL	= 128
	};
	
	void drawWeird( float alpha );
	
	void drawShadowVolumes();
	void markShadows( bool bDebug = false );
	void ambientPass( bool bDebug = false );
	void diffusePass( bool bDebug = false );

	void ambientAndDiffusePass();
	void addShadows();
	
	void addRenderableNode( RenderableNode * r );
	void addRenderableNode( Scene * s, RenderableNode * r );
	void addNode( Scene * s, Node * node );
	void removeNodeFromGroup( Group<RenderableNode> & group, Node * n );
	
	RefPtr<Node> root;
	
	Group<RenderableNode> shadowNodes;
	Group<RenderableNode> renderNodes;
	Group<Effect> effects;
	
	ShaderProgram weirdShader;
	
	ShaderProgram shadowShader;
	ShaderProgram ambientShader;
	ShaderProgram diffuseShader;
	ShaderProgram ambientDiffuseShader;
	ShaderProgram ambientDiffuseTexShader;
	
	Texture	lightTex;

	RefPtr<Texture> bgTex;
	Texture resultTex;
	
	cm::M44 viewMatrix;

	// hacky...
	ShaderProgram * curShader;
	Texture * curTexture;
	
	//Camera * camera;
	
};
}