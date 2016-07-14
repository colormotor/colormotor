#include "cm_utils.h"
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <poll.h>

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
	return std::string(buffer); 
}

///////////////
// Gross filewatcher

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

FileWatcher::FileWatcher( const std::string & path, int msecs  )
:
fileFlags(0),
msecs(msecs)
{
  //http://man7.org/tlpi/code/online/diff/inotify/demo_inotify.c.html
     
     struct inotify_event *event;
 
     fd = inotify_init();                 /* Create inotify instance */
     if (fd == -1)
         assert(0);

  numFiles =  1;
  
  int wd = inotify_add_watch(fd, path.c_str(), IN_MODIFY ); //IN_ALL_EVENTS);
    Wd.push_back(wd);

    //paths.push_back(path);

    running = true;
    thread = std::thread(&FileWatcher::run, this);
}

FileWatcher::~FileWatcher()
{
    stop();
    for( int i = 0; i < Wd.size(); i++ )
       inotify_rm_watch( fd, Wd[i] );
   close(fd);
     printf("Closed file watcher\n");
}

bool FileWatcher::hasFileChanged()
{
  bool res = fileFlags != 0;
  fileFlags = 0;
  return res;
}

static void             /* Display information from inotify_event structure */
 displayInotifyEvent(FileWatcher * watcher, struct inotify_event *i)
 {
    /*
     printf("    wd =%2d; ", i->wd);
     if (i->cookie > 0)
         printf("cookie =%4d; ", i->cookie);
    
    
     printf("mask = ");
     if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
     if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
     if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
     if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
     if (i->mask & IN_CREATE)        printf("IN_CREATE ");
     if (i->mask & IN_DELETE)        printf("IN_DELETE ");
     if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
     if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
     if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
     if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
     if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
     if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
     if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
     if (i->mask & IN_OPEN)          printf("IN_OPEN ");
     if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
     if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
     printf("\n");
    
*/

    if( i->len < 1 )
        return;

    std::string name = i->name;
    if( getFileExt(name) == "py" && getFilenameFromPath(name) != "tmp.py" )
    {
        printf("Modified file: %s\n", i->name);
        watcher->fileFlags = 1;
    }

 }
 
void FileWatcher::stop()
{
    if(running)
    {
        mutex.lock();
        printf("Joining thread\n");
        running = false;
        thread.join();
        printf("Stopped file watching thread\n");
        mutex.unlock();
    }
}

void FileWatcher::run()
{
  char *p;
  struct inotify_event *event;
  char buf[EVENT_BUF_LEN] __attribute__ ((aligned(8)));
    printf("Starting file watcher\n");

  while( running )
  {
        //mutex.lock();
        struct pollfd pfd = { fd, POLLIN, 0 };
        int ret = poll(&pfd, 1, 50);  // timeout of 50ms
        if (ret < 0) {
            fprintf(stderr, "poll failed: %s\n", strerror(errno));
        } else if (ret == 0) {
            continue;
        }

    int numRead = read(fd, buf, EVENT_BUF_LEN);
        
        if( numRead == 0 )
    {
      continue;
    }

    for (p = buf; p < buf + numRead; ) {
             event = (struct inotify_event *) p;
             displayInotifyEvent(this, event);
         //fileFlags = 1;
             p += EVENT_SIZE + event->len;
         }
         //mutex.unlock();
         std::this_thread::sleep_for(std::chrono::milliseconds(10));

    //Thread::sleep(msecs);
  }
}


}