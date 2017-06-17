#pragma once
#pragma warning(disable:4251)
#pragma warning(disable:4996)

#ifdef _WIN32
#ifdef YCOMM_EXPORTS
#define YCOMMDLL_API __declspec(dllexport)
#else
#define YCOMMDLL_API __declspec(dllimport)
#endif
#else
#define YCOMMDLL_API
#endif

//#define _WIN32_WINDOWS
#ifdef __CYGWIN__
#define __USE_W32_SOCKETS
#endif

#define defaultBufferSize 65536

#include <iostream>

enum ProtocolType{TCP = 1, UDP = 2};
enum ServerRoleType{SENDER = 1, RECEIVER = 2};
enum CastType{UNICAST = 1, BROADCAST = 2, MULTICAST = 3};

// definements for unicode support
#ifdef _UNICODE
typedef std::wstring tstring;
typedef std::wostream tostream;
typedef std::wofstream tofstream;
typedef std::wostringstream tostringstream;
typedef std::wistringstream tistringstream;
typedef std::wstringstream tstringstream;
typedef std::wstreambuf tstreambuf;
typedef std::wifstream tifstream;
//typedef wchar_t TCHAR;

#define tcin  wcin
#define tcout wcout
#define tcerr wcerr
#define tclog wclog
#define _stprintf swprintf
#define _tcscpy wcscpy
#define _tcslen wcslen
#define _tcstol wcstol
#define _tprintf wprintf
#ifndef _WIN32
#define _ttoi wtoi
#define _itot itow
#else
#define _ttoi _wtoi
#define _itot _itow
#endif

#ifndef _T
#define _T(x) L ## x
#endif
#else
typedef std::string tstring;
typedef std::ostream tostream;
typedef std::ofstream tofstream;
typedef std::ostringstream tostringstream;
typedef std::istringstream tistringstream;
typedef std::stringstream tstringstream;
typedef std::streambuf tstreambuf;
typedef std::ifstream tifstream;
typedef char char_t;
//typedef char_t TCHAR;
#define tcin  cin
#define tcout cout
#define tcerr cerr
#define tclog clog
#define _stprintf sprintf
#define _tcscpy strcpy
#define _tcslen strlen
#define _tcstol strtol
#define _tprintf printf
#define _ttoi atoi
#define _itot itoa

#ifndef _T
#define _T
#endif

#endif

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
#ifdef WIN32
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif
