#pragma once
#include<Windows.h>

class WindowsShellCodeInvoke {
#define HEAVENS_GAET_ENTER 0x6A, 0x33, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x83, 0x04, 0x24, 0x05, 0xCB
#define HEAVENS_GATE_LEAVE 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x44, 0x24, 0x04, 0x23, 0x00, 0x00, 0x00, 0x83, 0x04, 0x24, 0x0D, 0xCB, 0x66, 0x8C, 0xD8, 0x8E, 0xD0

#define FUNCTION_ENTER 0x55, 0x89, 0xE5, 0x83, 0xe4, 0xf0
#define FUNCTION_LEAVE 0x89, 0xEC, 0x5D, 0xC3

private:
	typedef void (__cdecl * LPSYSCALL_FUNC)(
		UINT64 shellcode,
		UINT64 args1, UINT64 args2, UINT64 args3, UINT64 args4,
		UINT64 args_nums,
		UINT64 args_arr,
		UINT32 *ret
	);

	LPSYSCALL_FUNC p_syscall_func;

public:
	WindowsShellCodeInvoke() { }

	LPVOID VirtualAllocMemory(LPVOID base, UINT32 size, UINT32 flAllocationType, UINT32 flProtect);

	VOID InvokeShellcode(UINT8* bytes_sequence, UINT32 length);

protected:
	void p32(UINT32, UINT8* dst);

	void p64(UINT64, UINT8* dst);
};