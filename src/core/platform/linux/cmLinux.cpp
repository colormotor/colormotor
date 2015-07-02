#include "cm.h"
#include "core/cmFileUtils.h"
#include "core/cmUtils.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>


/// Error alert
void  alert(const char *pFormat,... )
{
	assert(false);
}

/// Yes no alert
bool  alertYesNo(const char *pFormat,... )
{
	assert(false);
	return false;
}


/// Message box
void  messageBox(const char *pFormat,... )
{
	assert(false);
}


/// Dialog for opening a folder
bool	openFolderDialog( std::string & str, const char *title )
{
	assert(false);
	return false;
}

/// \todo String!
/// Dialog for opening file
bool		openFileDialog( std::string & path,  const char * type, int maxsize  )
{
	assert(false);
	return false;
}

/// Dialog for saving file
bool		saveFileDialog( std::string & path, const char * type, int maxsize  )
{
	assert(false);
	return false;
}


std::string getResourcePath( const char * identifier, const char * file ) 
{
	assert(false);
	return file;
}

std::string  getExecutablePath()
{
	return std::string("");
}

