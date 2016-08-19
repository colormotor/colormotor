namespace cm
{
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

bool	fileExists( const std::string & fileName );

/// List files in folder @ path
std::vector <std::string> getFilesInFolder( const char * path);

/// Get current working dir
std::string getCurrentDirectory();

size_t getFileSize( FILE * f );

/// Untested
std::string  getExecutablePath();

std::string hexDump( unsigned char * buffer, int size, int valuesPerLine=10 );
std::string hexDump( unsigned short * buffer, int size, int valuesPerLine=10 );
std::string hexDump( unsigned int * buffer, int size, int valuesPerLine=10 );
/// Create a unsigned char array containing the data in path. 
std::string binarize( const std::string & path, const std::string & name );

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


}