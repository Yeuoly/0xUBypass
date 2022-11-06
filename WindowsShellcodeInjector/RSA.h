#pragma once
#include"mini-gmpxx.h"
#include<stdint.h>

#define BigUint mpz_class

#define ENCRYPT_KEY_SIZE (8 * 8)
#define ENCRYPT_BLOCK_SIZE (ENCRYPT_KEY_SIZE)
#define ENCRYPT_DST_SIZE (ENCRYPT_KEY_SIZE * 2)

#define CEIL(a, b) ((a % b) > 0 ? ((a / b) + 1) : (a / b))

#define PRINTARR(arr) for(int i = 0; i < sizeof(arr); i++) printf("%02x", arr[i])

namespace rsa {
	//Generate a prime greater than 2^bits
	BigUint GeneratePrime(int bits);

	//phi equals to (p - 1) * (q - 1);
	BigUint GeneratePhi(BigUint p, BigUint q);

	//Genertae a prime e
	BigUint GenerateE(BigUint phi);

	//ed = 1 (mod phi)
	BigUint GenerateD(BigUint e, BigUint phi);

	//Encrypt
	BigUint Encrypt(BigUint m, BigUint e, BigUint N);

	//Decrypt
	BigUint Decrypt(BigUint c, BigUint d, BigUint N);

	//Generate N equals to p * q;
	BigUint GenerateN(BigUint p, BigUint q);

	void init();
};

namespace math {
	//Generate a^-1 mod n
	BigUint Invert(BigUint a, BigUint n);

	//n ^ e mod m
	BigUint Powmod(BigUint n, BigUint e, BigUint m);

	BigUint Phi(BigUint e);

	//Greatest Common Dividor
	BigUint Gcd(BigUint a, BigUint b);

	//Lessest Common 
	BigUint Lcm(BigUint a, BigUint b);

	//Test if a number is prime
	bool IsPrime(BigUint n, int k);

	//Generate a random number between min and max
	BigUint RandomNumber(BigUint min, BigUint max);
};

namespace std {
	constexpr static char PrintableConvertTable[] = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'm', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'/', '\\', '?', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '+', '=', '[', ']', '|', ';', '<', '>', '.', ',', '~', '`',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	constexpr static char PrintableInvertTable[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 
		57, 58, 59, 61, 0, 63, 64, 62, 66, 75, 65, 74, 52, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 0, 71, 72, 67, 
		73, 54, 56, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
		50, 51, 68, 53, 69, 60, 0, 77, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 0, 14, 15, 16, 17, 18, 19, 20, 21, 
		22, 23, 24, 25, 0, 70, 0, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	constexpr static int PrintableSize = sizeof(PrintableConvertTable);

	void Printable2Number(string src, BigUint &ed, BigUint &N);

	void Number2Printable(string &dst, BigUint ed, BigUint N);

	void GeneratePair(string &private_key, string &public_key);

	void EncryptShell(uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen, string public_key);

	void DecryptShell(uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen, string private_key);

#define NEEDED_ENCRYPT_LENGTH(SRC_LEN) ((CEIL(SRC_LEN, (ENCRYPT_BLOCK_SIZE / 8)) * (ENCRYPT_DST_SIZE / 8)))
};