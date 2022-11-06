#include<iostream>
#include<Windows.h>
#include"WindowsShellcodeInjector.h"

void WindowsShellCodeInvoke::p32(UINT32 src, UINT8 *dst) {
	for (int i = 0; i < 4; i++) {
		dst[i] = src & 0xff;
		src >>= 8;
	}
}

void WindowsShellCodeInvoke::p64(UINT64 src, UINT8 *dst) {
	for (int i = 0; i < 8; i++) {
		dst[i] = src & 0xff;
		src >>= 8;
	}
}

LPVOID WindowsShellCodeInvoke::VirtualAllocMemory(LPVOID base, UINT32 size, UINT32 flAllocationType, UINT32 flProtect) {
	UINT64 base_address = (UINT64)base;
	UINT8 p_base_address[4];
	p32((UINT32)&base_address, p_base_address);

	UINT64 region_size = size;
	UINT8 p_region_size[4];
	p32((UINT32)&region_size, p_region_size);

	UINT64 tmp = flAllocationType;
	UINT8 fl_allocation_type[8];
	p64(tmp, fl_allocation_type);

	UINT8 fl_protect[8];
	tmp = flProtect;
	p64(tmp, fl_protect);

#define FLAT_ARRAY_DWORD(arr, base) arr[0 + base], arr[1 + base], arr[2 + base], arr[3 + base]
#define FLAT_ARRAY_QWORD(arr, base) FLAT_ARRAY_DWORD(arr, base + 0), FLAT_ARRAY_DWORD(arr, base + 4)

	UINT8 bytes_sequence[] = {
		FUNCTION_ENTER,
		HEAVENS_GAET_ENTER,

		0x48, 0x31, 0xC9,										// xor rcx, rcx
		0x48, 0xFF, 0xC9,										// dec rcx				#ProcessHandle
		0x49, 0x89, 0xCA,										// mov r10, rcx
		0xBA, FLAT_ARRAY_DWORD(p_base_address, 0),				// mov edx,				#&base_address
		0x41, 0xB8, 0xFF, 0xFF, 0xFE, 0x7F,						// mov r8d, 0x7FFEFFFF	#ZeroBits
		0x41, 0xB9, FLAT_ARRAY_DWORD(p_region_size, 0),			// mov r9d,				#&RegionSize
		0x41, 0x57,												// push r15
		0x49, 0xBF, FLAT_ARRAY_QWORD(fl_protect, 0),			// movabs r15,			#flAllocationType
		0x41, 0x57,												// push r15
		0x49, 0xBF, FLAT_ARRAY_QWORD(fl_allocation_type, 0),	// movabs r15,			#flProtect
		0x41, 0x57,												// push r15
		0x41, 0x5F,												// pop r15
		0x41, 0x5F,												// pop r15
		0x41, 0x5F,												// pop r15

		0xB8, 0x18, 0x00, 0x00, 0x00,							// mov eax, 0x18

		0x48, 0x83, 0xEC, 0x40,		// sub rsp, 0x30
		0x0F, 0x05,	  				// syscall
		
		HEAVENS_GATE_LEAVE,
		FUNCTION_LEAVE,
	};

	InvokeShellcode(bytes_sequence, sizeof(bytes_sequence));

	return (LPVOID)base_address;
}

VOID WindowsShellCodeInvoke::InvokeShellcode(UINT8* bytes_sequence, UINT32 length) {
	UINT32 block_size = (length & 0xfffff000) | ((length | 0xfff) ? 1000 : 0);
	LPVOID bytes_mem = VirtualAlloc(NULL, block_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy_s(bytes_mem, block_size, bytes_sequence, length);
	typedef void(*ShellMem)();
	((ShellMem)bytes_mem)();
}