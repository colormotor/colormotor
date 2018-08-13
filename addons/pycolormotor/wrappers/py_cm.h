namespace cm
{

%pythonappend ParamList::ParamList() %{
	self.thisown = False
%}

%pythonappend Param::Param() %{
	self.thisown = False
%}

enum PARAM_TYPE
{
	PARAM_UNKNOWN  = -1,
	PARAM_FLOAT = 0,
	PARAM_DOUBLE,
	PARAM_BOOL,
	PARAM_STRING,
	PARAM_CSTRING,
	PARAM_EVENT,
	PARAM_SELECTION,
	PARAM_INT,
	PARAM_VECTORf,
	PARAM_VECTORd,
	NUM_PARAM_TYPES
};


class Param 
{
public:
	Param();
	virtual ~Param();
	
	Param * clone( bool keepAddress = false );
	void clone( const Param & p );
	
	void setFloat( float val, int index = 0, bool bInformListeners=true );
	void setBool( bool val, bool bInformListeners=true );
	void setString( const std::string& val, bool bInformListeners=true );
	void setInt( int val, bool bInformListeners=true );
	
	bool isDirty() { bool o=dirty; dirty=false; return o; }
	
	float getFloat() const;
	bool  getBool() const;
	int	  getInt() const;
	const char* getString() const;
	
	// float param specific
	float getMin() const { return _min; }
	float getMax() const { return _max; }
	float getDefault() const { return _default; }
	
    
	PARAM_TYPE getType() const { return _type; }
	
	const char * getName() const { return _name.c_str(); }
	bool isName( const std::string& str ) const;
	
	static PARAM_TYPE  getTypeFromString( const char * str  );
	static const char * getTypeName( PARAM_TYPE type );
	
	const std::string & getOptions() const { return options; }
	Param* setOptions( const std::string & opt ) { options = opt; return this; }
	Param* appendOption( const std::string & opt ) { options += opt; return this; }
	bool hasOption( const std::string & opt );
	
	const std::string & getOptions() const { return options; }
	Param* setOptions( const std::string & opt ) { options = opt + ";"; return this; }
	Param* appendOption( const std::string & opt ) { options += opt + ";"; return this; }
	bool hasOption( const std::string & opt );
	bool hasOptions() const { return options.length() != 0; }

	Param* describe( const std::string& desc )
	{
		this->description = desc; return this;
	}


	const std::string & getPath() const { return _path; }
	void setPath( const std::string & path );
    
	Param* sameLine() { return appendOption("sameline"); }
    Param* noGui() { return appendOption("h"); }
};


class ParamList
{
public:
	ParamList( const std::string & name = "Params" );
	ParamList( const ParamList & mom );

	~ParamList();
	
	void release();	

	ParamList * clone(  bool keepAddresses = false  );
	
	int	getNumParams() const { return params.size(); }
	Param * getParam( int index ) const { if( index < (int)params.size() ) return params[index]; else return 0; }
	
	void addParams( const ParamList & list );
	void addParam( Param * param );
	
	std::string getPath() const;
	ParamList* getChild( const std::string & name );
	ParamList * getChild( int i ) { return children[i]; }
	ParamList * getParent() { return parent; }

	void newChild( const std::string & name );
	void setCurrent( const std::string & name );
	void resetCurrent();
	
	void addChild( ParamList * params );
	int getNumChildren() const { return children.size(); }

	void addSeparator();
	
	Param*  addFloat( const std::string& name, float value, float min = 0.0f, float max = 1.0f );
	Param*  addBool( const std::string& name, bool value );
	Param*  addInt( const std::string& name, int value );
	Param*  addString( const std::string& name, const std::string & value );

	bool setString( const std::string &name, const std::string & v, bool inform = true );
	bool setFloat( const std::string& name, float v, bool inform = true );
	bool setBool( const std::string& name, bool v, bool inform = true );
	bool setInt( const std::string& name, int v, bool inform = true );

	const std::string &getName() const { return name; }
	void setName( const std::string & name ) { this->name = name; }

	bool remove( Param * v );
	Param * find(const std::string& name );
	Param * findSafe(const std::string& name );
	
	void saveXml( const std::string & fname );
	/// load from xml file
	bool loadXml( const std::string & fname );
};

}