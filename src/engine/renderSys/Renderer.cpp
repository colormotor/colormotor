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

#include "Renderer.h"
#include "ShadowMesh.h"
#include "shaders/shadowVolume.h"
#include "shaders/ambientPass.h"
#include "shaders/diffusePass.h"
#include "shaders/weird1.h"
#include "shaders/ambientAndDiffuse.h"
#include "shaders/ambientAndDiffuseTex.h"

namespace cm
{

void drawBillboard( Texture * tex, const float * viewm, const float * pos, float w, float h )
{
CM_GLCONTEXT
#define M(row,col) viewm[col*4+row]
	float x[3] = { M(0,0)*w, M(0,1)*w, M(0,2)*w };
	float y[3] = { M(1,0)*h, M(1,1)*h, M(1,2)*h };
	
	bool flip = false;
	
	float mu = tex->info.maxU;
	float mv = tex->info.maxV;
	
	float uv[4][2]
	=
	{
		{ 0.0f, mv },
		{ 0.0f, 0.0f },
		{ mu, 0.0f },
		{ mu, mv } 
	};
		
	float verts[4][3] = {
	{ pos[0] - x[0] + y[0], pos[1] - x[1] + y[1], pos[2] - x[2] + y[2] },
	{ pos[0] - x[0] - y[0], pos[1] - x[1] - y[1], pos[2] - x[2] - y[2] },
	{ pos[0] + x[0] - y[0], pos[1] + x[1] - y[1], pos[2] + x[2] - y[2] },
	{ pos[0] + x[0] + y[0], pos[1] + x[1] + y[1], pos[2] + x[2] + y[2] }
	};

	tex->bind();
	CM_GLERROR;
	
	GeometryDrawer::reset();
	GeometryDrawer::setTexCoordPointer(uv);
	GeometryDrawer::setVertexPointer(verts);
	GeometryDrawer::drawArrays(TRIANGLEFAN,4,0);
	GeometryDrawer::reset();
	
	CM_GLERROR;
	tex->unbind();
	
}

void makeLightTexture( Texture * tex )
{
	int w = tex->getWidth();
	int h = tex->getHeight();
	
	float c[2] = { 0.5f*w, 0.5f*h };
	float len[2] = {  c[0]*c[0], c[1]*c[1] };
	
	tex->lock();
	for( int y = 0; y < h; y++ )
		for( int x = 0; x < w; x++ )
		{
			float p[2] = { (float)x,(float)y };
			float d[2] = { p[0] - c[0], p[1] - c[1] };
			float l = 1.0-( (d[0]*d[0]) + (d[1]*d[1]) )/len[0];
			l = clamp(l,0.0f,1.0f);
			
			tex->pixel(x,y,l,l,l,l);
		}
	tex->unlock();
	CM_GLERROR;
}

//////////////////////////////////////////////////////////////////////////

Renderer::Renderer( int w, int h )
:
bgTex(0),
initialized(false),
bShadows(true),
wireframe(false),
ambientColor(0.4,0.4,0.4,1.0),
curShader(0),
curTexture(0)
{
	front = true;
	back = true;
	zfail = false;
	useHalfEdge = false;

	root = new Node();
	root->setLabel("root");
	lightPos(10,5,3);
	extrusion = 100;
	exposure = 1.0;
	contrast = 1.0;
	ambientAmt = 0.3;
	fog = 100000;
	
	params.addFloat("extrusion",&extrusion,0,50);
	params.addFloat("exposure",&exposure,0.4,3.0);
	params.addFloat("contrast",&contrast,1.0,2.0);
	params.addFloat("ambientAmt",&ambientAmt,0.0,1.0);
	params.addFloat("diffuseMin",&diffuseMin,0.0,0.5);
	params.addFloat("shadow val",&shadowVal,0.0,1.0);
	params.addSpacer();
	
	params.addBool("wireframe",&wireframe);
	params.addBool("shadows",&bShadows);
	params.addFloat("amb r",&ambientColor.r,0,1);
	params.addFloat("amb g",&ambientColor.g,0,1);
	params.addFloat("amb b",&ambientColor.b,0,1);
	params.addSpacer();
	params.addBool("z fail",&zfail);

	init(w,h);
}

Renderer::Renderer()
:
bgTex(0),
initialized(false),
bShadows(true),
wireframe(false),
ambientColor(0.4,0.4,0.4,1.0),
curShader(0),
curTexture(0)
{
	front = true;
	back = true;
	zfail = false;
	
	root = new Node();
	root->setLabel("root");
	lightPos(10,5,3);
	extrusion = 100;
	exposure = 1.0;
	contrast = 1.0;
	ambientAmt = 0.3;
	fog = 100000;
	
	params.addFloat("extrusion",&extrusion,0,50);
	params.addFloat("exposure",&exposure,0.4,3.0);
	params.addFloat("contrast",&contrast,1.0,2.0);
	params.addFloat("ambientAmt",&ambientAmt,0.0,1.0);
	params.addFloat("diffuseMin",&diffuseMin,0.0,0.5);
	params.addFloat("shadow val",&shadowVal,0.0,1.0);
	params.addSpacer();
	
	params.addBool("wireframe",&wireframe);
	params.addBool("shadows",&bShadows);
	params.addFloat("amb r",&ambientColor.r,0,1);
	params.addFloat("amb g",&ambientColor.g,0,1);
	params.addFloat("amb b",&ambientColor.b,0,1);
	params.addSpacer();
	params.addBool("z fail",&zfail);
	
}

//////////////////////////////////////////////////////////////////////////
 
Renderer::~Renderer()
{
	release();
	root = 0;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::release()
{
	clear();
	lightTex.release();
	shadowShader.release();
	ambientShader.release();
	diffuseShader.release();
	bgTex = 0;
	params.release();
}

//////////////////////////////////////////////////////////////////////////

void Renderer::clear()
{
	root = new Node();
	root->setLabel("root");
	shadowNodes.release();
	renderNodes.release();
	effects.release();
}

//////////////////////////////////////////////////////////////////////////

bool Renderer::init( int w, int h )
{
	width = w;
	height = h;
	
	if( !shadowShader.loadString( vsShadow, psShadow ) )
	{
		debugPrint("Could not load shadowShader \n");
		return false;
	}
	
	if( !ambientShader.loadString( vsAmbient, psAmbient ) )
	{
		debugPrint("Could not load ambientShader \n");
		return false;
	}
	
	if( !diffuseShader.loadString( vsDiffuse, psDiffuse ) )
	{
		debugPrint("Could not load diffuseShader \n");
		return false;
	}
	
	if( !weirdShader.loadString( vsWeird, psWeird ) )
	{
		debugPrint("Could not load weirdShader \n");
		return false;
	}
	
	if( !ambientDiffuseShader.loadString( vsAmbientDiffuse, psAmbientDiffuse ) )
	{
		debugPrint("Could not load ambientDiffuseShader \n");
		return false;
	}

	if( !ambientDiffuseTexShader.loadString( vsAmbientDiffuseTex, psAmbientDiffuseTex ) )
	{
		debugPrint("Could not load ambientDiffuseTexShader \n");
		return false;
	}
	
	lightTex.create(256,256);
	makeLightTexture(&lightTex);
	
	resultTex.create(w,h);
//	resultTex.flipY = true;
	
	
	for( int i = 0; i < effects.size(); i++ )
		if( !effects[i]->initialized )
			effects[i]->init(w,h);
			
	//params.addFloat("fog",&fog,0.00001,0.01);
	initialized = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::addRenderable(  Renderable * r, bool addToRoot )
{
	RenderableNode * n = new RenderableNode();
	n->addRenderable(r);
	addNode(n,addToRoot);
}

//////////////////////////////////////////////////////////////////////////

void Renderer::addNode(  Node * n, bool addToRoot )
{
	if(addToRoot)
		root->addChild(n);
		
	if( n->isOfClassType("RenderableNode") )
	{
		RenderableNode * rn = (RenderableNode*)n;
		addRenderableNode( rn );
	}
	
	for( int i = 0; i < n->getNumChildren(); i++ )
		addNode(n->getChild(i),false);
}

//////////////////////////////////////////////////////////////////////////

RenderableNode* Renderer::addMesh( Mesh * m, const Color & clr )
{
	RenderableNode * rn = new RenderableNode();
	RenderableModel * rm = new RenderableModel();
	rn->addRenderable(rm);
	rm->setDiffuseColor(clr);
	rm->addMesh(m);
	addNode(rn);
	return rn;
}

RenderableNode* Renderer::addMeshToNode( Mesh * m, Node * node, const Color & clr)
{
	RenderableNode * rn = new RenderableNode();
	RenderableModel * rm = new RenderableModel();
	rn->addRenderable(rm);
	rm->setDiffuseColor(clr);
	rm->addMesh(m);
	node->addChild(rn);
	return rn;
}
	

//////////////////////////////////////////////////////////////////////////

void Renderer::addRenderableNode(  RenderableNode * n )
{
	bool foundAny = false;
	bool foundShadow = false;
	
	for( int j = 0; j < n->getNumObjects(); j++ )
	{
		Renderable * r = n->getRenderable(j);
		foundAny = true;
		if( !r->isOfClassType("RenderableModel") )
			continue;
		foundShadow = true;
		RenderableModel * rm = (RenderableModel*)r;
		rm->buildShadowVolumes(true,useHalfEdge); //n->getNodeMatrix(),lightPos);
		
	}
	
	if(foundShadow)
	{
		shadowNodes.add(n);
	}
	
	if(foundAny)
	{
		renderNodes.add(n);
	}
}

//////////////////////////////////////////////////////////////////////////

void Renderer::addRenderableNode( Scene * scene, RenderableNode * n )
{
	bool foundMesh = false;
	bool foundShadow = false;
	
	for( int j = 0; j < n->getNumObjects(); j++ )
	{
		Renderable * r = n->getRenderable(j);
		
		if( !r->isOfClassType("RenderableModel") )
			continue;
			
		RenderableModel * rm = (RenderableModel*)r;
		
		for( int i = 0; i < rm->getNumMeshes(); i++ )
		{
			Mesh * m = rm->getMesh(i);
			int ind = m->getId();
			
			// if shadow mesh is not in scene create it
			if( ind != -1 )
			{
				Mesh * shad = scene->getShadowMesh(ind);
				if(shad==0)
					shad = generateShadowMesh(m,true,useHalfEdge);
				scene->setShadowMesh(ind,shad);
				rm->setShadowMesh(i,shad);
				foundShadow = true;
			}
			
			foundMesh = true;
		}
	}
	
	if(foundMesh)
	{
		renderNodes.add(n);
	}
	
	if(foundShadow)
	{
		shadowNodes.add(n);
	}
	
}

//////////////////////////////////////////////////////////////////////////

void Renderer::addNode( Scene * scene, Node * node )
{
	if( node->isOfClassType("RenderableNode") )
	{
		RenderableNode * rn = (RenderableNode*)node;
		addRenderableNode( scene, rn );
	}
	
	for( int i = 0; i < node->getNumChildren(); i++ )
		addNode(scene,node->getChild(i));
}

//////////////////////////////////////////////////////////////////////////


void Renderer::addScene( Scene * scene )
{
	for( int i = 0; i < scene->getNumChildren(); i++ )
		addNode(scene, scene->getChild(i));
		
	root->addChild(scene);
}

void Renderer::removeNode( Node * n )
{
	removeNodeFromGroup(shadowNodes,n);
	removeNodeFromGroup(renderNodes,n);
	
	n->getParent()->removeChild(n);
	//root->removeChild(n);
}

void Renderer::removeNodeFromGroup( Group<RenderableNode> & group, Node * n )
{
	if( n->isOfClassType("RenderableNode") )
	{
		group.remove((RenderableNode*)n);
	}
	
	for( int i = 0; i < n->getNumChildren(); i++ )
		removeNodeFromGroup(group,n->getChild(i));
}

//////////////////////////////////////////////////////////////////////////
/*
Scene * Renderer::addScene( const char * path )
{
}
	*/
//////////////////////////////////////////////////////////////////////////

void Renderer::ambientPass( bool bDebug )
{
	enableDepthBuffer(true);
	setBlendMode(BLENDMODE_NONE);
	setCullMode(CULL_CW);
	
	
	pushMatrix();
	
	CM_GLERROR;
	
	
	for( int i = 0; i < renderNodes.size(); i++ )
	{
		RenderableNode * n = renderNodes[i];
		setModelViewMatrix( n->getWorldMatrix() );
		
		for( int j = 0; j < n->getNumObjects(); j++ )
		{
			Renderable * r = n->getRenderable(j);
			
			// add a default material if necessary
			if(!r->getNumMaterials())
				r->addMaterial();
				
			for( int k = 0; k < r->getNumMaterials(); k++ )
			{
				Material * mtl = r->getMaterial(k);
				if( !mtl->hasAmbient )
					continue;

				ShaderProgram * shad = 0;//mtl->getShader();
				bool hasShader = shad;
				if(!shad)
					shad = &ambientShader;
				if(!hasShader)
					shad->bind();
					
				if( !mtl->getShader() )
				mtl->begin();
				shad->setFloat("ambientAmt",ambientAmt);
				shad->setVec4( "ambient", mtl->ambientColor );
				shad->setVec4( "diffuse", mtl->diffuseColor );
				r->render(k,false);
				mtl->end();
				
				if(!hasShader)
					shad->unbind();
			}
		}
	}
	
	popMatrix();
}

void Renderer::diffusePass( bool bDebug )
{
CM_GLCONTEXT
	if( !bDebug )
	{
		glDisable(GL_POLYGON_OFFSET_FILL);

		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glActiveStencilFaceEXT(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, STENCIL_VAL, 0xFFFFFFFF);

		glActiveStencilFaceEXT(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, STENCIL_VAL, 0xFFFFFFFF);
		glEnable(GL_STENCIL_TEST);
	}

	setBlendMode(BLENDMODE_ADDITIVE);
	setCullMode(CULL_CW);
	
	pushMatrix();
	
	for( int i = 0; i < renderNodes.size(); i++ )
	{
		RenderableNode * n = renderNodes[i];
		setModelViewMatrix( n->getWorldMatrix() );
		
		for( int j = 0; j < n->getNumObjects(); j++ )
		{
			Renderable * r = n->getRenderable(j);
			
			for( int k = 0; k < r->getNumMaterials(); k++ )
			{
				Material * mtl = r->getMaterial(k);
				if( !mtl->hasDiffuse )
					continue;

				ShaderProgram * shad = mtl->getShader();
				bool hasShader = shad;
				if(!shad)
					shad = &diffuseShader;
				if(!hasShader)
					shad->bind();
					
				mtl->begin();
				shad->setVec3("lightPos",lightPos);
				shad->setMatrix4x4("viewMatrix",viewMatrix);
				shad->setFloat("exposure",exposure);
				shad->setVec4( "diffuse", mtl->diffuseColor );
				
				r->render(k,false);
				mtl->end();
				if(!hasShader)
					shad->unbind();
				
			}
		}
	}
	
	popMatrix();
	
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
}
  
void Renderer::ambientAndDiffusePass()
{
CM_GLCONTEXT
	if(wireframe)
	{
		setFillMode(FILL_WIRE);
		setBlendMode(BLENDMODE_ADDITIVE);
		enableDepthBuffer(false);
	} 
	else
	{
		enableDepthBuffer(true);
		setFillMode(FILL_SOLID);
		setBlendMode(BLENDMODE_ALPHA);
	}
	
	setCullMode(CULL_NONE);
	glDisable(GL_STENCIL_TEST);
	pushMatrix();
	
	for( int i = 0; i < renderNodes.size(); i++ )
	{
		RenderableNode * n = renderNodes[i];
		if(!n->isVisible())
			continue;
			
		setModelViewMatrix( n->getWorldMatrix() );
		
		for( int j = 0; j < n->getNumObjects(); j++ )
		{
			Renderable * r = n->getRenderable(j);
			
			// add a default material if necessary
			if(!r->getNumMaterials())
				r->addMaterial();
				
			if(r->invisible)
				continue;
			
			for( int k = 0; k < r->getNumMaterials(); k++ )
			{
				Material * mtl = r->getMaterial(k);
				if( !mtl->hasDiffuse )
					continue;

				/*ShaderProgram * shad = mtl->getShader();
				bool hasShader = shad;
				if(!shad)
				{
					if(mtl->getNumTextures())
						shad = &ambientDiffuseTexShader;
					else
						shad = &ambientDiffuseShader;
				}
				
				if(!hasShader && shad != curShader )
				{
					shad->bind();
				}
				else
				{
					if( curShader )
						curShader->unbind();
					curShader = 0;
				}
				
				mtl->begin();*/
				
				// base material 
				// bind textures and shaders manually
				if( strcmp( mtl->getClassType(), "Material" ) == 0 )
				{
					ShaderProgram * shad = mtl->getShader();
					
				 	if(!shad)
					{
						if(mtl->getNumTextures())
							shad = &ambientDiffuseTexShader;
						else
							shad = &ambientDiffuseShader;
							
						if(shad != curShader )
						{
							shad->bind();
							curShader = shad;
						}

						if( mtl->getNumTextures() )//&& mtl->getTexture(0) != curTexture )
						{
							curTexture = mtl->getTexture(0);
							curTexture->bind();
						}
						else
						{
							curTexture = 0;
						}
						
						
					}

					// set default params
					shad->setVec3("lightPos",lightPos);
					shad->setMatrix4x4("viewMatrix",viewMatrix);
					shad->setFloat("exposure",exposure);
					shad->setFloat("contrast",contrast);
					shad->setVec4( "diffuse", mtl->diffuseColor );
					shad->setFloat("ambientAmt",ambientAmt);
					shad->setFloat("diffuseMin",diffuseMin);
					shad->setVec4( "ambient", ambientColor );
					shad->setFloat("fog",1.0/fog);
						
					CM_GLERROR;

					r->render(k,false);
					
					//shad->unbind();
					//if( curTexture )
					//	curTexture->unbind();
						
				}
				else 
				{
					curTexture = 0;
					curShader = 0;
					mtl->begin();
					r->render(k,false);
					mtl->end();
				}
				
			}
		}
	}
	
	if( curShader )
		curShader->unbind();
	if( curTexture )
		curTexture->unbind();
	curShader = 0;
	curTexture = 0;
	
	setBlendMode(BLENDMODE_NONE);
	popMatrix();
	
	setFillMode(FILL_SOLID);
}
	
	
void Renderer::updateShadows()
{
	for( int i = 0; i < shadowNodes.size(); i++ )
	{
		RenderableNode * n = shadowNodes[i];
		if(!n->isVisible())
			continue;
		
		for( int j = 0; j < n->getNumObjects(); j++ )
			n->getRenderable(j)->updateShadows();//n->getWorldMatrix(),lightPos);
	}
}

void Renderer::addShadows()
{
	CM_GLCONTEXT
	
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	glDepthFunc(GL_LEQUAL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glActiveStencilFaceEXT(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, STENCIL_VAL, 0xFFFFFFFF);

	glActiveStencilFaceEXT(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_NOTEQUAL, STENCIL_VAL, 0xFFFFFFFF);
	glEnable(GL_STENCIL_TEST);

	
	enableDepthBuffer(false);
	setBlendMode(BLENDMODE_MULTIPLY);
	setCullMode(CULL_NONE);
	
	setIdentityTransform();
	gfx::color(shadowVal,shadowVal,shadowVal,1.0);
	gfx::fillRect(-1,-1,2,2);
	
	glDepthMask(GL_TRUE);
	glActiveStencilFaceEXT(GL_FRONT);
	glDisable(GL_STENCIL_TEST);
	glActiveStencilFaceEXT(GL_BACK);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	enableStencilBuffer(false);
	glDisable(GL_POLYGON_OFFSET_FILL);
}
	
void Renderer::markShadows( bool bDebug )
{
CM_GLCONTEXT
	
	if(!bDebug)
	{
		setBlendMode(BLENDMODE_NONE);
	}
	else
	{
		setBlendMode(BLENDMODE_ALPHA);
	}
	
	
	if( bDebug )
	{
		glDepthFunc(GL_LEQUAL);
		setFillMode(FILL_WIRE);
		if(front)
		{
			setCullMode(CULL_CW);
			drawShadowVolumes();
		}
		
		if(back)
		{
			setCullMode(CULL_CCW);
			drawShadowVolumes();
		}
		setFillMode(FILL_SOLID);
		return;
	}
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0f, 1.5f);
		
	if(zfail)
	{
	   glDisable(GL_CULL_FACE);
		
	   // apply stencil
	   glDepthMask(GL_FALSE);
	   glEnable(GL_DEPTH_TEST);
	   glDepthFunc(GL_LESS);
	   
	   glEnable(GL_STENCIL_TEST);
	   glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	   glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	   
	   glActiveStencilFaceEXT(GL_BACK);
	   glStencilOp(GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
	   glStencilFunc(GL_ALWAYS, STENCIL_VAL, 0xFFFFFFFF);
	
	   glActiveStencilFaceEXT(GL_FRONT);
	   glStencilOp(GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);
	   glStencilFunc(GL_ALWAYS, STENCIL_VAL, 0xFFFFFFFF);
	   
	   clearStencil(STENCIL_VAL);
	   
	   drawShadowVolumes();
	}
	else
	{
		glDisable(GL_CULL_FACE);
		
	   // apply stencil
	   glDepthMask(GL_FALSE);
	   glDepthFunc(GL_LESS);
		
	   glEnable(GL_STENCIL_TEST);
	   glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	   glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		
	   glActiveStencilFaceEXT(GL_FRONT);
	   glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP_EXT);
	   glStencilFunc(GL_ALWAYS, STENCIL_VAL, 0xFFFFFFFF);
		
	   glActiveStencilFaceEXT(GL_BACK);
	   glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT);
	   glStencilFunc(GL_ALWAYS, STENCIL_VAL, 0xFFFFFFFF);
	   
	   clearStencil(STENCIL_VAL);
	   
	   drawShadowVolumes();
	}
	
}

void Renderer::drawWeird( float alpha )
{
CM_GLCONTEXT
	
	
	pushMatrix();
	weirdShader.bind();
	weirdShader.setVec3("lightPos",lightPos);
	weirdShader.setMatrix4x4("viewMatrix",viewMatrix);
	weirdShader.setFloat("extrusion",extrusion);
	weirdShader.setFloat("exposure",exposure);
	weirdShader.setFloat("alpha",alpha);
	
	for( int i = 0; i < shadowNodes.size(); i++ )
	{
		RenderableNode * n = shadowNodes[i];
		setModelViewMatrix( n->getWorldMatrix() );
		for( int j = 0; j < n->getNumObjects(); j++ )
		{
			Renderable * r = n->getRenderable(j);
			Material * mtl = 0;
			if(r->getNumMaterials())
				mtl = r->getMaterial(0);
			if(mtl)
				weirdShader.setVec4("diffuse",mtl->diffuseColor);
				
			n->getRenderable(j)->renderShadows();
		}
	}
	weirdShader.unbind();
	popMatrix();
}

void Renderer::drawShadowVolumes()
{
	pushMatrix();
	shadowShader.bind();
	shadowShader.setVec3("lightPos",lightPos);
	shadowShader.setMatrix4x4("viewMatrix",viewMatrix);
	shadowShader.setFloat("extrusion",extrusion);
	
	for( int i = 0; i < shadowNodes.size(); i++ )
	{
		RenderableNode * n = shadowNodes[i];
		if(!n->isVisible())
			continue;

		setModelViewMatrix( n->getWorldMatrix() );
		for( int j = 0; j < n->getNumObjects(); j++ )
			n->getRenderable(j)->renderShadows();
	}
	shadowShader.unbind();
	popMatrix();
}

void Renderer::begin( Camera & cam, double msecs, bool bDebug )
{
	begin(cam.getViewMatrix(),cam.getProjectionMatrix(),msecs,bDebug);
	cam.apply();
}

void Renderer::begin( const cm::M44 & viewMat, const cm::M44 & projMat, double msecs, bool bDebug )
{
	CM_GLCONTEXT
	
	curShader = 0;
	pushMatrix();
	
	viewMatrix = viewMat;
	
	//glEnable(GL_DEPTH_CLAMP_NV);
	
	enableDepthBuffer(true);
	enableStencilBuffer(true);
	
	clearStencil(STENCIL_VAL);
	setCullMode(CULL_CW);
	setBlendMode(BLENDMODE_NONE);
	setDepthFunc(DEPTH_LEQUAL);
	
	root->traverse(viewMat,msecs);
	
	//if( bShadows )
	//	updateShadows();
	
	ambientAndDiffusePass();
	
	// draw transparent stuff ( and light )
	setBlendMode(BLENDMODE_ALPHA_PREMULTIPLIED);
	setCullMode(CULL_NONE);
	//drawBillboard(&lightTex,viewMat,lightPos,3,3);
	
	if(bShadows)
	{
		markShadows(bDebug);
		addShadows();
	}
	/*
	if( !bDebug )
	{
		ambientPass( false );
		markShadows( false );
	
		diffusePass( false );
	}
	
	enableStencilBuffer(false);
	if( bDebug )
	{
		setBlendMode(BLENDMODE_ADDITIVE);
		glDepthFunc(GL_ALWAYS);
		setCullMode(CULL_NONE);
		setFillMode(FILL_SOLID);
		
		setFillMode(FILL_SOLID);
		drawWeird(0.5);
		setFillMode(FILL_WIRE);
		drawWeird(1.2);
		setFillMode(FILL_SOLID);
//		markShadows(true);
	}
	*/
	
	enableDepthBuffer(false);
	enableStencilBuffer(false);
	setBlendMode(BLENDMODE_ALPHA_PREMULTIPLIED);
	glDepthFunc(GL_LEQUAL);
	setCullMode(CULL_NONE);
	setFillMode(FILL_SOLID);
	
}

void Renderer::end( bool bPostProcess )
{
	if(bPostProcess)
		postProcess();
	//drawBillboard(&lightTex,viewMat,lightPos,1,1);
	
	popMatrix();
}

void Renderer::render( const cm::M44 & viewMat, const cm::M44 & projMat, double msecs, bool bDebug, bool bPostProcess  )
{
	begin(viewMat,projMat,msecs,bDebug);
	end(bPostProcess);
	/*setModelViewMatrix(viewMat);
	cm::M44 m;
	m.translation(lightPos);
	drawAxis(m,2.0);*/
	
	
}
	
void Renderer::addEffect( Effect * e )
{
	effects.add(e);
	
	if(initialized)
		e->init(width,height);
	
	params.addParams( e->params );
}

void Renderer::postProcess( Texture * sceneTex )
{
	bool anyEffectsActive = false;
	for( int i = 0; i < effects.size(); i++ )
	{
		if(effects.get(i)->active)
		{
			anyEffectsActive = true;
			break;
		}
	}
	
	if(!anyEffectsActive)
		return;
		

	
	Texture * tex = sceneTex;
	
	// grab frame buffer if no texture supplied
	if(!tex)
	{
		resultTex.grabFrameBuffer();
		tex = &resultTex;
		sceneTex = tex;
	}
	
	Effect * e;
	for( int i = 0; i < effects.size(); i++ )
	{
		e = effects[i];
		e->setInput(0,tex);
		e->apply();
		tex = e->getOutput();
	}
	setIdentityTransform();
	
	setBlendMode(BLENDMODE_ALPHA_PREMULTIPLIED);
	gfx::color(1,1,1,1);
	
	//if(bgTex.isValid())
	//	bgTex.draw();
		
	sceneTex->draw();
	
	setBlendMode(BLENDMODE_ALPHA_PREMULTIPLIED);
	e->getOutput()->draw();
}

}