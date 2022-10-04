// FunctionStackTrace.h  Version 1.1
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTIONSTACKTRACE_H
#define FUNCTIONSTACKTRACE_H

#include <malloc.h>
#define FUNCTION_STACK_TRACE(x) \
	char * function_stack_trace_buffer = (char *) _alloca(8); \
	CopyMemory(function_stack_trace_buffer, x, 8);

#endif //FUNCTIONSTACKTRACE_H
