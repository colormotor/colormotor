

#pragma once


#define CM_SINGLETON(classname) \
\
    static classname* _instance;  \
\
    static classname*  getInstance() \
    { \
        if (_instance == 0) \
        {\
\
            if (_instance == 0) \
            { \
				classname* newObject = new classname();  \
\
				_instance = newObject; \
            } \
        } \
\
        return _instance; \
    } \
\
    static inline classname* getInstanceWithoutCreating()  \
    { \
        return _instance; \
    } \
\
    static void  deleteInstance() \
    { \
        if (_instance != 0) \
        { \
            classname* old = _instance; \
            _instance = 0; \
            delete old; \
        } \
    } \
\
    void clearSingletonInstance() \
    { \
        if (_instance == this) \
            _instance = 0; \
    } 
	
	
///////////////////////////////////////////////

#define CM_SINGLETON_IMPL(classname) \
\
    classname* classname::_instance = 0;
	
	
			


///////////////////////////////////////////////

					
#define CM_STATIC_SINGLETON(classname) \
\
static classname*  getInstance() \
{ \
	static classname instance; \
	return &instance; \
} 







