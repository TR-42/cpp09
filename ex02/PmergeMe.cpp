#include "./PmergeMe.hpp"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>	 // DEBUG
#include <iterator>
#include <set>

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
	std::set<PmergeMe::VALUE_TYPE> uniqueCheck(this->_container1.begin(), this->_container1.end());
	if (this->_container1.size() != uniqueCheck.size())
		throw std::invalid_argument("invalid argument (not unique)");
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

static void _recursive(
	PmergeMe::CONTAINER_TYPE_1 &arr,
	size_t depth,
	size_t spanSize
)
{
	if (arr.size() < spanSize)
		return;

	size_t spanSizeHalf = spanSize / 2;
	for (
		PmergeMe::CONTAINER_TYPE_1::iterator it = arr.begin();
		std::distance(it, arr.end()) < spanSize;
	) {
		PmergeMe::CONTAINER_TYPE_1::iterator leftMaxIt = it + spanSizeHalf - 1;
		PmergeMe::CONTAINER_TYPE_1::iterator rightMaxIt = leftMaxIt + spanSizeHalf;
		PmergeMe::CONTAINER_TYPE_1::iterator nextIt = rightMaxIt + 1;
		if (*rightMaxIt < *leftMaxIt) {
			for (
				PmergeMe::CONTAINER_TYPE_1::iterator leftIt = it, rightIt = leftMaxIt + 1;
				rightIt != nextIt;
				++rightIt, ++leftIt
			) {
				PmergeMe::VALUE_TYPE temp = *leftIt;
				*leftIt = *rightIt;
				*rightIt = temp;
			}
		}
		it = nextIt;
	}
	_recursive(arr, depth + 1, spanSize * 2);
	return;
}

typedef std::deque<std::deque<PmergeMe::VALUE_TYPE> > Sort1PairsType;
void PmergeMe::sort1(
)
{
	Sort1PairsType pairs;
	for (
		PmergeMe::CONTAINER_TYPE_1::iterator it = this->_container1.begin();
		it != this->_container1.end();
		++it
	) {
		PmergeMe::CONTAINER_TYPE_1::iterator nextIt = it + 1;
		PmergeMe::CONTAINER_TYPE_1 pair;
		if (nextIt == this->_container1.end()) {
			pair.push_back(*it);
		} else if (*it <= *nextIt) {
			pair.push_back(*it);
			pair.push_back(*nextIt);
			++it;
		} else {
			pair.push_back(*nextIt);
			pair.push_back(*it);
			++it;
		}
		pairs.push_back(pair);
	}

	std::cout << "pairs.size(): " << pairs.size() << std::endl;

	while (1 < pairs.size()) {
		if (pairs.size() == 2 && pairs[0].size() != pairs[1].size()) {
			break;
		}

		for (
			Sort1PairsType::iterator leftIt = pairs.begin();
			leftIt != pairs.end();
			++leftIt
		) {
			Sort1PairsType::iterator rightIt = leftIt + 1;
			if ((leftIt + 1) == pairs.end()) {
				break;
			}
			PmergeMe::CONTAINER_TYPE_1::iterator insertToIt;
			if (rightIt->back() < leftIt->back()) {
				insertToIt = leftIt->begin();
			} else {
				insertToIt = leftIt->end();
			}
			std::cout << "leftIt->size(): " << leftIt->size() << std::endl;
			std::cout << "rightIt->size(): " << rightIt->size() << std::endl;
			leftIt->insert(insertToIt, rightIt->begin(), rightIt->end());

			leftIt = pairs.erase(rightIt) - 1;
		}
	}

	this->_container1 = pairs.front();
	if (pairs.size() == 2) {
		// DEBUG
		this->_container1.insert(this->_container1.end(), pairs.back().begin(), pairs.back().end());
	}
}

void PmergeMe::sort2(
)
{
	this->_container2.sort();
}
