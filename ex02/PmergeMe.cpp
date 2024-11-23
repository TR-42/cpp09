#include "./PmergeMe.hpp"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>
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

static PmergeMe::CONTAINER_TYPE_1::iterator _swap_range(
	PmergeMe::CONTAINER_TYPE_1::iterator leftIt,
	PmergeMe::CONTAINER_TYPE_1::iterator rightIt
)
{
	PmergeMe::CONTAINER_TYPE_1::iterator it = leftIt, it2 = rightIt;
	while (it != rightIt) {
		PmergeMe::VALUE_TYPE temp = *it;
		*it = *it2;
		*it2 = temp;

		++it;
		++it2;
	}

	return it2;
}

static void _recursive(
	PmergeMe::CONTAINER_TYPE_1 &arr,
	const PmergeMe::CONTAINER_TYPE_1 &insertSpanCount,
	size_t spanSize
)
{
	if (arr.size() < spanSize)
		return;

	size_t spanSizeHalf = spanSize / 2;
	for (
		PmergeMe::CONTAINER_TYPE_1::iterator it = arr.begin();
		spanSize <= static_cast<size_t>(std::distance(it, arr.end()));
	) {
		PmergeMe::CONTAINER_TYPE_1::iterator leftMaxIt = it + spanSizeHalf - 1;
		PmergeMe::CONTAINER_TYPE_1::iterator rightMaxIt = leftMaxIt + spanSizeHalf;
		if (*rightMaxIt < *leftMaxIt) {
			it = _swap_range(it, leftMaxIt + 1);
		} else {
			it = rightMaxIt + 1;
		}
	}

	_recursive(arr, insertSpanCount, spanSize * 2);

	size_t spanCount = arr.size() / spanSize;
	bool isAdditionalSpanAvailable = (arr.size() % spanSize) != 0;
	if (isAdditionalSpanAvailable)
		++spanCount;
	if (spanCount <= 2)
		return;

	std::deque<std::deque<PmergeMe::CONTAINER_TYPE_1> > spanSet;
	for (
		// 最初の2ユニット(1スパン)は必ずソート済みなのでunsortedには含めない
		PmergeMe::CONTAINER_TYPE_1::iterator it = arr.begin() + spanSize;;
	) {
		size_t spanSetSize = spanSet.size();
		if (spanSetSize == 0 || spanSet.back().size() == insertSpanCount.at(spanSetSize - 1)) {
			spanSet.push_back(std::deque<PmergeMe::CONTAINER_TYPE_1>());
		}
		spanSet.back().push_back(std::deque<PmergeMe::VALUE_TYPE>(it, it + spanSizeHalf));
		it = arr.erase(it, it + spanSizeHalf);
		size_t distanceToEnd = static_cast<size_t>(std::distance(it, arr.end()));
		if (distanceToEnd < spanSize) {
			break;
		}
		it += spanSizeHalf;
	}

	size_t spanSetUnitCountSum = 0;
	while (spanSet.size() != 0) {
		std::deque<PmergeMe::CONTAINER_TYPE_1> targetSpanSet = spanSet.front();
		spanSet.pop_front();

		size_t initialTargetSpanSetSize = targetSpanSet.size();
		spanSetUnitCountSum += initialTargetSpanSetSize;
		while (targetSpanSet.size() != 0) {
			PmergeMe::CONTAINER_TYPE_1 targetSpan = targetSpanSet.back();
			targetSpanSet.pop_back();

			PmergeMe::VALUE_TYPE targetValue = targetSpan.back();
			// 最初の2ユニット(1スパン)は必ずソート済みなので比較対象範囲
			size_t searchRangeUnitCount = 2 + spanSetUnitCountSum - 1;
			PmergeMe::CONTAINER_TYPE_1::iterator rangeTopIt = arr.begin();
			for (size_t range = searchRangeUnitCount; 0 < range; range /= 2) {
				PmergeMe::CONTAINER_TYPE_1::iterator it2 = rangeTopIt + (range / 2) * spanSizeHalf;
				if (*(it2 + spanSizeHalf - 1) < targetValue) {
					rangeTopIt = it2 + spanSizeHalf;
				}
			}
			arr.insert(rangeTopIt, targetSpan.begin(), targetSpan.end());
		}
	}
}

template <typename T>
static T generate_insert_span_count(
	size_t requiredCount
)
{
	T container;
	size_t lastValue = 1;
	size_t sum = lastValue;
	size_t n = 1;
	container.push_back(lastValue);
	while ((sum * 2) < requiredCount) {
		size_t nextValue = 2 * lastValue + ((n++ & 1) == 0 ? 1 : -1);
		container.push_back(nextValue);
		lastValue = nextValue;
		// オーバーフローは到底起こり得ないのでチェックは省略
		sum += nextValue;
	}

	for (
		typename T::iterator it = container.begin();
		it != container.end();
		++it
	) {
		*it *= 2;
	}

	return container;
}

void PmergeMe::sort1(
)
{
	PmergeMe::CONTAINER_TYPE_1 insertSpanCount = generate_insert_span_count<PmergeMe::CONTAINER_TYPE_1>(this->_container1.size());
	_recursive(this->_container1, insertSpanCount, 2);
}

void PmergeMe::sort2(
)
{
	this->_container2.sort();
}
