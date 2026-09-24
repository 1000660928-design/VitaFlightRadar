#ifndef VFR_MATH_WRAPPER_H
#define VFR_MATH_WRAPPER_H

/*
 * VitaFlightRadar build wrapper.
 * Keep the real VitaSDK math header, then expose Jansson declarations to
 * main.c for the OpenSky states parser added in v2.3.
 */
#include_next <math.h>
#include <jansson.h>

#endif
