#pragma once

//---------------------------------------------------------------------------//
//
// SystemTime.hpp
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

inline bool operator ==(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    else if ( st1.wYear   != st2.wYear )   { return false; }
    else if ( st1.wMonth  != st2.wMonth )  { return false; }
    else if ( st1.wDay    != st2.wDay )    { return false; }
    else if ( st1.wHour   != st2.wHour )   { return false; }
    else if ( st1.wMinute != st2.wMinute ) { return false; }
    else if ( st1.wSecond != st2.wSecond ) { return false; }
    else if ( st1.wMilliseconds != st2.wMilliseconds ) { return false; }
    else { return true; }
}

//---------------------------------------------------------------------------//

inline bool operator >(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    if      ( st1.wYear   > st2.wYear )   { return true; }
    else if ( st1.wYear   < st2.wYear )   { return false; }
    else if ( st1.wMonth  > st2.wMonth )  { return true; }
    else if ( st1.wMonth  < st2.wMonth )  { return false; }
    else if ( st1.wDay    > st2.wDay )    { return true; }
    else if ( st1.wDay    < st2.wDay )    { return false; }
    else if ( st1.wHour   > st2.wHour )   { return true; }
    else if ( st1.wHour   < st2.wHour )   { return false; }
    else if ( st1.wMinute > st2.wMinute ) { return true; }
    else if ( st1.wMinute < st2.wMinute ) { return false; }
    else if ( st1.wSecond > st2.wSecond ) { return true; }
    else if ( st1.wSecond < st2.wSecond ) { return false; }
    else if ( st1.wMilliseconds > st2.wMilliseconds ) { return true; }
    else { return false; }
}

//---------------------------------------------------------------------------//

inline bool operator >=(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    if      ( st1.wYear   > st2.wYear )   { return true; }
    else if ( st1.wYear   < st2.wYear )   { return false; }
    else if ( st1.wMonth  > st2.wMonth )  { return true; }
    else if ( st1.wMonth  < st2.wMonth )  { return false; }
    else if ( st1.wDay    > st2.wDay )    { return true; }
    else if ( st1.wDay    < st2.wDay )    { return false; }
    else if ( st1.wHour   > st2.wHour )   { return true; }
    else if ( st1.wHour   < st2.wHour )   { return false; }
    else if ( st1.wMinute > st2.wMinute ) { return true; }
    else if ( st1.wMinute < st2.wMinute ) { return false; }
    else if ( st1.wSecond > st2.wSecond ) { return true; }
    else if ( st1.wSecond < st2.wSecond ) { return false; }
    else if ( st1.wMilliseconds >= st2.wMilliseconds ) { return true; }
    else { return false; }
}

//---------------------------------------------------------------------------//

inline bool operator !=(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    return !( st1 === st2 );
}

//---------------------------------------------------------------------------//

inline bool operator <(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    return !( st1 >= st2 );
}

//---------------------------------------------------------------------------//

inline bool operator <=(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
    return !( st1 > st2 );
}

//---------------------------------------------------------------------------//

// SystemTime.hpp