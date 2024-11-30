#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

#include "./BitcoinExchange.hpp"

#define DB_FILE_PATH "data.csv"
#define INPUT_FILE_SEPARATOR " | "
#define INPUT_FILE_HEADER "date | value"

// 今回は「両端は含まない」として考える
#define INPUT_VALUE_MIN 0
#define INPUT_VALUE_MAX 1000
#define INPUT_VALUE_STR_MAX_LEN 4

#define MINIMUM_LINE_FORMAT DATE_FORMAT INPUT_FILE_SEPARATOR "0"
static void processOneLine(
	const BitcoinExchange &db,
	const std::string &line
)
{
	if (line.length() < (sizeof(MINIMUM_LINE_FORMAT) - 1)) {
		std::cout << "Error: Invalid line format (line too short)" << std::endl;
		return;
	}

	std::string date = line.substr(0, sizeof(DATE_FORMAT) - 1);
	std::string separator = line.substr(sizeof(DATE_FORMAT) - 1, sizeof(INPUT_FILE_SEPARATOR) - 1);
	std::string valueStr = line.substr(sizeof(DATE_FORMAT) + sizeof(INPUT_FILE_SEPARATOR) - 2);
	if (separator != INPUT_FILE_SEPARATOR) {
		std::cout
			<< "Error: Invalid line format: " << line
			<< std::endl;
		return;
	}
	if (!isValidDateStr(date)) {
		std::cout
			<< "Error: Invalid date format: " << date
			<< std::endl;
		return;
	}
	if (!isValidPositiveNumStr(valueStr)) {
		std::cout
			<< "Error: Invalid value format: " << valueStr
			<< std::endl;
		return;
	}

	double value = std::atof(valueStr.c_str());
	if (value <= INPUT_VALUE_MIN || INPUT_VALUE_MAX <= value) {
		std::cout
			<< "Error: Invalid value: " << value
			<< std::endl;
		return;
	}

	double latestPrice = db.getLatestPriceAt(date);
	if (latestPrice == 0) {
		// 最新価格が0の場合は、データが存在しないとみなす (価値0のものを取引することはできないため)
		std::cout
			<< "Error: No price data for the date: " << date
			<< std::endl;
		return;
	}
	std::cout
		<< date
		<< " => "
		<< value
		<< " = " << latestPrice * value
		<< std::endl;
}

int main(
	int argc,
	const char **argv
)
{
	if (argc != 2) {
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " <input file path>"
			<< std::endl;
		return 1;
	}

	BitcoinExchange db;
	try {
		db = BitcoinExchange::loadFromFile(DB_FILE_PATH);
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	try {
		std::ifstream inputFile(argv[1]);
		if (!inputFile) {
			throw std::runtime_error("Failed to open file: " + std::string(argv[1]));
		}

		std::string line;
		bool isHeader = true;
		bool isPreviousLineEmpty = false;
		while (std::getline(inputFile, line)) {
			if (isHeader) {
				if (line != INPUT_FILE_HEADER) {
					throw std::invalid_argument("Invalid header: " + line);
				}
				isHeader = false;
				continue;
			}
			if (isPreviousLineEmpty) {
				// ヘッダ以外のエラーは、ログのためstdoutに出力
				std::cout
					<< "Error: Empty line appeared other than the last line"
					<< std::endl;
			}
			if (line.empty()) {
				isPreviousLineEmpty = true;
				continue;
			}
			isPreviousLineEmpty = false;
			processOneLine(db, line);
		}

		if (isHeader) {
			throw std::invalid_argument("Empty file");
		}
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	// ヘッダ以外の各行のエラーがあったとしても、異常終了とはしない
	return 0;
}
