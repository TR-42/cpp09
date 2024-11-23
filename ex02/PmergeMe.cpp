#include "./PmergeMe.hpp"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>

PmergeMe::VALUE_TYPE PmergeMe::MAX = std::numeric_limits<PmergeMe::VALUE_TYPE>::max();
PmergeMe::VALUE_TYPE PmergeMe::MIN = std::numeric_limits<PmergeMe::VALUE_TYPE>::min();

PmergeMe::PmergeMe()
{
}

static bool isPositiveIntStr(
	const char *str
)
{
	while (*str != '\0') {
		if (!std::isdigit(*str))
			return false;
		str++;
	}
	return true;
}

PmergeMe::PmergeMe(
	int argc,
	const char **argv
) : _container1(),
		_container2()
{
	errno = 0;
	char *endptr;
	for (int i = 1; i < argc; i++) {
		if (!isPositiveIntStr(argv[i]))
			throw std::invalid_argument("invalid argument");
		PmergeMe::VALUE_TYPE value = std::strtoull(argv[i], &endptr, 10);
		if (errno == ERANGE) {
			const char *msg = strerror(errno);
			throw std::out_of_range(msg);
		} else if (*endptr != '\0') {
			throw std::invalid_argument("invalid argument");
		}
		this->_container1.push_back(value);
	}
	this->_container2 = std::list<PmergeMe::VALUE_TYPE>(this->_container1.begin(), this->_container1.end());
}

PmergeMe::PmergeMe(
	const PmergeMe &src
) : _container1(src._container1),
		_container2(src._container2)
{
}

PmergeMe::~PmergeMe(
)
{
}

PmergeMe &PmergeMe::operator=(
	const PmergeMe &src
)
{
	if (this == &src)
		return *this;

	this->_container1 = src._container1;
	this->_container2 = src._container2;

	return *this;
}

const PmergeMe::CONTAINER_TYPE_1 PmergeMe::getContainer1(
) const
{
	return this->_container1;
}
const PmergeMe::CONTAINER_TYPE_2 PmergeMe::getContainer2(
) const
{
	return this->_container2;
}

void PmergeMe::sort1(
)
{
	std::sort(this->_container1.begin(), this->_container1.end());
}

void PmergeMe::sort2(
)
{
	this->_container2.sort();
}
