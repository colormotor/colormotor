#include "cm_utils.h"
#include <gtk/gtk.h>

namespace cm
{

static void SetDefaultPath( GtkWidget *dialog, const char *defaultPath )
{
    if ( !defaultPath || strlen(defaultPath) == 0 )
        return;

    /* GTK+ manual recommends not specifically setting the default path.
       We do it anyway in order to be consistent across platforms.

       If consistency with the native OS is preferred, this is the line
       to comment out. -ml */
    gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(dialog), defaultPath );
}

#define MAX_STRLEN 512

static void AddFiltersToDialog( GtkWidget *dialog, const char *type )
{
    GtkFileFilter *filter;
    
    if ( !type || strlen(type) == 0 )
        return;

    std::string wildcard = std::string("*.") + type;
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, type);
    gtk_file_filter_add_pattern( filter, wildcard.c_str() );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name( filter, "*.*" );
    gtk_file_filter_add_pattern( filter, "*" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );
}

static void WaitForCleanup(void)
{
    while (gtk_events_pending())
        gtk_main_iteration();
}

/// \todo String!
/// Dialog for opening file
bool		openFileDialog( std::string & path,  const char*  type, int maxsize  )
{
	static std::string currentOpenPath="none";

	bool result = false;
	char * outPath;

	GtkWidget *dialog;

    if ( !gtk_init_check( NULL, NULL ) )
    {
        return false;
    }

    dialog = gtk_file_chooser_dialog_new( "Open File",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Open", GTK_RESPONSE_ACCEPT,
                                          NULL );

    bool typeSpecified = true;
    if( strcmp(type,"*")==0 )
	{
		typeSpecified = false;
	}
	else
	{
    	AddFiltersToDialog(dialog, type);
    }

    /* Set the default path */
    if(currentOpenPath != "none")
	    SetDefaultPath(dialog, currentOpenPath.c_str());
	else
		SetDefaultPath(dialog, getCurrentDirectory().c_str());

    result = false;
    if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
    {
        char *filename;

        filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );

        path = filename;
        
        g_free( filename );

        currentOpenPath = getDirectoryFromPath(path);
       	
        result = true;
    }

    WaitForCleanup();
    gtk_widget_destroy(dialog);
    WaitForCleanup();

    return result;
}

/// Dialog for saving file
bool		saveFileDialog( std::string & path, const char* type, int maxsize  )
{
	static std::string currentSavePath="none";

	bool result = false;
	char * outPath;

	GtkWidget *dialog;

    if ( !gtk_init_check( NULL, NULL ) )
    {
        return false;
    }

    dialog = gtk_file_chooser_dialog_new( "Save File",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Save", GTK_RESPONSE_ACCEPT,
                                          NULL );

    bool typeSpecified = true;
    if( strcmp(type,"*")==0 )
	{
		typeSpecified = false;
	}
	else
	{
    	AddFiltersToDialog(dialog, type);
    }

    /* Set the default path */
    if( currentSavePath != "none" )
    	SetDefaultPath(dialog, currentSavePath.c_str());
    else
    	SetDefaultPath(dialog, getCurrentDirectory().c_str());

    result = false;
    if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
    {
        char *filename;

        filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );

        std::string res = filename;
        if(typeSpecified)
		{
			std::string suff = getFileExt(res);
			if( suff != type )
			{
				res = getPathWithoutExt(res) + "." + std::string(type);
			}
		}

		path = res;
       	
       	currentSavePath = getDirectoryFromPath(path);

        g_free( filename );

        result = true;
    }

    WaitForCleanup();
    gtk_widget_destroy(dialog);
    WaitForCleanup();

    return result;
}


std::string  getExecutablePath()
{
	char buffer[PATH_MAX];

	char* path_end;
  	/* Read the target of /proc/self/exe.  */
  	if (readlink ("/proc/self/exe", buffer, PATH_MAX) <= 0)
    	return "";
  	/* Find the last occurence of a forward slash, the path separator.  */
 	path_end = strrchr (buffer, '/');
  	if (path_end == NULL)
    	return "";
 	 /* Advance to the character past the last slash.  */
  	++path_end;
	/* Obtain the directory containing the program by truncating the
	   path after the last slash.  */
	*path_end = '\0';
	/* The length of the path is the number of characters up through the
	 last slash.  */
	return buffer;
}

}