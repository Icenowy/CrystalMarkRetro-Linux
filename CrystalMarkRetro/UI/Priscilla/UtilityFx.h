/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

////------------------------------------------------
//   Debug
////------------------------------------------------

void SetDebugMode(DWORD mode);
void DebugPrint(CString cstr);

////------------------------------------------------
//   File Information
////------------------------------------------------

int GetFileVersion(const TCHAR* fileName, TCHAR* version = NULL);
void GetFileVersionEx(const TCHAR* file, CString& version);
BOOL IsFileExist(const TCHAR* fileName);

////------------------------------------------------
//   Utility
////------------------------------------------------

ULONGLONG GetTickCountFx();

ULONG64 B8toB64(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0, BYTE b4 = 0, BYTE b5 = 0, BYTE b6 = 0, BYTE b7 = 0);
DWORD B8toB32(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0);

////------------------------------------------------
//   .ini support function
////------------------------------------------------

DWORD GetPrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR  lpReturnedString,DWORD nSize,LPCTSTR lpFileName);
UINT GetPrivateProfileIntFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName);
BOOL WritePrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName);

////------------------------------------------------
//   Check CodeSign
////------------------------------------------------
#if _MSC_VER > 1310
BOOL CheckCodeSign(LPCWSTR certName, LPCWSTR filePath);
#endif
////------------------------------------------------
//   Play Sound
////------------------------------------------------

BOOL AlertSound(const CString& alertSoundPath, int volume);