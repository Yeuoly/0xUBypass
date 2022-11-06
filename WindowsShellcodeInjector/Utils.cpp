#include"RSA.h"
#include"Utils.h"
#include<iostream>

/*
I Just do not understand... there is about 1/8 probability that encrypt would fault, but... why?? signed? 
I tried many ways, but.. ok, I choice a more efficiency way
*/
void GenerateKey(std::string &public_key, std::string &private_key) {
	bool flag = false;
	rsa::init();
	while (!flag) {
		flag = true;
		public_key = "";
		private_key = "";
		std::GeneratePair(private_key, public_key);
		uint8_t shellcode[] = {
			0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
			0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78
		};
		uint8_t encrypted_shellcode[NEEDED_ENCRYPT_LENGTH(sizeof(shellcode))] = { 0 };
		uint8_t decrypted_shellcode[sizeof(shellcode)] = { 0 };

		std::EncryptShell(shellcode, sizeof(shellcode), encrypted_shellcode, sizeof(encrypted_shellcode), public_key);
		std::DecryptShell(encrypted_shellcode, sizeof(encrypted_shellcode), decrypted_shellcode, sizeof(decrypted_shellcode), private_key);

		for (int i = 0; i < sizeof(shellcode); i++) {
			if (shellcode[i] != decrypted_shellcode[i]) {
				flag = false;
			}
		}
	}
}