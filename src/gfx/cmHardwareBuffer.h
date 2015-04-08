
#pragma once

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmRenderTarget.h"

namespace cm
{


class HardwareBuffer : public GfxObject
{
public:
	enum USAGE
	{
		USAGE_DYNAMIC = 0x1,
		USAGE_WRITEONLY = 0x2,
		USAGE_LOCAL = 0x4,
		USAGE_DEFAULT = USAGE_DYNAMIC | USAGE_WRITEONLY
	};

	HardwareBuffer()
	{
		_size = 0;
		_stride = 0;
		_nextFreeIndex = 0;
	}


	virtual ~HardwareBuffer()
	{
		release();
	}

	virtual void release()
	{
		_size = 0;
		_stride = 0;
		_nextFreeIndex = 0;
	}

protected:
	/// creates vertex buffer
	/// \param size number of elements in buffer
	/// \param stride stride of one vertex
	/// \param usage buffer usage
	virtual bool	init(	int size, int stride ,USAGE usage = USAGE_DEFAULT	)
	{
		_size = size;
		_stride = stride;
		_usage = usage;
		return false;
	}

public:
	/// update buffer with data 
	/// \param data data to insert in buffer
	/// \param start start index ( default = 0 )
	/// \param count if 0 all buffer is updated...
	virtual bool	update( void * data, int count = 0,int start = 0  ) = 0;
	
	/// Stride of one element in buffer
	int			getStride() const { return _stride; }
	USAGE		getUsage() const { return _usage; }
	int			getSize() const { return _size; }

protected:
	void			setStride( int stride ) { _stride = stride; }
	void			setUsage( USAGE usage ) { _usage = usage; }
protected:
	USAGE			_usage;
	int				_stride;
	int				_size;

	/// next free index ( for dynamic updates )
	int				_nextFreeIndex; 
};

}
