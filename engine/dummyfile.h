/**
 * Copyright - xWhitey, 2024.
 * dummyfile.h - description
 *
 * Global project (This file can be used anywhere) header file
 * Authors: xWhitey.
 * Do not delete this comment block. Respect others' work!
 */

#ifdef DUMMYFILE_H_RECURSE_GUARD
#error Recursive header files inclusion detected in dummyfile.h
#else //DUMMYFILE_H_RECURSE_GUARD

#define DUMMYFILE_H_RECURSE_GUARD

#ifndef DUMMYFILE_H_GUARD
#define DUMMYFILE_H_GUARD
#pragma once

//Fix of a weird bug which appears only in old MSVC toolchain (mostly MSVS 2017 Community builds have this bug)
#if _MSC_VER < 1920
#ifdef no_init_all
#undef no_init_all
#endif //no_init_all

#define no_init_all deprecated

#endif //MSC_VER < 1920

#endif //DUMMYFILE_H_GUARD

#undef DUMMYFILE_H_RECURSE_GUARD
#endif //DUMMYFILE_H_RECURSE_GUARD