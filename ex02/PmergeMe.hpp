#pragma once

#include <time.h>

#include <deque>
#include <list>

#define __CONTAINER_TYPE_1 std::deque<PmergeMe::VALUE_TYPE>
#define __CONTAINER_TYPE_2 std::list<PmergeMe::VALUE_TYPE>
#define __CONTAINER_TYPE_1_STR "std::deque"
#define __CONTAINER_TYPE_2_STR "std::list "

class PmergeMe
{
 public:
	typedef unsigned long long VALUE_TYPE;
	typedef __CONTAINER_TYPE_1 CONTAINER_TYPE_1;
	typedef __CONTAINER_TYPE_2 CONTAINER_TYPE_2;

 private:
	static VALUE_TYPE MAX;
	static VALUE_TYPE MIN;
	PmergeMe::CONTAINER_TYPE_1 _container1;
	PmergeMe::CONTAINER_TYPE_2 _container2;

 public:
	PmergeMe();
	PmergeMe(int argc, const char **argv);
	PmergeMe(const PmergeMe &src);
	virtual ~PmergeMe();
	PmergeMe &operator=(const PmergeMe &src);

	const PmergeMe::CONTAINER_TYPE_1 getContainer1() const;
	const PmergeMe::CONTAINER_TYPE_2 getContainer2() const;
	void sort1();
	void sort2();
};
