/*
 *  CMStbImage.h
 *  FFGLPlugins
 *
 *  Created by ensta on 9/29/10.
 *  Copyright 2010 Resolume. All rights reserved.
 *
 */

#pragma once


namespace cm
{

class Texture;
	
/// Convert RGB to BGR and backwards
void swizzleRGB( unsigned char * data, int w, int h, int numComponents );

/// Load image into buffer, outputs width and height and number of components in original image
/// If requestedNumComponents is non zero, image will be converted to requested number of components
unsigned char * loadImage( const char * filename, int * width, int * height, int * numComponents, int requestedNumComponents );

/// Load a texture from an image
Texture * textureFromImage( const char * filename, int requestedNumComponents = 0, int mipLevels = 1 );

/// Load a texture from an image
bool textureFromImage( Texture * tex, const char * filename, int requestedNumComponents = 0, int mipLevels = 1 );

/// Write buffer to png image
bool writeToPng( const char * filenmame, int w, int h, int numComp, const void * data, int strideInBytes = 0 );
/// Write texture to png image
bool writeTextureToPng( const char * filenmame, Texture * tex, int startx = 0, int starty = 0 );

/// Write buffer to tga image
bool writeToTga( const char * filenmame, int w, int h, int numComp, const void * data );
/// Write texture to png image
bool writeTextureToPng( const char * filenmame, Texture * tex );

}