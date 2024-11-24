#include <cerrno>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

#include "./PmergeMe.hpp"

#define SEC_TO_US(time) ((time.tv_sec) * (1000 * 1000) + (time.tv_nsec / 1000))

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

static void print_result(
	const char *containerName,
	PmergeMe::CONTAINER_TYPE_1::size_type size,
	const struct timespec &time
)
{
	std::cout
		<< "Time to process a range of "
		<< std::setw(4) << size
		<< " elements with "
		<< containerName
		<< " : "
		<< SEC_TO_US(time)
		<< "."
		<< std::setw(3) << std::setfill('0') << (time.tv_nsec % 1000) << std::setfill(' ')
		<< " us"
		<< std::endl;
}

static struct timespec _sub_timespec(
	const struct timespec &start,
	const struct timespec &end
)
{
	struct timespec result;

	result.tv_sec = end.tv_sec - start.tv_sec;
	result.tv_nsec = end.tv_nsec - start.tv_nsec;
	if (result.tv_nsec < 0) {
		--result.tv_sec;
		result.tv_nsec += 1000 * 1000 * 1000;
	}
	return result;
}

static struct timespec _execSort(
	PmergeMe &v,
	void (PmergeMe::*sortMethod)()
)
{
	struct timespec start, end;

	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start) != 0) {
		const char *msg = std::strerror(errno);
		throw std::runtime_error(msg);
	}

	(v.*sortMethod)();

	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end) != 0) {
		const char *msg = std::strerror(errno);
		throw std::runtime_error(msg);
	}

	return _sub_timespec(start, end);
}

int main(
	int argc,
	const char **argv
)
{
	if (argc < 2) {
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " <positive integer>..."
			<< std::endl;
		return 1;
	}

	try {
		PmergeMe v(argc, argv);
		struct timespec sort1Time, sort2Time;

		print_container("Before: ", v.getContainer1());
		sort1Time = _execSort(v, &PmergeMe::sort1);
		sort2Time = _execSort(v, &PmergeMe::sort2);
#if defined(DEBUG) || defined(VALIDATE)
		std::cout
			<< std::endl
			<< "# RESULT ===" << std::endl;
#endif	// DEBUG || VALIDATE

		print_container("After:  ", v.getContainer1());
#if defined(DEBUG) || defined(VALIDATE)
		print_container("After2: ", v.getContainer2());
#endif	// DEBUG || VALIDATE

		print_result(__CONTAINER_TYPE_1_STR, v.getContainer1().size(), sort1Time);
		print_result(__CONTAINER_TYPE_2_STR, v.getContainer1().size(), sort2Time);

	} catch (const std::exception &e) {
		std::cerr
			<< "Error: "
			<< e.what()
			<< std::endl;
		return 1;
	}
	return 0;
}
