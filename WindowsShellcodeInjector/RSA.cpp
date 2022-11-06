#include"RSA.h"
#include<iostream>
#include<time.h>
#include<stack>

clock_t start, finish;
#define TESTTIME(code, module) start = clock(); \
	code \
	finish = clock(); \
	std::cout << "module:" << module " takes " << finish - start << std::endl;

BigUint math::Gcd(BigUint a, BigUint b) {
	if (a == 0) {
		return b;
	}
	if (a > b) {
		return Gcd(b, a);
	}
	return Gcd(b % a, a);
}

void rsa::init() {
	srand(time(NULL));
}

int phi(int n)
{
	float result = n;
	for (int p = 2; p * p <= n; ++p)
	{
		if (n % p == 0)
		{
			while (n % p == 0)
				n /= p;
			result *= (1.0 - (1.0 / (float)p));
		}
	}

	if (n > 1)
		result *= (1.0 - (1.0 / (float)n));

	return (int)result;
}

BigUint math::Phi(BigUint e) {
	BigUint result = e;

	for (BigUint p = 2; p * p <= e; p++) {
		if (e % p == 0) {
			while (e % p == 0) {
				e /= p;
			}
			result = result - result / p;
		}
	}

	if (e > 1) {
		result = result - result / e;
	}

	return result;
}

BigUint math::Powmod(BigUint n, BigUint e, BigUint m) {
	if (e == 1) return n % m;
	if (e == 0) return 1;

	if ((e & 1) == 1) {
		// y = n ^ e mod m
		// y = ((n mod m) * ((n ^ (e-1) mod m))) mod m
		return ((n % m) * (Powmod(n, e - 1, m))) % m;
	}
	BigUint tmp = Powmod(n, e / 2, m);
	return (tmp * tmp) % m;
}

BigUint math::Invert(BigUint a, BigUint n) {
	BigUint result;
	mpz_invert(result.get_mpz_t(), a.get_mpz_t(), n.get_mpz_t());
	return result;
}

BigUint math::Lcm(BigUint a, BigUint b) {
	return a * b / Gcd(a, b);
}

BigUint repeatMod(BigUint x, BigUint a) {
	BigUint b = x - 1;
	BigUint ans = 1;
	int times = 0;
	while (b > 0) {
		times++;
		if ((b & 1) == 1) {
			ans = ((ans % x) * (a % x)) % x;
		}
		a = a * a % x;
		b >>= 1;
	}
	return ans;
}

bool math::IsPrime(BigUint n, int k) {
	//Miller-Rabin
	while (k--) {
		BigUint t = RandomNumber(2, n - 2) + 2;
		while (t % n == 0) {
			t = RandomNumber(2, n - 2) + 2;
		}
		if (repeatMod(n, t) != 1) {
			return false;
		}
	}
	return true;
}

BigUint math::RandomNumber(BigUint min, BigUint max) {
	const int random_max = 2 << 14;
	BigUint distance = max - min;
	if (distance == 0) {
		return 0;
	}
	if (distance < random_max) {
		return rand() % distance;
	}

	BigUint result = 0;
	while (distance >= random_max) {
		result = result * random_max + rand();
		distance /= random_max;
	}

	result += min;

	while (result > max) {
		result >>= 1;
	}

	return result;
}


BigUint rsa::GeneratePrime(int bits) {
	if (bits == 0) return 0;
	if (bits == 1) return 1;

	BigUint smallest = BigUint(1) << (bits - 1);
	BigUint biggest = (BigUint(1) << bits) - 1;
	BigUint result = math::RandomNumber(smallest, biggest);
	while (!math::IsPrime(result, 40)) {
		//get a rand number greater than smallest
		result = math::RandomNumber(smallest, biggest);
	}
	return result;
}

BigUint rsa::GenerateN(BigUint p, BigUint q) {
	return p * q;
}

BigUint rsa::GeneratePhi(BigUint p, BigUint q) {
	return (p - 1) * (q - 1);
}

BigUint rsa::GenerateE(BigUint phi) {
	BigUint e;
	mpz_sqrt(e.get_mpz_t(), phi.get_mpz_t());
	mpz_sqrt(e.get_mpz_t(), e.get_mpz_t());
	for (; e < phi; e++) {
		if (math::Gcd(e, phi) == 1) {
			return e;
		}
	}

	return 7;
}

BigUint rsa::GenerateD(BigUint e, BigUint phi) {
	return math::Invert(e, phi);
}

BigUint rsa::Encrypt(BigUint m, BigUint e, BigUint N) {
	BigUint result = 0;
	mpz_powm(result.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), N.get_mpz_t());
	return result;
}

BigUint rsa::Decrypt(BigUint c, BigUint d, BigUint N) {
	BigUint result = 0;
	mpz_powm(result.get_mpz_t(), c.get_mpz_t(), d.get_mpz_t(), N.get_mpz_t());
	return result;
}

void std::Printable2Number(string src, BigUint &ed, BigUint &N) {
	size_t idx = src.find(':');
	if (idx == std::string::npos) {
		return;
	}
	size_t srclen = src.length();
	ed = 0, N = 0;
	for (size_t i = 0; i < idx; i++) {
		ed = ed * PrintableSize + PrintableInvertTable[src[i]];
	}
	for (size_t i = idx + 1; i < srclen; i++) {
		N = N * PrintableSize + PrintableInvertTable[src[i]];
	}
}

void std::Number2Printable(string &dst, BigUint ed, BigUint N) {
	stack<uint8_t> num;
	while (ed > 0) {
		BigUint remainder = ed % PrintableSize;
		uint8_t unit = (uint8_t)remainder.get_ui();
		ed /= PrintableSize;
		num.push(PrintableConvertTable[unit]);
	}
	while (!num.empty()) {
		dst += num.top();
		num.pop();
	}

	dst.append(":");

	while (N > 0) {
		BigUint remainder = N % PrintableSize;
		uint8_t unit = (uint8_t)remainder.get_ui();
		N /= PrintableSize;
		num.push(PrintableConvertTable[unit]);
	}

	while (!num.empty()) {
		dst += num.top();
		num.pop();
	}
}

void std::GeneratePair(string &private_key, string &public_key) {
	BigUint p = rsa::GeneratePrime(ENCRYPT_KEY_SIZE);
	BigUint q = rsa::GeneratePrime(ENCRYPT_KEY_SIZE);
	BigUint N = rsa::GenerateN(p, q);
	BigUint phi = rsa::GeneratePhi(p, q);
	BigUint e = rsa::GenerateE(phi);
	BigUint d = rsa::GenerateD(e, phi);

	Number2Printable(public_key, e, N);
	Number2Printable(private_key, d, N);
}

void P(BigUint src, size_t bits, uint8_t *dst) {
	for (int i = 0; i < bits / 8; i++) {
		BigUint unit = src & 0xff;
		dst[i] = (uint8_t)unit.get_ui();
		src >>= 8;
	}
}

void U(uint8_t *src, size_t bits, BigUint &dst) {
	dst = 0;
	for (int i = (bits / 8) - 1; i >= 0; i--) {
		dst = dst * 0x100 + src[i];
	}
}

void std::EncryptShell(uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen, string public_key) {
	BigUint e = 0, N = 0;
	Printable2Number(public_key, e, N);

	//convert char[] to BigUint[]
	BigUint tmp_size = CEIL(srclen, (ENCRYPT_BLOCK_SIZE / 8));
	BigUint *tmp = new BigUint[tmp_size.get_ui()];

	for (BigUint i = 0; i < tmp_size; i++) {
		tmp[i.get_ui()] = 0;
		U(src + (ENCRYPT_BLOCK_SIZE / 8) * i.get_ui(), ENCRYPT_BLOCK_SIZE, tmp[i.get_ui()]);
	}

	//encrypt
	for (BigUint i = 0; i < tmp_size; i++) {
		tmp[i.get_ui()] = rsa::Encrypt(tmp[i.get_ui()], e, N);
	}

	//convert BigUint[] to char[]
	for (BigUint i = 0; i < tmp_size; i++) {
		P(tmp[i.get_ui()], ENCRYPT_DST_SIZE, dst + (ENCRYPT_DST_SIZE / 8) * i.get_ui());
	}

	delete[tmp_size.get_ui()]tmp;
}

void std::DecryptShell(uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen, string private_key) {
	BigUint d = 0, N = 0;
	Printable2Number(private_key, d, N);

	//convert char[] to BigUint[]
	BigUint tmp_size = CEIL(srclen, (ENCRYPT_DST_SIZE / 8));
	BigUint *tmp = new BigUint[tmp_size.get_ui()];

	for (BigUint i = 0; i < tmp_size; i++) {
		tmp[i.get_ui()] = 0;
		U(src + (ENCRYPT_DST_SIZE / 8) * i.get_ui(), ENCRYPT_DST_SIZE, tmp[i.get_ui()]);
	}

	//encrypt
	for (BigUint i = 0; i < tmp_size; i++) {
		tmp[i.get_ui()] = rsa::Decrypt(tmp[i.get_ui()], d, N);
	}

	//convert BigUint[] to char[]
	for (BigUint i = 0; i < tmp_size; i++) {
		P(tmp[i.get_ui()], ENCRYPT_BLOCK_SIZE, dst + (ENCRYPT_BLOCK_SIZE / 8) * i.get_ui());
	}

	delete[tmp_size.get_ui()]tmp;
}