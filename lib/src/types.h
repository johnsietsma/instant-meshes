#pragma once

#define SINGLE_PRECISION

/* Application precision -- can be set to single or double precision */
#if defined(SINGLE_PRECISION)
typedef float Float;
#else
typedef double Float;
#endif