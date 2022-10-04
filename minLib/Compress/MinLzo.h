#pragma once

#include "lzoconf.h"
#include "lzo1x.h"

/**
 * \ingroup minLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-09-08
 *
 * \author jgkim
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \todo 
 *
 * \bug 
 *
 */
namespace minLib
{

class minLzo
{
	enum
	{
		lzo_success       =  0,
		lzo_error         = -1,
		input_data_error  = -2,
		internal_error    = -3,
		can_not_compress  = -4,
	};

public:
	minLzo(void);
	~minLzo(void);
	int init();
	int lzoCompress(unsigned char* pInBuffer, 
					int nInLength, 
					unsigned char* pOutBuffer, 
					int &nOutLength);

	int lzoDeCompress(unsigned char* pOutBuffer, 
					int nOutLength, 
					unsigned char* pInBuffer, 
					int &nNewLength);

protected:
	lzo_bytep pWorkmem;
};

}