#pragma once

#include<string>

using namespace std;

class uint128 {
public:
	uint128() :x1(0), x2(0) {}
	uint128(int x) :x1(0), x2(x) {}
	uint128(unsigned long long x) : x1(0), x2(x) {};

	uint128 operator = (string str);
	uint128 operator = (uint128 x);
	uint128 operator = (unsigned long long x);
	uint128 operator >> (int x)const;
	uint128 operator << (int x)const;
	uint128 operator & (uint128 x)const;
	unsigned long long operator & (unsigned long long x)const;
	uint128 operator | (uint128 x)const;
	uint128 operator | (unsigned long long x)const;
	uint128 operator ~ ()const;
	operator bool()const;
	int count1s()const;
	int getLast1()const;
	string toStr()const;
	string toHexStr()const;

	unsigned long long x1, x2;
};