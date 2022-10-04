#include "StdAfx.h"
#include ".\minlzo.h"

/* portability layer */
#define WANT_LZO_MALLOC 1
#include "portab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace minLib;

minLzo::minLzo(void)
	: pWorkmem(NULL)
{	
}

minLzo::~minLzo(void)
{
	SAFE_DELETE(pWorkmem);	
}

int minLzo::init()
{
	if (lzo_init() != LZO_E_OK)
	{
		printf("internal error - lzo_init() failed !!!\n");
		printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable `-DLZO_DEBUG' for diagnostics)\n");
		return -1;
	}
	else
	{
		// Allocate blocks and the work-memory
		// pIn = (lzo_bytep) lzo_malloc(IN_LEN);
		// pOut = (lzo_bytep) lzo_malloc(OUT_LEN);
		pWorkmem = (lzo_bytep) lzo_malloc(LZO1X_1_MEM_COMPRESS);

		// if (pIn == NULL || pOut == NULL || pWorkmem == NULL)
		if (pWorkmem == NULL)
		{
			printf("out of memory\n");
			return lzo_error;
		}
		else
		{
			return lzo_success;
		}
	}
}

int minLzo::lzoCompress(unsigned char* pInBuffer, 
						 int nInLength, 
						 unsigned char* pOutBuffer, 
						 int &nOutLength)
{
	if (pInBuffer == NULL ||
		nInLength == 0 ||
		pOutBuffer == NULL)
	{
		return input_data_error;
	}

	int nReturn = 0;

	/**
	* compress from `in' to `out' with LZO1X-1
	*/	
	nReturn = lzo1x_1_compress(pInBuffer,
						nInLength,
						pOutBuffer,
						(lzo_uint*) &nOutLength,
						pWorkmem);
					
	if (nReturn == LZO_E_OK)
	{
		if (nOutLength >= nInLength)
		{
			return can_not_compress;
		}
		else
		{
			return lzo_success;
		}
	}
	else
	{		
		return internal_error;
	}	
}

int minLzo::lzoDeCompress(unsigned char* pOutBuffer, 
						   int nOutLength, 
						   unsigned char* pInBuffer, 
						   int &nNewLength)
{
	if (pOutBuffer == NULL ||
		nOutLength == 0 ||
		pInBuffer == NULL)
	{
		return input_data_error;
	}

	int nReturn = 0;
	/**
	* decompress, now going from `out' to `in'
	*/		
	nReturn = lzo1x_decompress(pOutBuffer, 
							nOutLength,
							pInBuffer,
							(lzo_uint*) &nNewLength,
							NULL);

	if (nReturn == LZO_E_OK && nNewLength == nOutLength)
	{
		return lzo_success;
	}
	else
	{
        return lzo_error;
	}    
}