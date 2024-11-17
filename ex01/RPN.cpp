#include "./RPN.hpp"

#include <cctype>
#include <limits>
#include <stdexcept>

RPN::VALUE_TYPE RPN::MAX = std::numeric_limits<RPN::VALUE_TYPE>::max();
RPN::VALUE_TYPE RPN::MIN = std::numeric_limits<RPN::VALUE_TYPE>::min();
RPN::VALUE_TYPE RPN::getMAX()
{
	return RPN::MAX;
}
RPN::VALUE_TYPE RPN::getMIN()
{
	return RPN::MIN;
}

RPN::RPN(
) : _valueStack()
{
}

RPN::RPN(
	const RPN &src
) : _valueStack(src._valueStack)
{
}

RPN::~RPN(
)
{
}

RPN &RPN::operator=(
	const RPN &src
)
{
	if (this == &src)
		return *this;

	this->_valueStack = src._valueStack;

	return *this;
}

RPN::VALUE_TYPE RPN::getResult(
) const
{
	if (_valueStack.size() != 1)
		throw std::invalid_argument("stack size is not 1");

	return _valueStack.top();
}

#define OP(op, validator) \
	{ \
		if (_valueStack.size() < 2) \
			throw std::invalid_argument("value stack is empty"); \
		VALUE_TYPE right = _valueStack.top(); \
		_valueStack.pop(); \
		VALUE_TYPE left = _valueStack.top(); \
		_valueStack.pop(); \
		validator(left, right); \
		_valueStack.push(left op right); \
	}
#define OP_CASE(op, validator) \
	case #op[0]: \
		OP(op, validator); \
		break;

static void _validate_plus(
	RPN::VALUE_TYPE left,
	RPN::VALUE_TYPE right
)
{
	if (0 < left && 0 < right && RPN::getMAX() - right < left)
		throw std::overflow_error("overflow");
	if (left < 0 && right < 0 && left < RPN::getMAX() - right)
		throw std::underflow_error("underflow");
}
static void _validate_minus(
	RPN::VALUE_TYPE left,
	RPN::VALUE_TYPE right
)
{
	if (0 < left && right < 0 && RPN::getMAX() + right < left)
		throw std::overflow_error("overflow");
	if (left < 0 && 0 < right && left < RPN::getMIN() + right)
		throw std::underflow_error("underflow");
}
static void _validate_multiply(
	RPN::VALUE_TYPE left,
	RPN::VALUE_TYPE right
)
{
	if (left == 0 || right == 0)
		return;
	if ((0 < left && 0 < right) || (left < 0 && right < 0)) {
		if (left == RPN::getMIN() || right == RPN::getMIN())
			throw std::overflow_error("overflow");
		else if (RPN::getMAX() / left < right)
			throw std::overflow_error("overflow");
	} else if (left < 0) {
		if (left == RPN::getMIN())
			throw std::overflow_error("overflow");
		else if (left < RPN::getMIN() / right)
			throw std::overflow_error("underflow");
	} else {
		if (right == RPN::getMIN())
			throw std::overflow_error("overflow");
		else if (right < RPN::getMIN() / left)
			throw std::overflow_error("underflow");
	}
}
static void _validate_divide(
	RPN::VALUE_TYPE left,
	RPN::VALUE_TYPE right
)
{
	if (right == 0)
		throw std::invalid_argument("division by zero");
	if (left == RPN::getMIN() && right == -1)
		throw std::overflow_error("overflow");
}
void RPN::processInput(
	char input
)
{
	switch (input) {
		OP_CASE(+, _validate_plus);
		OP_CASE(-, _validate_minus);
		OP_CASE(*, _validate_multiply);
		OP_CASE(/, _validate_divide);

		default:
			if (std::isdigit(input))
				_valueStack.push(input - '0');
			else
				throw std::invalid_argument("invalid input");
			break;
	}
}
