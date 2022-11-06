#pragma once
#include<Windows.h>
#include<vector>

class ShellcodeItem {
public:
	UINT32 length;
	UINT8 *codes;

	ShellcodeItem(UINT32 length, UINT8 *codes) {
		this->codes = (UINT8 *)malloc(length);
		for (int i = 0; i < length; i++) {
			this->codes[i] = codes[i];
		}
	}

	~ShellcodeItem() {
		free(codes);
	}
};

class ShellcodeContainer {
public:
	UINT32 size;
	std::vector<ShellcodeItem *> codes;

	void push(ShellcodeItem *);

	ShellcodeItem *pop();

	~ShellcodeContainer();
};

class ShellcodeSplitor {
private:
	UINT8 *shellcode;
	UINT32 length;
public:
	ShellcodeSplitor(UINT8 *shellcode, UINT32 length) {
		this->shellcode = (UINT8 *)malloc(length);
		for (int i = 0; i < length; i++) {
			this->shellcode[i] = shellcode[i];
		}
	}

	~ShellcodeSplitor() {
		if (shellcode != NULL) {
			free(shellcode);
		}
	}

	ShellcodeContainer *split();
};