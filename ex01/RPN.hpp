#pragma once

#include <stack>
#include <string>

class RPN
{
 public:
	typedef long VALUE_TYPE;
	static RPN::VALUE_TYPE getMAX();
	static RPN::VALUE_TYPE getMIN();

 private:
	static VALUE_TYPE MAX;
	static VALUE_TYPE MIN;
	std::stack<RPN::VALUE_TYPE> _valueStack;

 public:
	RPN();
	RPN(const RPN &src);
	virtual ~RPN();
	RPN &operator=(const RPN &src);

	RPN::VALUE_TYPE getResult() const;
	void processInput(char input);
};
