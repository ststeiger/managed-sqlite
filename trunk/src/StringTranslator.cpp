/******************************************************************************\
 * StringTranslator.cpp                                                       *
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
*/

#include "StringTranslator.h"

string M2UStr(String ^str)
{
    // Conversion
    LPTSTR nativeString = (LPTSTR)Marshal::StringToHGlobalAuto(str).ToPointer();

    // Storage
    string stlString = nativeString;

    // Clean up
    IntPtr ptr( (void*)nativeString );
    Marshal::FreeHGlobal(ptr);

    // Result
    return stlString;
}
String ^ U2MStr(const string &str)
{
    IntPtr ptr( (void*)str.c_str() );
    return Marshal::PtrToStringAuto(ptr);
}
