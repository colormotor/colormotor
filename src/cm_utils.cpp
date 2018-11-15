#include "cm_utils.h"
#ifdef CM_OSX
#include <CoreFoundation/CFDate.h>
#endif

namespace cm
{

void printSameLine( const char * format, ... )
{
	char txt[1000];
	
	if( strlen(format) > 1000 )
		return;
		
	for( int i = 0; i < 1000; i++ )
		putc('\b', stdout);

	va_list	parameter;
	va_start(parameter,format);
	vsnprintf(txt,1000,format,parameter);
	va_end(parameter);
	printf("%s", txt);
}

/*
/// Output formatted debug 
void  debugPrint(const char *pFormat,... )
{
	char debugText[4096];
	va_list	parameter;
	va_start(parameter,pFormat);
	vsnprintf(debugText,4096,pFormat,parameter);
	va_end(parameter);
	
	#ifdef WIN32
	OutputDebugStr(debugText);
	#else
	printf("%s",debugText);
	#endif
	
}
*/

///////////////////////////////////////////////////////////////////////
   
std::string makePathNice(  const std::string & path )
{
	std::string out = path;
	// theres probably a better way to do this.
	int sz = path.size();
	int ind = out.rfind('\\',sz);
	while( ind != -1 )
	{
		out[ind] = '/';
		sz = ind;
		ind = out.rfind('\\',sz);
	}
	
	return out;
}

///////////////////////////////////////////////////////////////////////

std::string getFilenameFromPath(  const std::string & path )
{	
	int ind = path.rfind('/',path.size());
	return path.substr( ind+1, path.size() );
}

///////////////////////////////////////////////////////////////////////

std::string getDirectoryFromPath(  const std::string & path )
{	
	int ind = path.rfind('/',path.size());
	return path.substr( 0, ind+1 );
}

///////////////////////////////////////////////////////////////////////

std::string	getPathWithoutExt(  const std::string &  fileName )
{
//	int ind = fileName.find('.',0);
	int ind = fileName.rfind('.',fileName.size());
	return fileName.substr( 0, ind );
}

///////////////////////////////////////////////////////////////////////

std::string getFileExt(  const std::string & fileName )
{
	int ind = fileName.rfind('.',fileName.size());
	return fileName.substr( ind+1, fileName.size() );
}

///////////////////////////////////////////////////////////////////////

bool	fileExists( const std::string & fileName )
{
	// hacky way to see if file exists
	FILE * f;
	f = fopen( fileName.c_str(), "rb" );
	bool res;
	if(f)
	{
		res = true;
		fclose(f);
	}
	else
		res = false;
	return res;
}

///////////////////////////////////////////////////////////////////////
/// Get current working dir
std::string getCurrentDirectory()
{
	std::string dir;// = "";
	char path[1024+1];
	GETCWD(path, 1024);//+path;
	dir = makePathNice(path);
	char c = dir[ dir.size() - 1 ];
	if( c != '/' )
		dir += "/";
	return dir;
}

///////////////////////////////////////////////////////////////////////

std::string joinPath( std::string a, std::string b )
{
	if( a[a.length()-1] != '/' )
		a = a+"/";
	if( b[0] == '/' )
		b = b.substr(1, std::string::npos);
	return a+b;
}


///////////////////////////////////////////////////////////////////////

std::vector <std::string> getFilesInFolder( const std::string& path, const std::string& ext)
{
	std::vector <std::string> files;
	struct dirent *de=NULL;
	DIR *d=NULL;

	std::string p = path;
	if(p[p.length()-1] != '/')
		p+="/";
	
	d=opendir(path.c_str());
	if(d == NULL)
	{
		debugPrint("Couldn't open directory");
		return files;
	}

	// Loop while not NULL
	while(de = readdir(d))
	{
		if(de->d_type==DT_REG)
		{
			std::string name = de->d_name;// de->d_type
			
			if (ext != "" && name.find(ext)==std::string::npos)
				continue;

			if(name!=".DS_Store")
			{
				files.push_back( p+name );
				debugPrint("%s\n",de->d_name);
			}

		}
	}
	
	closedir(d);
	return files;
}

///////////////////////////////////////////////////////////////////////

size_t getFileSize( FILE * f )
{
	//Get the file length:
	int fseekres = fseek(f,0, SEEK_END);   //fseek==0 if ok
	long filelen = ftell(f);
	fseekres = fseek(f,0, SEEK_SET);
	
	return filelen;
}


bool readLine( std::string & s, FILE * file )
{
	int i = 0;
	char c = fgetc(file) ;

	s = "";
	
    if(c==EOF || c=='\x01')
        return false;
	while( c != '\n' && c!= '\r' )
	{
		if( c == EOF )
		{
			if( i ) // if we read something end string
			{
				ungetc(1,file);
				s += '\0';
				return true;
			}
			
			return false;
		}
		
		s += c;
		c = fgetc(file);
		++i;
	}

	return true;
}

/// (Wrapper arounf strtok) Splits a string into tokens delimited by characters in delimiters.
std::vector<std::string> split( const std::string& str, const std::string& delimiters )
{
    char * buf = new char[str.length()+1]; // need to consider EOS
    memcpy(buf, str.c_str(), str.length()+1);
    
    std::vector<std::string> tokens;
    const char * tok = strtok(buf, delimiters.c_str());
    
    while(tok != NULL)
    {
        tokens.push_back(std::string(tok) + "");
        tok = strtok(NULL, delimiters.c_str());
    }
    
    delete [] buf;
    
    return tokens;
}

/// Returns a string between two delimiter characters a and b
std::string stringBetween( const std::string& str, char a, char b )
{
    size_t ia = str.find_first_of(a);
    if(ia==std::string::npos)
        return "";
    size_t ib = str.find_first_of(b);
    if(ib==std::string::npos)
        return "";
    size_t n = ib-ia;
    if(n < 2)
    {
        return "";
    }
    return str.substr(ia+1, n-1);
}

/// Extracts a string from a file
std::string stringFromFile( const std::string & path )
{
    std::ifstream f(path.c_str());
    if(f.fail())
    {
        //printf("Could not open file %s\n", path.c_str());
        return "";
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}


static void stdPrintf( std::string & str, const char *pFormat,... )
{
	static char txt[4096];
	
	if( strlen(pFormat) > 4096 )
		return;
	
	va_list	parameter;
	va_start(parameter,pFormat);
	vsnprintf(txt,4096,pFormat,parameter);
	va_end(parameter);
	str = txt;
}

std::string hexDump( unsigned char * buffer, int size, int valuesPerLine )
{
	size /= sizeof(unsigned char);
	std::string out = "";
	std::string tmp;
	for( int i = 0; i < size; i++ )
	{
		if(i&&!(i%valuesPerLine))
				out += "\n";
		stdPrintf(tmp,"0x%02X",buffer[i]);
		out+=tmp;
		if( i < size-1 )
			out += ", ";
	}
	
	return out;
}

std::string hexDump( unsigned short * buffer, int size, int valuesPerLine )
{
	size /= sizeof(unsigned short);
	std::string out = "";
	std::string tmp;
	for( int i = 0; i < size; i++ )
	{
		if(i&&!(i%valuesPerLine))
				out += "\n";
		stdPrintf(tmp,"0x%04X",buffer[i]);
		out+=tmp;
		if( i < size-1 )
			out += ", ";
	}
	
	return out;
}

std::string hexDump( unsigned int * buffer, int size, int valuesPerLine )
{
	size /= sizeof(unsigned int);
	std::string out = "";
	std::string tmp;
	for( int i = 0; i < size; i++ )
	{
		if(i&&!(i%valuesPerLine))
				out += "\n";
		stdPrintf(tmp,"0x%08X",buffer[i]);
		out+=tmp;
		if( i < size-1 )
			out += ", ";
	}
	
	return out;
}

std::string binarize( const std::string & path, const std::string & name )
{
    FILE * f = fopen(path.c_str(),"rb");
    if(!f)
        return "";
    
    size_t sz = getFileSize(f);
    std::vector<unsigned char> buf(sz);
    fread(&buf[0],1,sz,f);
    std::string bufs = hexDump(&buf[0], sz);
    
    std::stringstream sizes, defs;
    sizes << "size_t " << name <<  "_size=" << (int)sz << ";";
    defs << "\nunsigned char " << name << "[] = \n{" << bufs << "\n};";
    
    fclose(f);
    return sizes.str() + defs.str();
}


std::string shaderString( const std::string& path )
{
	std::string out="";
	FILE * f = fopen( path.c_str(), "r" );
	
	if( !f )
	{
		printf("Could not load %s\n", path.c_str());
		return out;
	}
	
	int size = getFileSize(f);
	
	char * fstr = new char[size+1];
	fread(fstr,1,size,f);
	fstr[size] = '\0';
	
	std::string str = fstr;
	delete [] fstr;
	fclose(f);
	
	// search for includes
	int  cur = 0;
	int ind = 0;
	
	while(true)
	{
		ind = str.find("#include",cur);
		if( ind == -1)
			break;
			
		// be sure line isnt commented... hack just checking for one /
		// TODO should have comment blocks too... search for  searchComment function...
		int newline = str.rfind('\n',ind); 
		int comm = str.rfind('/',ind);
		if( comm > newline )
		{
			continue;
		}
		
		// add stuff in the middle
		if( ind > cur )
			out+=str.substr(cur,ind-cur-1);
		
		int b1 = str.find('\"',ind);
		if( b1 == -1)
		{
			printf("Shader preprocessor error: expected \" after #include 'n");
			return "";
		}
		
		int b2 = str.find('\"',b1+1);
		if( b2 == -1)
		{
			printf("Shader preprocessor error: expected \" \n");
			return "";
		}
		
		cur = str.find('\n',b2);
		if( cur == -1 )
		{
			printf("Shader preprocessor error expected new line \n");
			return "";
		}
		
		std::string incPath = str.substr(b1+1,b2-b1-1);//,)
		
		std::string included = shaderString(incPath.c_str()) ;
		out+=included;
		
	}

	// append remaining stuff
	
	if( cur < (int)str.size() )
	{
		std::string sub =  str.substr(cur,str.size()-cur);
		
		out += sub;//str.substr(cur,str.size()-cur-1);
		//debugPrint("%s\n",out.c_str());
	}

	return out;
}

double getTickCount()
{
#ifdef CM_LINUX
    // from http://stackoverflow.com/questions/2958291/equivalent-to-gettickcount-on-linux
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#elif defined CM_OSX
    return CFAbsoluteTimeGetCurrent()*1000;
#else
    return (double) GetTickCount();
#endif
}


}