#ifndef __XUNZIP_H__
#define __XUNZIP_H__

#pragma once
// XUnzip.cpp  Version 1.3
//
// Authors:      Mark Adler et al. (see below)
//
// Modified by:  Lucian Wischik
//               lu@wischik.com
//
// Version 1.0   - Turned C files into just a single CPP file
//               - Made them compile cleanly as C++ files
//               - Gave them simpler APIs
//               - Added the ability to zip/unzip directly in memory without 
//                 any intermediate files
// 
// Modified by:  Hans Dietrich
//               hdietrich@gmail.com
//
// Version 1.3:  - Corrected size bug introduced by 1.2
//
// Version 1.2:  - Many bug fixes.  See CodeProject article for list.
//
// Version 1.1:  - Added Unicode support to CreateZip() and ZipAdd()
//               - Changed file names to avoid conflicts with Lucian's files
//
///////////////////////////////////////////////////////////////////////////////
//
// Lucian Wischik's comments:
// --------------------------
// THIS FILE is almost entirely based upon code by Info-ZIP.
// It has been modified by Lucian Wischik.
// The original code may be found at http://www.info-zip.org
// The original copyright text follows.
//
///////////////////////////////////////////////////////////////////////////////
//
// Original authors' comments:
// ---------------------------
// This is version 2002-Feb-16 of the Info-ZIP copyright and license. The 
// definitive version of this document should be available at 
// ftp://ftp.info-zip.org/pub/infozip/license.html indefinitely.
// 
// Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.
//
// For the purposes of this copyright and license, "Info-ZIP" is defined as
// the following set of individuals:
//
//   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,
//   Jean-loup Gailly, Hunter Goatley, Ian Gorman, Chris Herborth, Dirk Haase,
//   Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz, 
//   David Kirschbaum, Johnny Lee, Onno van der Linden, Igor Mandrichenko, 
//   Steve P. Miller, Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs, 
//   Kai Uwe Rommel, Steve Salisbury, Dave Smith, Christian Spieler, 
//   Antoine Verheijen, Paul von Behren, Rich Wales, Mike White
//
// This software is provided "as is", without warranty of any kind, express
// or implied.  In no event shall Info-ZIP or its contributors be held liable
// for any direct, indirect, incidental, special or consequential damages
// arising out of the use of or inability to use this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//    1. Redistributions of source code must retain the above copyright notice,
//       definition, disclaimer, and this list of conditions.
//
//    2. Redistributions in binary form (compiled executables) must reproduce 
//       the above copyright notice, definition, disclaimer, and this list of 
//       conditions in documentation and/or other materials provided with the 
//       distribution. The sole exception to this condition is redistribution 
//       of a standard UnZipSFX binary as part of a self-extracting archive; 
//       that is permitted without inclusion of this license, as long as the 
//       normal UnZipSFX banner has not been removed from the binary or disabled.
//
//    3. Altered versions--including, but not limited to, ports to new 
//       operating systems, existing ports with new graphical interfaces, and 
//       dynamic, shared, or static library versions--must be plainly marked 
//       as such and must not be misrepresented as being the original source.  
//       Such altered versions also must not be misrepresented as being 
//       Info-ZIP releases--including, but not limited to, labeling of the 
//       altered versions with the names "Info-ZIP" (or any variation thereof, 
//       including, but not limited to, different capitalizations), 
//       "Pocket UnZip", "WiZ" or "MacZip" without the explicit permission of 
//       Info-ZIP.  Such altered versions are further prohibited from 
//       misrepresentative use of the Zip-Bugs or Info-ZIP e-mail addresses or 
//       of the Info-ZIP URL(s).
//
//    4. Info-ZIP retains the right to use the names "Info-ZIP", "Zip", "UnZip",
//       "UnZipSFX", "WiZ", "Pocket UnZip", "Pocket Zip", and "MacZip" for its 
//       own source and binary releases.
//
///////////////////////////////////////////////////////////////////////////////


//#define _USE_32BIT_TIME_T	//+++1.2


#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#pragma warning(disable : 4996)	// disable bogus deprecation warning

// THIS FILE is almost entirely based upon code by Jean-loup Gailly
// and Mark Adler. It has been modified by Lucian Wischik.
// The original code may be found at http://www.gzip.org/zlib/
// The original copyright text follows.
//
//
//
// zlib.h -- interface of the 'zlib' general purpose compression library
//  version 1.1.3, July 9th, 1998
//
//  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//  Jean-loup Gailly        Mark Adler
//  jloup@gzip.org          madler@alumni.caltech.edu
//
//
//  The data format used by the zlib library is described by RFCs (Request for
//  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
//  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
//
//
//     The 'zlib' compression library provides in-memory compression and
//  decompression functions, including integrity checks of the uncompressed
//  data.  This version of the library supports only one compression method
//  (deflation) but other algorithms will be added later and will have the same
//  stream interface.
//
//     Compression can be done in a single step if the buffers are large
//  enough (for example if an input file is mmap'ed), or can be done by
//  repeated calls of the compression function.  In the latter case, the
//  application must provide more input and/or consume the output
//  (providing more output space) before each call.
//
//     The library also supports reading and writing files in gzip (.gz) format
//  with an interface similar to that of stdio.
//
//     The library does not install any signal handler. The decoder checks
//  the consistency of the compressed data, so the library should never
//  crash even in case of corrupted input.
//
// for more info about .ZIP format, see ftp://ftp.cdrom.com/pub/infozip/doc/appnote-970311-iz.zip
//   PkWare has also a specification at ftp://ftp.pkware.com/probdesc.zip

#define zmalloc(len) malloc(len)

#define zfree(p) free(p)

/*
void *zmalloc(unsigned int len)
{ char *buf = new char[len+32];
  for (int i=0; i<16; i++)
  { buf[i]=i;
	buf[len+31-i]=i;
  }
  *((unsigned int*)buf) = len;
  char c[1000]; wsprintf(c,"malloc 0x%lx  - %lu",buf+16,len);
  OutputDebugString(c);
  return buf+16;
}

void zfree(void *buf)
{ char c[1000]; wsprintf(c,"free   0x%lx",buf);
  OutputDebugString(c);
  char *p = ((char*)buf)-16;
  unsigned int len = *((unsigned int*)p);
  bool blown=false;
  for (int i=0; i<16; i++)
  { char lo = p[i];
	char hi = p[len+31-i];
	if (hi!=i || (lo!=i && i>4)) blown=true;
  }
  if (blown)
  { OutputDebugString("BLOWN!!!");
  }
  delete[] p;
}
*/

DECLARE_HANDLE(HZIPDT);	// An HZIPDT identifies a zip file that has been opened

typedef DWORD ZRESULT;
// return codes from any of the zip functions. Listed later.

#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3

typedef struct
{
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYA;

typedef struct
{
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;

#ifdef _UNICODE
typedef ZIPENTRYW ZIPENTRYDT;
#else
typedef ZIPENTRYA ZIPENTRYDT;
#endif

// These are the result codes:
#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
// The following come from general system stuff (e.g. files not openable)
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
// The following come from mistakes on the part of the caller
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
#define ZR_PASSWORD   0x00001000     // we didn't get the right password to unzip the file
// The following come from bugs within the zip library itself
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

#pragma warning(disable : 4702)   // unreachable code

static ZRESULT zopenerror = ZR_OK; //+++1.2

#endif