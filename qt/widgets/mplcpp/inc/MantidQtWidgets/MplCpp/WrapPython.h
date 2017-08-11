#ifndef WRAPPYTHON_H
#define WRAPPYTHON_H
/*
 Copyright &copy; 2017 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
 National Laboratory & European Spallation Source

 This file is part of Mantid.

 Mantid is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 Mantid is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
*/

//
// This header wraps the Python.h header and allows it to be compiled in debug
// mode under MSVC without requiring a special debug build of the python
// interpreter/libraries. Define MTD_DEBUG_PYTHON to get the standard behaviour
//
// It was modified from boost::python to avoid the whole dependency.
// Modifications:
//   - BOOST_ ->MTD_
//   - Remove parts for <= v2.2 compatability. Minimum version = 2.5
//

#ifdef _DEBUG
#ifndef MTD_DEBUG_PYTHON
#ifdef _MSC_VER
// VC8.0 will complain if system headers are #included both with
// and without _DEBUG defined, so we have to #include all the
// system headers used by pyconfig.h right here.
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
#include <basetsd.h>
#include <io.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#endif
#undef _DEBUG // Don't let Python force the debug library just because we're
              // debugging.
#define DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
#endif
#endif

#include <pyconfig.h>
#if defined(_SGI_COMPILER_VERSION) && _SGI_COMPILER_VERSION >= 740
#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#undef HAVE_STDINT_H // undo Python 2.5.1 define
#endif

//
// Python's LongObject.h helpfully #defines ULONGLONG_MAX for us...
//
#include <limits.h>
#ifndef ULONG_MAX
#define MTD_PYTHON_ULONG_MAX_UNDEFINED
#endif
#ifndef LONGLONG_MAX
#define MTD_PYTHON_LONGLONG_MAX_UNDEFINED
#endif
#ifndef ULONGLONG_MAX
#define MTD_PYTHON_ULONGLONG_MAX_UNDEFINED
#endif

//
// Get ahold of Python's version number
//
#include <patchlevel.h>

#if PY_MAJOR_VERSION < 2 || PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 5
#error Python 2.5 or higher is required.
#endif

//
// Some things we need in order to get Python.h to work with compilers other
// than MSVC on Win32
//
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(__GNUC__) && defined(__CYGWIN__)

#define SIZEOF_LONG 4

#elif defined(__MWERKS__)

#ifndef _MSC_VER
#define PY_MSC_VER_DEFINED_FROM_WRAP_PYTHON_H 1
#define _MSC_VER 900
#endif

#undef hypot // undo the evil #define left by Python.

#elif defined(__BORLANDC__)
#undef HAVE_HYPOT
#define HAVE_HYPOT 1
#endif

#endif // _WIN32

//
// Main Python header
//
#include <Python.h>

#ifdef MTD_PYTHON_ULONG_MAX_UNDEFINED
#undef ULONG_MAX
#undef MTD_PYTHON_ULONG_MAX_UNDEFINED
#endif

#ifdef MTD_PYTHON_LONGLONG_MAX_UNDEFINED
#undef LONGLONG_MAX
#undef MTD_PYTHON_LONGLONG_MAX_UNDEFINED
#endif

#ifdef MTD_PYTHON_ULONGLONG_MAX_UNDEFINED
#undef ULONGLONG_MAX
#undef MTD_PYTHON_ULONGLONG_MAX_UNDEFINED
#endif

#ifdef PY_MSC_VER_DEFINED_FROM_WRAP_PYTHON_H
#undef _MSC_VER
#endif

#ifdef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
#undef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
#define _DEBUG
#ifdef _CRT_NOFORCE_MANIFEST_DEFINED_FROM_WRAP_PYTHON_H
#undef _CRT_NOFORCE_MANIFEST_DEFINED_FROM_WRAP_PYTHON_H
#undef _CRT_NOFORCE_MANIFEST
#endif
#endif

// Define Python 3 macros for Python 2.x
#if PY_VERSION_HEX < 0x02060000

#define Py_TYPE(o) (((PyObject *)(o))->ob_type)
#define Py_REFCNT(o) (((PyObject *)(o))->ob_refcnt)
#define Py_SIZE(o) (((PyVarObject *)(o))->ob_size)

#define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,

#endif

#ifdef __MWERKS__
#pragma warn_possunwant off
#elif _MSC_VER
#pragma warning(disable : 4786)
#endif

#if defined(HAVE_LONG_LONG)
#if defined(PY_LONG_LONG)
#define MTD_PYTHON_LONG_LONG PY_LONG_LONG
#elif defined(LONG_LONG)
#define MTD_PYTHON_LONG_LONG LONG_LONG
#else
#error "HAVE_LONG_LONG defined but not PY_LONG_LONG or LONG_LONG"
#endif
#endif

#endif // WRAPPYTHON_H
