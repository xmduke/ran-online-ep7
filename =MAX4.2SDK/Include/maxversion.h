#pragma once
#include "buildnumber.h" // defines VERSION_INT

// VERSION_INT is defined in buildnumber.h and is written to by the builder, inserting the current build number.
// VERSION_INT is used for the fourth least siginifact version component eg. 4.1.1.[36]

#ifndef MAX_VERSION_MAJOR
#define MAX_VERSION_MAJOR 4
#endif

#ifndef MAX_VERSION_MINOR
#define MAX_VERSION_MINOR 2
#endif

#ifndef MAX_VERSION_POINT
#define MAX_VERSION_POINT 0
#endif

#define _MAX_VERSION(a, b, c,d) a##b##c##d
#define MAX_VERSION _MAX_VERSION(MAX_VERSION_MAJOR, MAX_VERSION_MINOR, MAX_VERSION_POINT,VERSION_INT)
