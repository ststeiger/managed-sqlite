/******************************************************************************\
 * StringTranslator.h                                                         *
 * Special string functions from managed to unmanaged and vice-versa          *
\******************************************************************************/

/*
    ManagedSQLite
    Copyright (c) 2007 ManagedSQLite Team.  All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
        * Neither the name of the ManagedSQLite Team nor the
          names of its contributors may be used to endorse or promote products
          derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY MANAGEDSQLITE TEAM ``AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE MANAGEDSQLITE TEAM BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

// Windows API includes and definitions
#ifndef WINVER
#define WINVER 0x0501
#endif // WINVER

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif // _WIN32_WINNT

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

// C runtime library includes
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// STL String includes and definitions
#include <string>
#include <sstream>
#ifdef UNICODE
typedef std::wstring        string;
typedef std::wstringstream  stringstream;
#else
typedef std::string         string;
typedef std::stringstream   stringstream;
#endif

// Managed includes
using namespace System;
using namespace System::Runtime::InteropServices;

string M2UStr(String ^str);
String ^ U2MStr(const string &str);

#define MANAGED_TO_UNMANAGED_STRING M2UStr
#define UNMANAGED_TO_MANAGED_STRING U2MStr