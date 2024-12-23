#include "./PmergeMe.hpp"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>

PmergeMe::VALUE_TYPE PmergeMe::MAX = std::numeric_limits<PmergeMe::VALUE_TYPE>::max();
PmergeMe::VALUE_TYPE PmergeMe::MIN = std::numeric_limits<PmergeMe::VALUE_TYPE>::min();

#ifdef DEBUG
template <typename T>
static void print_container(
	const std::string &header,
	const T &container
)
{
	std::cout
		<< header;
	for (
		typename T::const_iterator it = container.begin();
		it != container.end();
		++it
	) {
		std::cout
			<< " "
			<< *it;
	}
	std::cout
		<< std::endl;
}
#endif	// DEBUG

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
			const char *msg = std::strerror(errno);
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

static PmergeMe::CONTAINER_TYPE_2::iterator __it_add(
	PmergeMe::CONTAINER_TYPE_2::iterator it,
	size_t n
)
{
	for (size_t i = 0; i < n; i++) {
		++it;
	}
	return it;
}

static PmergeMe::CONTAINER_TYPE_1::iterator _getInsertTo1(
	PmergeMe::CONTAINER_TYPE_1::iterator rangeTopIt,
	size_t rangeUnitCount,
	size_t unitSize,
	PmergeMe::VALUE_TYPE targetValue
)
{
#ifdef DEBUG
	std::cout
		<< std::endl
		<< "_getInsertTo1(): "
		<< "rangeUnitCount: "
		<< rangeUnitCount
		<< ", rangeTopItValue: "
		<< *rangeTopIt
		<< ", unitSize: "
		<< unitSize
		<< ", targetValue: "
		<< targetValue
		<< std::endl;
#endif	// DEBUG
	if (rangeUnitCount == 0)
		return rangeTopIt;
	PmergeMe::CONTAINER_TYPE_1::iterator rangeCenterUnitTopIt = rangeTopIt + (rangeUnitCount / 2) * unitSize;
	PmergeMe::CONTAINER_TYPE_1::iterator rangeCenterUnitEndIt = rangeCenterUnitTopIt + unitSize;
	PmergeMe::VALUE_TYPE rangeCenterUnitMaxValue = *(rangeCenterUnitEndIt - 1);
#ifdef DEBUG
	std::cout
		<< "rangeEndIt(Value): "
		<< *((rangeTopIt + rangeUnitCount * unitSize) - 1)
		<< ", rangeCenterUnitTopIt: "
		<< *rangeCenterUnitTopIt
		<< ", rangeCenterUnitMaxValue: "
		<< rangeCenterUnitMaxValue
		<< std::endl;
#endif	// DEBUG
	if (rangeCenterUnitMaxValue < targetValue) {
		// 前半分 + targetが除かれる
		return _getInsertTo1(rangeCenterUnitEndIt, rangeUnitCount - (rangeUnitCount / 2) - 1, unitSize, targetValue);
	} else {
		return _getInsertTo1(rangeTopIt, rangeUnitCount / 2, unitSize, targetValue);
	}
}

static PmergeMe::CONTAINER_TYPE_2::iterator _getInsertTo2(
	PmergeMe::CONTAINER_TYPE_2::iterator rangeTopIt,
	size_t rangeUnitCount,
	size_t unitSize,
	PmergeMe::VALUE_TYPE targetValue
)
{
#ifdef DEBUG
	std::cout
		<< std::endl
		<< "_getInsertTo1(): "
		<< "rangeUnitCount: "
		<< rangeUnitCount
		<< ", rangeTopItValue: "
		<< *rangeTopIt
		<< ", unitSize: "
		<< unitSize
		<< ", targetValue: "
		<< targetValue
		<< std::endl;
#endif	// DEBUG
	if (rangeUnitCount == 0)
		return rangeTopIt;
	PmergeMe::CONTAINER_TYPE_2::iterator rangeCenterUnitTopIt = __it_add(rangeTopIt, (rangeUnitCount / 2) * unitSize);
	PmergeMe::CONTAINER_TYPE_2::iterator rangeCenterUnitEndIt = __it_add(rangeCenterUnitTopIt, unitSize);
	PmergeMe::CONTAINER_TYPE_2::iterator tmpRangeCenterUnitMaxValIt = rangeCenterUnitEndIt;
	PmergeMe::VALUE_TYPE rangeCenterUnitMaxValue = *(--tmpRangeCenterUnitMaxValIt);
#ifdef DEBUG
	PmergeMe::CONTAINER_TYPE_2::iterator rangeEndValIt = __it_add(rangeTopIt, rangeUnitCount * unitSize - 1);
	std::cout
		<< "rangeEndIt(Value): "
		<< *rangeEndValIt
		<< ", rangeCenterUnitTopIt: "
		<< *rangeCenterUnitTopIt
		<< ", rangeCenterUnitMaxValue: "
		<< rangeCenterUnitMaxValue
		<< std::endl;
#endif	// DEBUG
	if (rangeCenterUnitMaxValue < targetValue) {
		if (rangeUnitCount == 1)
			return rangeCenterUnitEndIt;
		// 前半分 + targetが除かれる
		return _getInsertTo2(rangeCenterUnitEndIt, rangeUnitCount - (rangeUnitCount / 2) - 1, unitSize, targetValue);
	} else {
		return _getInsertTo2(rangeTopIt, rangeUnitCount / 2, unitSize, targetValue);
	}
}

static void _sort1(
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
			it = std::swap_ranges(it, leftMaxIt + 1, leftMaxIt + 1);
		} else {
			it = rightMaxIt + 1;
		}
	}

#ifdef DEBUG
	std::cout
		<< "spanSize: "
		<< spanSize
		<< std::endl;
	print_container("before recursive:", arr);
#endif	// DEBUG

	_sort1(arr, insertSpanCount, spanSize * 2);

	size_t spanCount = arr.size() / spanSize;
	bool isAdditionalSpanAvailable = spanSizeHalf <= (arr.size() % spanSize);
	if (isAdditionalSpanAvailable)
		++spanCount;
	if (spanCount < 2)
		return;
#ifdef DEBUG
	std::cout
		<< "spanCount: "
		<< spanCount
		<< std::endl;
	print_container("before erase:", arr);
#endif	// DEBUG

	std::deque<std::deque<PmergeMe::CONTAINER_TYPE_1> > spanSet;
	PmergeMe::CONTAINER_TYPE_1::const_iterator insertSpanCountIt = insertSpanCount.begin();
	for (
		// 最初の2ユニット(1スパン)は必ずソート済みなのでunsortedには含めない
		PmergeMe::CONTAINER_TYPE_1::iterator it = arr.begin() + spanSize;;
	) {
		size_t spanSetSize = spanSet.size();
		if (spanSetSize == 0 || spanSet.back().size() == *insertSpanCountIt) {
			spanSet.push_back(std::deque<PmergeMe::CONTAINER_TYPE_1>());
			++insertSpanCountIt;
		}
		spanSet.back().push_back(std::deque<PmergeMe::VALUE_TYPE>(it, it + spanSizeHalf));
		it = arr.erase(it, it + spanSizeHalf);
		size_t distanceToEnd = static_cast<size_t>(std::distance(it, arr.end()));
		if (distanceToEnd < spanSize) {
			break;
		}
		it += spanSizeHalf;
	}

	size_t lastSpanSetSize = 0;
	size_t searchRangeUnitCount = 1;
	while (spanSet.size() != 0) {
		std::deque<PmergeMe::CONTAINER_TYPE_1> targetSpanSet = spanSet.front();
		spanSet.pop_front();

		size_t initialTargetSpanSetSize = targetSpanSet.size();
		// 最初の2ユニット(1スパン)は必ずソート済みなので比較対象範囲
		searchRangeUnitCount += lastSpanSetSize + initialTargetSpanSetSize;
		lastSpanSetSize = initialTargetSpanSetSize;
		while (targetSpanSet.size() != 0) {
			PmergeMe::CONTAINER_TYPE_1 targetSpan = targetSpanSet.back();
			targetSpanSet.pop_back();

			// そのunitのMAX値
			PmergeMe::VALUE_TYPE targetValue = targetSpan.back();
#ifdef DEBUG
			print_container("arr Before:", arr);
			print_container("targetSpan:", targetSpan);
			std::cout
				<< "searchRangeUnitCount: "
				<< searchRangeUnitCount
				<< std::endl;
			std::cout
				<< "spanSizeHalf: "
				<< spanSizeHalf
				<< std::endl;
			std::cout
				<< "targetValue: "
				<< targetValue
				<< std::endl;
#endif	// DEBUG
			PmergeMe::CONTAINER_TYPE_1::iterator insertToIt = _getInsertTo1(arr.begin(), searchRangeUnitCount, spanSizeHalf, targetValue);
#ifdef DEBUG
			std::cout
				<< "insertTo: "
				<< *insertToIt;
			if (insertToIt == arr.begin()) {
				std::cout << " (begin)";
			} else {
				std::cout << " (after " << *(insertToIt - 1) << ")";
			}
			std::cout
				<< std::endl;
#endif	// DEBUG
			arr.insert(insertToIt, targetSpan.begin(), targetSpan.end());
#ifdef DEBUG
			print_container("arr After :", arr);
			std::cout << std::endl;
#endif	// DEBUG
		}
	}
}

static void _sort2(
	PmergeMe::CONTAINER_TYPE_2 &arr,
	const PmergeMe::CONTAINER_TYPE_2 &insertSpanCount,
	size_t spanSize
)
{
	if (arr.size() < spanSize)
		return;

	size_t spanSizeHalf = spanSize / 2;
	for (
		PmergeMe::CONTAINER_TYPE_2::iterator it = arr.begin();
		spanSize <= static_cast<size_t>(std::distance(it, arr.end()));
	) {
		PmergeMe::CONTAINER_TYPE_2::iterator leftMaxIt = __it_add(it, spanSizeHalf - 1);
		PmergeMe::CONTAINER_TYPE_2::iterator rightMaxIt = __it_add(leftMaxIt, spanSizeHalf);
		if (*rightMaxIt < *leftMaxIt) {
			PmergeMe::CONTAINER_TYPE_2::iterator rightMinIt = leftMaxIt;
			++rightMinIt;
			it = std::swap_ranges(it, rightMinIt, rightMinIt);
		} else {
			it = ++rightMaxIt;
		}
	}

#ifdef DEBUG
	std::cout
		<< "spanSize: "
		<< spanSize
		<< std::endl;
	print_container("before recursive:", arr);
#endif	// DEBUG

	_sort2(arr, insertSpanCount, spanSize * 2);

	size_t spanCount = arr.size() / spanSize;
	bool isAdditionalSpanAvailable = spanSizeHalf <= (arr.size() % spanSize);
	if (isAdditionalSpanAvailable)
		++spanCount;
	if (spanCount < 2)
		return;
#ifdef DEBUG
	std::cout
		<< "spanCount: "
		<< spanCount
		<< std::endl;
	print_container("before erase:", arr);
#endif	// DEBUG

	std::list<std::list<PmergeMe::CONTAINER_TYPE_2> > spanSet;
	PmergeMe::CONTAINER_TYPE_2::const_iterator insertSpanCountIt = insertSpanCount.begin();
	for (
		// 最初の2ユニット(1スパン)は必ずソート済みなのでunsortedには含めない
		PmergeMe::CONTAINER_TYPE_2::iterator it = __it_add(arr.begin(), spanSize);;
	) {
		size_t spanSetSize = spanSet.size();
		if (spanSetSize == 0 || spanSet.back().size() == *insertSpanCountIt) {
			spanSet.push_back(std::list<PmergeMe::CONTAINER_TYPE_2>());
			++insertSpanCountIt;
		}
		PmergeMe::CONTAINER_TYPE_2::iterator moveEndIt = __it_add(it, spanSizeHalf);
		PmergeMe::CONTAINER_TYPE_2 targetSpan;
		targetSpan.splice(targetSpan.begin(), arr, it, moveEndIt);
		spanSet.back().push_back(targetSpan);
		it = moveEndIt;
		size_t distanceToEnd = static_cast<size_t>(std::distance(it, arr.end()));
		if (distanceToEnd < spanSize) {
			break;
		}
		it = __it_add(it, spanSizeHalf);
	}

	size_t lastSpanSetSize = 0;
	size_t searchRangeUnitCount = 1;
	while (spanSet.size() != 0) {
		std::list<PmergeMe::CONTAINER_TYPE_2> targetSpanSet = spanSet.front();
		spanSet.pop_front();

		size_t initialTargetSpanSetSize = targetSpanSet.size();
		// 最初の2ユニット(1スパン)は必ずソート済みなので比較対象範囲
		searchRangeUnitCount += lastSpanSetSize + initialTargetSpanSetSize;
		lastSpanSetSize = initialTargetSpanSetSize;
		while (targetSpanSet.size() != 0) {
			PmergeMe::CONTAINER_TYPE_2 targetSpan = targetSpanSet.back();
			targetSpanSet.pop_back();

			PmergeMe::VALUE_TYPE targetValue = targetSpan.back();
#ifdef DEBUG
			print_container("arr Before:", arr);
			print_container("targetSpan:", targetSpan);
			std::cout
				<< "searchRangeUnitCount: "
				<< searchRangeUnitCount
				<< std::endl;
			std::cout
				<< "targetValue: "
				<< targetValue
				<< std::endl;
#endif	// DEBUG
			PmergeMe::CONTAINER_TYPE_2::iterator insertToIt = _getInsertTo2(arr.begin(), searchRangeUnitCount, spanSizeHalf, targetValue);
#ifdef DEBUG
			PmergeMe::CONTAINER_TYPE_2::iterator tmpInsertToItBack = insertToIt;
			std::cout
				<< "insertTo: "
				<< *insertToIt;
			if (insertToIt == arr.begin()) {
				std::cout << " (begin)";
			} else {
				std::cout << " (after " << *(--tmpInsertToItBack) << ")";
			}
			std::cout
				<< std::endl;
#endif	// DEBUG
			arr.splice(insertToIt, targetSpan);
#ifdef DEBUG
			print_container("arr After :", arr);
			std::cout << std::endl;
#endif	// DEBUG
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
#if defined(DEBUG) || defined(VALIDATE)
	std::cout << std::endl
						<< "# sort1 ===" << std::endl;
#endif	// DEBUG || VALIDATE
	PmergeMe::CONTAINER_TYPE_1 insertSpanCount = generate_insert_span_count<PmergeMe::CONTAINER_TYPE_1>(this->_container1.size());
	_sort1(this->_container1, insertSpanCount, 2);

#if defined(DEBUG) || defined(VALIDATE)
	PmergeMe::VALUE_TYPE lastValue = this->_container1.front();
	for (
		PmergeMe::CONTAINER_TYPE_1::const_iterator it = this->_container1.begin() + 1;
		it != this->_container1.end();
		++it
	) {
		if (lastValue > *it) {
			std::cerr
				<< "sort1 failed ("
				<< lastValue
				<< " > "
				<< *it
				<< ")"
				<< std::endl;
		}
		lastValue = *it;
	}
#endif	// DEBUG || VALIDATE
}

void PmergeMe::sort2(
)
{
#if defined(DEBUG) || defined(VALIDATE)
	std::cout << std::endl
						<< "# sort2 ===" << std::endl;
#endif	// DEBUG || VALIDATE
	PmergeMe::CONTAINER_TYPE_2 insertSpanCount = generate_insert_span_count<PmergeMe::CONTAINER_TYPE_2>(this->_container2.size());
	_sort2(this->_container2, insertSpanCount, 2);
#if defined(DEBUG) || defined(VALIDATE)
	PmergeMe::VALUE_TYPE lastValue = this->_container2.front();
	for (
		PmergeMe::CONTAINER_TYPE_2::const_iterator it = this->_container2.begin();
		it != this->_container2.end();
		++it
	) {
		if (it == this->_container2.begin())
			continue;
		if (lastValue > *it) {
			std::cerr
				<< "sort2 failed ("
				<< lastValue
				<< " > "
				<< *it
				<< ")"
				<< std::endl;
		}
		lastValue = *it;
	}
#endif	// DEBUG || VALIDATE
}
