#include "./BitcoinExchange.hpp"

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

#define COLUMN_NAME_DATE "date"
#define COLUMN_NAME_PRICE "exchange_rate"
#define CSV_HEADER COLUMN_NAME_DATE "," COLUMN_NAME_PRICE

bool isValidDateStr(
	const std::string &date
)
{
	if (date.length() != sizeof(DATE_FORMAT) - 1) {
		return false;
	}

	for (std::size_t i = 0; i < sizeof(DATE_FORMAT) - 1; i++) {
		if (DATE_FORMAT[i] == '-') {
			if (date[i] != '-') {
				return false;
			}
		} else {
			if (!std::isdigit(date[i])) {
				return false;
			}
		}
	}

	int month = std::atoi(date.substr(5, 2).c_str());
	if (12 < month)
		return false;
	int day = std::atoi(date.substr(8, 2).c_str());
	switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (31 < day)
				return false;
			break;

		case 4:
		case 6:
		case 9:
		case 11:
			if (30 < day)
				return false;
			break;

		case 2:
			if (29 < day)
				return false;
			bool isLeapYear = false;
			int year = std::atoi(date.substr(0, 4).c_str());
			// 特殊な暦は考慮しない
			if (year % 4 == 0) {
				if (year % 100 == 0) {
					if (year % 400 == 0)
						isLeapYear = true;
				} else {
					isLeapYear = true;
				}
			}
			if (!isLeapYear && 28 < day)
				return false;
			break;
	}

	return true;
}
bool isValidPositiveNumStr(
	const std::string &str
)
{
	bool hasDot = false;

	if (str.empty())
		return false;

	for (std::size_t i = 0; i < str.length(); i++) {
		if (str[i] == '.') {
			if (hasDot)
				return false;
			hasDot = true;
		} else if (!std::isdigit(str[i])) {
			return false;
		}
	}

	return true;
}

BitcoinExchange::BitcoinExchange(
) : _priceHistory()
{
}

BitcoinExchange::BitcoinExchange(
	const BitcoinExchange &src
) : _priceHistory(src._priceHistory)
{
}

BitcoinExchange::~BitcoinExchange(
)
{
}

BitcoinExchange &BitcoinExchange::operator=(
	const BitcoinExchange &src
)
{
	if (this == &src)
		return *this;

	this->_priceHistory = src._priceHistory;

	return *this;
}

double BitcoinExchange::getLatestPriceAt(const std::string &date) const
{
	double latestPrice = 0.0f;
	for (
		std::vector<PriceHistory>::const_iterator it = _priceHistory.begin();
		it != _priceHistory.end();
		++it
	) {
		if (it->date == date)
			return it->price;
		else if (date < it->date)
			break;
		else
			latestPrice = it->price;
	}
	return latestPrice;
}

BitcoinExchange BitcoinExchange::loadFromFile(
	const std::string &filePath
)
{
	std::ifstream dbFile(filePath);

	if (!dbFile)
		throw std::invalid_argument("Failed to open file");

	std::string line;
	std::size_t lineNum = 0;
	std::stringstream errorStr;
	BitcoinExchange db;
	bool hasAnyError = false;
	bool isPreviousLineEmpty = false;
	while (std::getline(dbFile, line)) {
		++lineNum;
		if (lineNum == 1) {
			if (line != (CSV_HEADER)) {
				hasAnyError = true;
				errorStr
					<< "line[" << lineNum << "]: "
					<< "Invalid header: " << line
					<< std::endl;
			}
			continue;
		}

		if (line.empty()) {
			if (isPreviousLineEmpty) {
				hasAnyError = true;
				errorStr
					<< "line[" << (lineNum - 1) << "]: "
					<< "Empty line appeared other than the last line"
					<< std::endl;
			}
			isPreviousLineEmpty = true;
			continue;
		} else if (isPreviousLineEmpty) {
			hasAnyError = true;
			errorStr
				<< "line[" << (lineNum - 1) << "]: "
				<< "Empty line appeared other than the last line"
				<< std::endl;
			isPreviousLineEmpty = false;
		}

		try {
			PriceHistory priceHistory = PriceHistory::fromCsvLine(line);
			if (db._priceHistory.size() > 0) {
				// 日付の重複もこれで検査できる
				if (priceHistory.date <= db._priceHistory.back().date) {
					hasAnyError = true;
					errorStr
						<< "line[" << lineNum << "]: "
						<< "Invalid date order: " << priceHistory.date
						<< std::endl;
				}
			}
			if (!hasAnyError) {
				db._priceHistory.push_back(priceHistory);
			}
		} catch (std::exception &e) {
			hasAnyError = true;
			errorStr
				<< "line[" << lineNum << "]: "
				<< e.what()
				<< std::endl;
		}
	}
	if (lineNum == 0) {
		throw std::invalid_argument("Empty file");
	}

	if (hasAnyError) {
		throw std::invalid_argument("The following error occurred\n" + errorStr.str());
	}

	return db;
}

#pragma region PriceHistory
BitcoinExchange::PriceHistory::PriceHistory(
	const std::string &date,
	double price
) : date(date),
		price(price)
{
}
BitcoinExchange::PriceHistory::PriceHistory(
	const PriceHistory &src
) : date(src.date),
		price(src.price)
{
}
BitcoinExchange::PriceHistory &BitcoinExchange::PriceHistory::operator=(
	const PriceHistory &src
)
{
	if (this == &src)
		return *this;

	this->date = src.date;
	this->price = src.price;

	return *this;
}

#define COMMA_POS (sizeof(DATE_FORMAT) - 1)
BitcoinExchange::PriceHistory BitcoinExchange::PriceHistory::fromCsvLine(
	const std::string &line
)
{
	if (line.empty())
		throw std::invalid_argument("Empty line");
	if (line.length() < sizeof(DATE_FORMAT) + 1)
		throw std::invalid_argument("Invalid line format (line too short)");

	if (line[COMMA_POS] != ',')
		throw std::invalid_argument("Invalid line format");

	std::string date = line.substr(0, sizeof(DATE_FORMAT) - 1);
	std::string priceStr = line.substr(COMMA_POS + 1);
	if (!isValidDateStr(date))
		throw std::invalid_argument("Invalid date format");
	if (!isValidPositiveNumStr(priceStr))
		throw std::invalid_argument("Invalid price format");

	double price = std::atof(priceStr.c_str());
	if (std::isnan(price))
		throw std::invalid_argument("Invalid price format");

	return PriceHistory(date, price);
}
#pragma endregion PriceHistory
