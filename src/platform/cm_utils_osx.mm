#include "cm.h"
#include "cm_utils.h"

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import <Foundation/NSData.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <inttypes.h>

namespace cm
{

std::string convertCfString( CFStringRef str )
{
	char buffer[4096];
	Boolean worked = CFStringGetCString( str, buffer, 4095, kCFStringEncodingUTF8 );
	if( worked ) {
		std::string result( buffer );
		return result;
	}
	else
		return std::string();
}

CFStringRef	createCfString( const std::string &str )
{
	CFStringRef result = CFStringCreateWithCString( kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8 );
	return result;
}

std::string	convertNsString( NSString *str )
{
	return std::string( [str UTF8String] );
}


static char tmpStr[1024];


/// Error alert
void  alert(const char *pFormat,... )
{
	
	 static char alertText[1024];
	 va_list	parameter;
	 va_start(parameter,pFormat);
	 vsprintf(alertText,pFormat,parameter);
	 va_end(parameter);
	
	NSString *nss = [[NSString alloc] initWithUTF8String:alertText];
	
	NSAlert * a = [NSAlert alertWithMessageText:@"ALERT" //[NSString alloc] initWithUTF8String:alertText
	
    defaultButton:@"OK" alternateButton:nil

    otherButton:nil informativeTextWithFormat:

    nss ];
	
	[a runModal];
}



/// Yes no alert
bool  alertYesNo(const char *pFormat,... )
{
	 static char alertText[1024];
	 va_list	parameter;
	 va_start(parameter,pFormat);
	 vsprintf(alertText,pFormat,parameter);
	 va_end(parameter);
	
	NSString *nss = [[NSString alloc] initWithUTF8String:alertText];
	
	NSAlert * a = [NSAlert alertWithMessageText:@"COLORMOTOR:" //[NSString alloc] initWithUTF8String:alertText
	
    defaultButton:@"YES" alternateButton:@"NO"

    otherButton:nil informativeTextWithFormat:

    nss ];
	
	if ([a runModal] == NSAlertFirstButtonReturn)
	{
		return true;
	}
	
	return false;
}


/// Message box
void  messageBox(const char *pFormat,... )
{
	static char alertText[1024];
	 va_list	parameter;
	 va_start(parameter,pFormat);
	 vsprintf(alertText,pFormat,parameter);
	 va_end(parameter);
	
	NSString *nss = [[NSString alloc] initWithUTF8String:alertText];
	
	NSAlert * a = [NSAlert alertWithMessageText:@"Message" //[NSString alloc] initWithUTF8String:alertText
	
    defaultButton:@"OK" alternateButton:nil

    otherButton:nil informativeTextWithFormat:

    nss ];
	
	[a runModal];
}

/// Dialog for opening a folder
bool	openFolderDialog( std::string & str, const char *title )
{
	NSString *nsTitle = [[NSString alloc] initWithUTF8String:title];
	
	int res;

	@autoreleasepool    {

	NSOpenGLContext *foo = [NSOpenGLContext currentContext];
	NSOpenPanel* oPanel = [NSOpenPanel openPanel]; 
	[oPanel setCanChooseDirectories:YES]; 
	[oPanel setCanChooseFiles:NO]; 
	[oPanel setCanCreateDirectories:YES]; 
	[oPanel setAllowsMultipleSelection:NO]; 
	[oPanel setAlphaValue:0.95]; 
	[oPanel setTitle:nsTitle]; 
	
	res = [oPanel  runModalForDirectory:nil file:nil types:nil];
	[foo makeCurrentContext];
	

	if (res==NSOKButton) 
	{
		 NSString *filename = [oPanel filename];
		 const char *cString = [filename UTF8String];
		 str = cString;
		 return true;
	}

	}
	
	return false;
}



/// \todo String!
/// Dialog for opening file
bool		openFileDialog( std::string & path,  const char * type, int maxsize  )
{
	NSString *nsType = [[NSString alloc] initWithUTF8String:type];
	
	@autoreleasepool    {
	NSOpenGLContext *foo = [NSOpenGLContext currentContext];
	NSOpenPanel* oPanel = [NSOpenPanel openPanel]; 
	[oPanel setCanChooseDirectories:NO]; 
	[oPanel setCanChooseFiles:YES]; 
	[oPanel setCanCreateDirectories:YES]; 
	[oPanel setAllowsMultipleSelection:NO]; 
	[oPanel setAlphaValue:0.95]; 
	[oPanel setTitle:@"Open"]; 
	
	int res;
	if( strcmp(type,"*")==0 )
	{
		res = [oPanel  runModalForDirectory:nil file:nil types:nil];
	}
	else
	{
		NSArray *fileTypes = [NSArray arrayWithObjects:nsType,nil];//, @"gif",@"png", @"psd", @"tga", nil]; 
		res = [oPanel runModalForDirectory:nil file:nil types:fileTypes];
	}
		
	[foo makeCurrentContext];
	
	if (res==NSOKButton) 
	{
		 NSString *filename = [oPanel filename];
		 const char *cString = [filename UTF8String];
         std::string res([filename UTF8String]);
         path = res;//cString;
		 return true;
	}
	
	}
	return false;
}


/// Dialog for saving file
bool		saveFileDialog( std::string & path, const char * type, int maxsize  )
{
	path = "";
	NSString *nsType = [[NSString alloc] initWithUTF8String:type];
	
	NSSavePanel *spanel = [NSSavePanel savePanel];
	//[spanel setCanChooseDirectories:NO]; 
	//[spanel setCanChooseFiles:YES]; 
	[spanel setCanCreateDirectories:YES]; 
//	[spanel setAllowsMultipleSelection:NO]; 
	[spanel setAlphaValue:0.95]; 
	[spanel setTitle:@"Save As"]; 
	
	bool hasFileType = false;
	
	int res;
	if( strcmp(type,"*")==0 )
	{
		res = [spanel  runModal];
	}
	else
	{
		NSArray *fileTypes = [NSArray arrayWithObjects:nsType,nil];//, @"gif",@"png", @"psd", @"tga", nil]; 
		[spanel setAllowedFileTypes:fileTypes];
		res = [spanel runModal];
		hasFileType = true;
	}
	
	if (res==NSOKButton) 
	{
		NSString *filename = [spanel filename];
        std::string res([filename UTF8String]);
        
		if(hasFileType)
		{
			std::string suff = getFileExt(res);
			if( suff != type )
			{
				res+=".";
				res+=type;
			}
		}
        path = res;//cString;
		//strcpy(outname,cString);
		return true;
	}
	
	
	return false;
}

std::string getResourcePath( const char * identifier, const char * file ) 
{
	NSString *nsFile = [[NSString alloc] initWithUTF8String:file];
	NSString *nsId = [[NSString alloc] initWithUTF8String:identifier];
	NSString *nsResource = [[NSBundle bundleWithIdentifier:nsId] pathForResource:nsFile ofType:nil];
	
	std::string out = [nsResource UTF8String];
	return out;
}

std::string  getExecutablePath()
{
	NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
	const char* cstring = [bundlePath cStringUsingEncoding:NSASCIIStringEncoding];
	return std::string(cstring);
}

// Gross filewatcher
static const char *flagstring(int flags)
{
    static char ret[512];
    std::string ors = "";
	
    ret[0]='\0'; // clear the string.
    if (flags & NOTE_DELETE) {strcat(ret,ors.c_str());strcat(ret,"NOTE_DELETE");ors="|";}
    if (flags & NOTE_WRITE) {strcat(ret,ors.c_str());strcat(ret,"NOTE_WRITE");ors="|";}
    if (flags & NOTE_EXTEND) {strcat(ret,ors.c_str());strcat(ret,"NOTE_EXTEND");ors="|";}
    if (flags & NOTE_ATTRIB) {strcat(ret,ors.c_str());strcat(ret,"NOTE_ATTRIB");ors="|";}
    if (flags & NOTE_LINK) {strcat(ret,ors.c_str());strcat(ret,"NOTE_LINK");ors="|";}
    if (flags & NOTE_RENAME) {strcat(ret,ors.c_str());strcat(ret,"NOTE_RENAME");ors="|";}
    if (flags & NOTE_REVOKE) {strcat(ret,ors.c_str());strcat(ret,"NOTE_REVOKE");ors="|";}
	
    return ret;
}

enum
{
	NUM_EVENT_SLOTS = 1,
	NUM_EVENT_FDS  = 1
};

FileWatcher::FileWatcher( const std::string & path, int msecs  )
:
fileFlags(0),
msecs(msecs)
{
    /* Open a kernel queue. */
    if ((kq = kqueue()) < 0) {
        fprintf(stderr, "Could not open kernel queue.  Error was %s.\n", strerror(errno));
		return;
    }
	
    /*
	 Open a file descriptor for the file/directory that you
	 want to monitor.
     */
    event_fd = open(path.c_str(), O_EVTONLY);
    if (event_fd <=0) {
        fprintf(stderr, "The file %s could not be opened for monitoring.  Error was %s.\n", path.c_str(), strerror(errno));
        return;
    }
	
	/*
	 The address in user_data will be copied into a field in the
	 event.  If you are monitoring multiple files, you could,
	 for example, pass in different data structure for each file.
	 For this example, the path string is used.
     */
    user_data = (void*)path.c_str();
	
	/* Set the timeout to wake us every half second. */
    timeout.tv_sec = 0;        // 0 seconds
    timeout.tv_nsec = msecs * 1000000;//1000000;//500000000;    // 500 milliseconds
	
    /* Set up a list of events to monitor. */
    vnode_events = NOTE_DELETE |  NOTE_WRITE | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE; //NOTE_ATTRIB | NOTE_EXTEND
    EV_SET( &events_to_monitor[0], event_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, vnode_events, 0, user_data);
	
	numFiles =  1;
	
	running = true;
    thread = std::thread(&FileWatcher::run, this);
}

FileWatcher::~FileWatcher()
{
	//stopThread();
	stop();
	::close(event_fd);
}

bool FileWatcher::hasFileChanged()
{
	bool res = fileFlags != 0;
	fileFlags = 0;
	return res;
}

void FileWatcher::stop()
{
    if(running)
    {
        //mutex.lock();
        printf("Joining thread\n");
        running = false;
        thread.join();
        printf("Stopped file watching thread\n");
        //mutex.unlock();
    }
}


void FileWatcher::run()
{
	while( running )
	{
		int event_count = kevent(kq, events_to_monitor, NUM_EVENT_SLOTS, event_data, numFiles, &timeout);
		if ((event_count < 0) || (event_data[0].flags == EV_ERROR)) {
			/* An error occurred. */
			fprintf(stderr, "An error occurred (event count %d).  The error was %s.\n", event_count, strerror(errno));
		}
		
		if (event_count) 
		{
            
			printf("Event  occurred.  Filter %d, flags %d, filter flags %s\n",
				   event_data[0].filter,
				   event_data[0].flags,
				   flagstring(event_data[0].fflags));
			
			fileFlags = event_data[0].fflags;
		}
		else
		{
			fileFlags = 0;
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
	}
}



}