// cl_tent.h
#ifndef CL_TENT_H
#define CL_TENT_H
#ifdef _WIN32
#pragma once
#endif

void CL_TempEntInit( void );

TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model );

#endif // CL_TENT_H