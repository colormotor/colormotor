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

namespace cm
{



/// \addtogroup Gfx
/// @{

/// \addtogroup RenderSystem
/// @{

/// Texure semantic type
enum TEXTURE_SEMANTIC
{
	TS_UNKNOWN = -1,
	TS_DIFFUSE = 0,
	TS_BUMP,
	TS_REFLECTION,
	TS_SPECULAR,
	NUM_TEXTURE_SEMANTICS
};

/// Descriptor for a material texture
/// contains (optional) info for sampler channel etc for automatic shader generation and import/export.
struct MtlTextureDesc
{
	MtlTextureDesc() 
	:
	semantic(TS_UNKNOWN),
	sampler(0),
	channel(0),
	texture(0),
	name("tex"),
	local(false)
	{
	}
	
	TEXTURE_SEMANTIC semantic;
	int			 sampler;
	int			 channel;
	bool		local; /// < Tells us if texture should be deleted when material is deleted
	Texture		*texture;
	std::string	name; // \todo eliminare?
};

/*
class MaterialFactory : public RttiFactory
{
	
	public:	
		MaterialFactory() {}
		virtual ~MaterialFactory() {}
	
		CM_SINGLETON( MaterialFactory )

};*/

/// Material class
/// \todo ADD PASSES -> this way we can make stuff like hair etc
class  Material : public Base
{
public:
	RTTIOBJECT( "Material", Material, Base )
	Material();

	~Material()
	{
		release();
	}

	virtual void release();
	virtual bool init() { return true; }
	virtual void begin();
	virtual void end();
	virtual void bindTexture( int index );
	
	/// User update function 
	virtual void update( double msecs ) {}
	
	/// Ovveride if material uses a shader
	virtual ShaderProgram * getShader() { return 0; }
	
	///   Get num textures for material
	int	getNumTextures() { return textures.size(); }

	///   Get texture at index
	Texture *		getTexture( int ind ) { return textures[ind].texture; }

	///   Get texture name at index
	const char *	getTextureName( int ind ) const { return textures[ind].name.c_str(); }

	/// Get texture descriptor at index
	const MtlTextureDesc & getTextureDesc( int ind ) const { return textures[ind]; }
	/// Get texture descriptor modifiable ref. at index
	MtlTextureDesc & getTextureDesc(  int ind )  { return textures[ind]; }

	///   Add a texture to material
	/// if sampler index is -1 samplers are assigned in consecutive order from 0
	
	bool			reloadTexture( int index, const char * filename );
	bool			addTexture( const char * filename, int sampler = -1, TEXTURE_SEMANTIC semantic = TS_UNKNOWN,  int channel = 0);
	bool			addTexture( Texture * tex, int sampler = -1,  TEXTURE_SEMANTIC semantic = TS_UNKNOWN,  int channel = 0);

	/// bind texture function, in case we use shaders it may be necessary to set extra info for texture
	
	/// \todo functions. ! public
	cm::Color			diffuseColor;
	cm::Color			specularColor;
	cm::Color			ambientColor;
	bool			hasAmbient;
	bool			hasSpecular;
	bool			hasDiffuse;
	
	ParamList	params;
	
protected:
	/// material textures
	std::vector<MtlTextureDesc>	textures;
	
};

//CM_SINGLETON_IMPL(MaterialFactory)

/// @}

/// @}


}