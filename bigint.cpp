#include "bigint.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

bigint::bigint() /*: sign(false) */ {}
bigint::bigint(const unsigned int &source) {
	value.resize(1);
		value[0] = source;
			trim();
}

bigint make_bigint(const char *source) {
	bigint answer, ten(10);
	for(int i = 0; i < strlen(source); i++) {
		bigint answer2 = answer * ten;
		answer2.trim();
		bigint answer3 = answer2 + bigint(source[i] - '0');
		answer3.trim();
		answer = answer3;
		answer.trim();
	}
	return answer;
}

void bigint::trim() {
	for(int i = value.size() - 1; i >= 0; i--) {
		if(value[i] != 0) {
			value.resize(i + 1);
			goto b;
		}
	}
	value.resize(0);
b:;
}

void bigint::div_level_up() {
	value.resize(value.size() + 1);
	for(int i = value.size() - 1; i >= 1; i--) {
		value[i] = value[i - 1];
	}
	trim();
}

void bigint::shift_right() {
	for(unsigned int i = 0; i < value.size() - 1; i++) value[i] = value[i+1];
	value.resize(value.size() - 1);
	trim();
}

void bigint::shift_left(const unsigned int &_value) {
	value.resize(value.size() + 1);
	for(int i = value.size() - 2; i >= 0; i--) value[i+1] = value[i];
	value[0] = _value;
	trim();
}

const bigint operator+(const bigint &op1, const bigint &op2) {
	bigint answer;
	answer.value.resize(std::max(op1.value.size(), op2.value.size())+1);

	uint64_t r;
	uint32_t carry = 0;
	for(unsigned int i = 0; i < std::max(op1.value.size(), op2.value.size()); i++) {
		r = (uint64_t)(i < op1.value.size() ? op1.value[i] : 0) +
			(i < op2.value.size() ? op2.value[i] : 0) +
			carry;

		answer.value[i] = r;
		carry = r >> 32;
	}
	answer.value[std::max(op1.value.size(), op2.value.size())] = carry;
	answer.trim();

	return answer;
}

const bigint operator-(const bigint &op1, const bigint &op2) {
	bigint answer;

	if(op1 < op2) return answer;
	if(op2.value.size() == 0) return op1;

	answer.value.resize(op1.value.size());

	int64_t r;
	uint32_t carry = 0;
	for(unsigned int i = 0; i < op1.value.size(); i++) {
		r = (int64_t)(op1.value[i]) - 
			(i < op2.value.size() ? op2.value[i] : 0) -
			carry;
		if(r < 0) {
			r += 0x100000000;
			carry = 1;
		} else carry = 0;
		answer.value[i] = r;
	}
	answer.trim();

	return answer;
}

const bigint operator*(const bigint &op1, const bigint &op2) {
	bigint answer;

	answer.value.resize(op1.value.size() + op2.value.size());

	uint64_t r;
	uint32_t carry = 0;
	for(unsigned int i = 0; i < op1.value.size(); i++) {
		for(unsigned int j = 0; j < op2.value.size() || carry; j++) {
			r = (uint64_t)answer.value[i+j] + 
				(uint64_t)op1.value[i] * (j < op2.value.size() ? op2.value[j] : 0) + carry;
			carry = r >> 32;
			answer.value[i+j] = r & 0xffffffff;
		}
	}
	answer.trim();

	return answer;
}

void divmod(const bigint &op1, const bigint &op2, bigint &_div, bigint &_mod) {
	bigint zero;

	_div = zero; _div.value.resize(op1.value.size());
	_mod = zero; _mod.value.resize(op1.value.size());

	for(int i = op1.value.size() - 1; i >= 0; i--) {
		_mod.shift_left(op1.value[i]);

		int64_t x = 0;

		// <find x>
		int64_t left = 0;
		int64_t right = 0x100000000;
		while(left < right) {
			int64_t median = (left + right)>>1;
			if(op2 * bigint(median) <= _mod) {
				x = median;
				left = median + 1;
			} else {
				right = median;// - 1;
			}
		}
		// </find x>


		_div.shift_left(x);
		_mod = _mod - op2 * bigint(x);

	}

	_div.trim();
	_mod.trim();
}

const bigint operator/(const bigint &op1, const bigint &op2) {
	bigint _div, _mod;
	divmod(op1, op2, _div, _mod);
	return _div;
}

const bigint operator%(const bigint &op1, const bigint &op2) {
	bigint _div, _mod;
	divmod(op1, op2, _div, _mod);
	return _mod;
}

bool operator==(const bigint &op1, const bigint &op2) {
	if(op1.value.size() != op2.value.size()) return false;
	for(unsigned int i = 0; i < op1.value.size(); i++)
		if(op1.value[i] != op2.value[i]) return false;
	return true;
}

bool operator<(const bigint &op1, const bigint &op2) {
	if(op1.value.size() != op2.value.size())
		return (op1.value.size() < op2.value.size());
	for(int i = op1.value.size() - 1; i >= 0; i--)
		if(op1.value[i] != op2.value[i])
			return op1.value[i] < op2.value[i];
	return false;
}

bool operator>(const bigint &op1, const bigint &op2) {
	return op2 < op1;
}

bool operator<=(const bigint &op1, const bigint &op2) {
	return (op1 < op2) || (op1 == op2);
}

bool operator>=(const bigint &op1, const bigint &op2) {
	return (op1 > op2) || (op1 == op2);
}

const bigint pow(const bigint &op, const bigint &p) {
	// Fast algorithm.
	bigint result(1), current = op, n = p;
	bigint zero;
	while(!(n == zero)) {
		if(n.value[0] & 1) result = result * current;
		current = current * current;
		n = div2(n);
	}
	return result;
}

const bigint powmod(const bigint &op, const bigint &p, const bigint &m) {
	// Fast algorithm.
	bigint result(1), current = op, n = p;
	bigint zero;

	if(current > m) current = current % m;

	while(!(n == zero)) {
		if(n.value[0] & 1) {
			result = result * current;
			if(result > m) result = result % m;
		}

		current = current * current;
		if(current > m) current = current % m;

		n = div2(n);
	}
	return result;
}

const bigint div2n(const bigint &op, const unsigned int &n) {
	bigint answer;
	if(op.value.size() == 0) return answer;

	unsigned int _div = n / 32;
	unsigned int _mod = n % 32;

	answer.value.resize(op.value.size());
	for(unsigned int i = 0; i < op.value.size() - _div - 1; i++) {
		uint32_t modmask = ~(0xffffffff << _mod);
		answer.value[i] = (op.value[i + _div] >> _mod) + ((op.value[i + _div + 1] & modmask) << (32 - _mod));
	}

	unsigned int i = op.value.size() - _div - 1;
	answer.value[i] = (op.value[i + _div] >> _mod); // + ((op.value[i + _div + 1] & modmask) << (32 - _mod));
	answer.trim();

	return answer;
}

const bigint div2(const bigint &op) {
	return div2n(op, 1);
}

// Check negative left, right, median!
const bigint sqrt(const bigint &op) {
	bigint answer;

	int pos = (op.value.size() + 1) / 2;
	answer.value.resize(pos);
	pos--;

	while(pos >= 0) {
		uint64_t left = 0, right = 0xffffffff;
		uint32_t digit = 0;
		while(left <= right) {
			uint64_t median = (left + right)>>1;

			answer.value[pos] = median;

			if(answer * answer <= op) {
				digit = median;
				left = median + 1;
			} else {
				right = median - 1;
			}
		}
		answer.value[pos] = digit;
		pos--;
	}

	answer.trim();
	return answer;
}

std::string bigint::to_hex() const {
	std::string answer;
	if(!value.size()) answer = "0"; else {
		bool flag = true;
		for(int i = value.size() - 1; i >= 0; i--) {
			char temp[16];
			if(flag) {
				sprintf(temp, "%x", value[i]);
				flag = false;
			} else sprintf(temp, "%08x", value[i]);
			answer += temp;
		}
	}
	return answer;
}

