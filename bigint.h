#pragma once
#ifndef BIGINT_H
#define BIGINT_H

#include <inttypes.h>
#include <cstdio>

#include <vector>
#include <string>

// It isn't a binary-coded decimal. :)

class bigint {
private:
public:
	// bool sign;
	std::vector<uint32_t> value;

	void trim();
	void div_level_up();

	void shift_right();
	void shift_left(const unsigned int &_value);
public:
	bigint();
	bigint(const unsigned int &source);

	friend const bigint operator+(const bigint &op1, const bigint &op2);
	friend const bigint operator-(const bigint &op1, const bigint &op2);
	friend const bigint operator*(const bigint &op1, const bigint &op2);
	friend const bigint operator/(const bigint &op1, const bigint &op2);
	friend const bigint operator%(const bigint &op1, const bigint &op2);

	friend bool operator==(const bigint &op1, const bigint &op2);
	friend bool operator<(const bigint &op1, const bigint &op2);
	friend bool operator>(const bigint &op1, const bigint &op2);
	friend bool operator<=(const bigint &op1, const bigint &op2);
	friend bool operator>=(const bigint &op1, const bigint &op2);

	friend const bigint pow(const bigint &op, const bigint &p);
	friend const bigint powmod(const bigint &op, const bigint &p, const bigint &m);

	friend const bigint div2(const bigint &op);
	friend const bigint div2n(const bigint &op, const unsigned int &n);

	friend const bigint sqrt(const bigint &op);
	friend void divmod(const bigint &op1, const bigint &op2, bigint &_div, bigint &_mod);

	std::string to_hex() const;
};

bigint make_bigint(const char *source);
#endif

