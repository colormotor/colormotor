#pragma once
#include "cm.h"

#ifdef WIN32
	#include <direct.h>
	#define GETCWD	_getcwd
#else

	#ifdef CM_MAC
		#include <mach-o/dyld.h>
        #include <sys/event.h>
	#else
		#include <unistd.h>
	#endif

	#define GETCWD	getcwd
	#include <sys/types.h>
	#include <dirent.h>
#endif

namespace cm
{

/// Python-like check if key is in dictionary
template <class A, class B>
bool in( A key, const std::map<A,B>& dict )
{
    return dict.find(key) != dict.end();
}


// Time
double getTickCount();
    
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// File utilities
    
/// Prints on the same line, useful for progress tracing
void printSameLine( const char * format, ... );

//void debugPrint(const char *pFormat,... );

/// Makes sure all path separators are slashes   
std::string makePathNice(  const std::string & path );

/// Get the filemame only
std::string getFilenameFromPath(  const std::string & path );

/// Get directory from path
std::string getDirectoryFromPath(  const std::string & path );

///  Get path without the extension
std::string	getPathWithoutExt(  const std::string &  fileName );

/// Get file extension
std::string getFileExt(  const std::string & fileName );

/// Return wether a file exists
bool	fileExists( const std::string & fileName );

/// Joins paths in a and b
std::string joinPath( std::string a, std::string b );

/// List files in folder @ path
std::vector <std::string> getFilesInFolder( const std::string& path, const std::string& ext="");

/// Get current working dir
std::string getCurrentDirectory();

/// Get size of file
size_t getFileSize( FILE * f );

/// Untested
std::string  getExecutablePath();

/// Create hex dump string of a byte buffer
std::string hexDump( unsigned char * buffer, int size, int valuesPerLine=10 );
/// Create hex dump string of a word buffer
std::string hexDump( unsigned short * buffer, int size, int valuesPerLine=10 );
/// Create hex dump string of a dword buffer
std::string hexDump( unsigned int * buffer, int size, int valuesPerLine=10 );

/// Create a unsigned char array containing the data in path. 
std::string binarize( const std::string & path, const std::string & name );

/// Read line from file
bool readLine( std::string & s, FILE * file );

/// (Wrapper arounf strtok) Splits a string into tokens delimited by characters in delimiters.
std::vector<std::string> split( const std::string& str, const std::string& delimiters );

/// Returns a string between two delimiter characters a and b
std::string stringBetween( const std::string& str, char a, char b );

/// Extracts a string from a file
std::string stringFromFile( const std::string & path );

/// Shader string from file
std::string shaderString( const std::string& path );

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Dialogs
/// Dialog for opening a folder
bool openFolderDialog( std::string & str, const char *title );
/// Dialog for opening file
bool openFileDialog( std::string & path,  const char * type, int maxsize = 2048  );
/// Dialog for saving file
bool saveFileDialog( std::string & path, const char * type, int maxsize = 2048  );

///////////////////////////////////////////////////////
// File watcher (gross for the moment)
/// Class to watch modifications to a file
class FileWatcher
{
public:
	FileWatcher( const std::string & path, int msecs = 200 );

	~FileWatcher();
	
	bool hasFileChanged();
	void stop();
	void run();

	std::thread thread;
	bool running;
	std::mutex mutex;

	unsigned int fileFlags;
	int msecs;
	int numFiles;

#ifdef CM_OSX
	int kq;
    int event_fd;
    struct kevent events_to_monitor[1];
    struct kevent event_data[1];
    void *user_data;
    struct timespec timeout;
    unsigned int vnode_events;
#elif defined CM_LINUX
	// Linux
	int fd;
	std::vector<int> Wd;
#endif

};
}
