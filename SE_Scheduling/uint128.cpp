#include "uint128.h"

const unsigned long long pow2table[65] = {
	0x0,0x1,0x3,0x7,0xF,0x1F,0x3F,0x7F,0xFF,
	0x1FF,0x3FF,0x7FF,0xFFF,0x1FFF,0x3FFF,0x7FFF,0xFFFF,
	0x1FFFF,0x3FFFF,0x7FFFF,0xFFFFF,0x1FFFFF,0x3FFFFF,0x7FFFFF,0xFFFFFF,
	0x1FFFFFF,0x3FFFFFF,0x7FFFFFF,0xFFFFFFF,0x1FFFFFFF,0x3FFFFFFF,0x7FFFFFFF,0xFFFFFFFF,
	0x1FFFFFFFF,0x3FFFFFFFF,0x7FFFFFFFF,0xFFFFFFFFF,0x1FFFFFFFFF,0x3FFFFFFFFF,0x7FFFFFFFFF,0xFFFFFFFFFF,
	0x1FFFFFFFFFF,0x3FFFFFFFFFF,0x7FFFFFFFFFF,0xFFFFFFFFFFF,0x1FFFFFFFFFFF,0x3FFFFFFFFFFF,0x7FFFFFFFFFFF,0xFFFFFFFFFFFF,
	0x1FFFFFFFFFFFF,0x3FFFFFFFFFFFF,0x7FFFFFFFFFFFF,0xFFFFFFFFFFFFF,0x1FFFFFFFFFFFFF,0x3FFFFFFFFFFFFF,0x7FFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,
	0x1FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF
};

uint128 uint128::operator >> (int x) const
{
	uint128 tmp = *this;
	if (x >= 64) {
		tmp.x2 = tmp.x1;
		tmp.x1 = 0;
		tmp.x2 = tmp.x2 >> (x - 64);
		return tmp;
	}
	else if (x >= 0) {
		unsigned long long shiftDown = (tmp.x1 & pow2table[x]) << (64 - x);
		tmp.x2 = (tmp.x2 >> x) | shiftDown;
		tmp.x1 = tmp.x1 >> x;
		return tmp;
	}
	else return *this << (-x);
}

uint128 uint128::operator << (int x) const
{
	uint128 tmp = *this;
	if (x >= 64) {
		tmp.x1 = tmp.x2;
		tmp.x2 = 0;
		tmp.x1 = tmp.x1 << (x - 64);
		return tmp;
	}
	else if (x >= 0) {
		unsigned long long shiftUp = (tmp.x2 & ~pow2table[64 - x]) >> (64 - x);
		tmp.x1 = (tmp.x1 << x) | shiftUp;
		tmp.x2 = tmp.x2 << x;
		return tmp;
	}
	else return *this >> (-x);
}

uint128  uint128::operator = (string str) {
	int i = 0;
	uint128 tmp;
	while (i < str.length() && str[i] == '0') i++;
	if (i >= str.length()) return tmp;
	str = str.substr(i);
	for (i = 0; i < str.length(); ++i) str[i] -= '0';
	for (i = 0; i < 128; ++i) {
		tmp = tmp >> 1;
		if ((str[str.length() - 1]) % 2) tmp.x1 |= 0x8000000000000000;
		for (int j = 0; j < str.length(); ++j) {
			if (str[j] % 2 && j < str.length() - 1) str[j + 1] += 10;
			str[j] = str[j] / 2;
		}
	}
	*this = tmp;
	return tmp;
}

uint128  uint128::operator = (uint128 x) {
	x1 = x.x1;
	x2 = x.x2;
	return *this;
}

uint128 uint128::operator = (unsigned long long x)
{
	uint128 tmp;
	tmp.x2 = x;
	*this = tmp;
	return tmp;
}

uint128 uint128::operator & (uint128 x) const
{
	uint128 tmp = *this;
	tmp.x1 &= x.x1;
	tmp.x2 &= x.x2;
	return tmp;
}

unsigned long long uint128::operator & (unsigned long long x) const
{
	return this->x2 & x;
}

uint128 uint128::operator | (uint128 x) const
{
	uint128 tmp = *this;
	tmp.x1 |= x.x1;
	tmp.x2 |= x.x2;
	return tmp;
}

uint128 uint128::operator | (unsigned long long x) const
{
	uint128 tmp = *this;
	tmp.x2 |= x;
	return tmp;
}

uint128 uint128::operator ~ () const
{
	uint128 tmp = *this;
	tmp.x1 = ~tmp.x1;
	tmp.x2 = ~tmp.x2;
	return tmp;
}

uint128::operator bool() const
{
	return x1 || x2;
}

int uint128::count1s() const
{
	uint128 tmp = *this;
	int cnt = 0;
	for (int i = 0; i < 128; ++i) {
		if (tmp & 0x1Ui64) cnt++;
		tmp = tmp >> 1;
	}
	return cnt;
}

int uint128::getLast1()const
{
	uint128 tmp = *this;
	int cnt = 0;
	for (; cnt < 128; ++cnt) {
		if (tmp & 0x1Ui64) return cnt;
		tmp = tmp >> 1;
	}
	return -1;
}

string uint128::toStr() const
{
	string ret = "";
	char res[45] = { 0 };
	uint128 tmp = *this;
	int i;
	for (i = 0; i < 128; ++i) {
		for (int j = 43; j > 0; --j) res[j] *= 2;
		for (int j = 43; j > 0; --j)
			if (res[j] > 9) {
				res[j] -= 10;
				res[j - 1]++;
			}
		if (tmp.x1 & 0x8000000000000000) {
			res[43]++;
			int j = 43;
			while (res[j] > 9) {
				res[j - 1]++;
				res[j] -= 10;
				j--;
			}
		}
		tmp = tmp << 1;
	}
	i = 0;
	while (i<=43 && res[i] == 0) i++;
	for (int j = i; j <= 43; ++j) res[j] += '0';
	if (i >= 44) {
		i = 43;
		res[43] = '0';
	}

	ret = res + i;
	return ret;
}

string uint128::toHexStr() const
{
	string res = "0x";
	char tmpStr[17] = {0};
	unsigned long long tmp = x1;
	for (int i = 15; i >= 0; --i) {
		tmpStr[i] = tmp & 0xF;
		tmpStr[i] += tmpStr[i] < 10 ? '0' : 'A' - 10;
		tmp = tmp >> 4;
	}
	res += tmpStr;
	tmp = x2;
	for (int i = 15; i >= 0; --i) {
		tmpStr[i] = tmp & 0xF;
		tmpStr[i] += tmpStr[i] < 10 ? '0' : 'A' - 10;
		tmp = tmp >> 4;
	}
	return res;
}