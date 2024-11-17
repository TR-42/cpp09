#pragma once

#include <string>
#include <vector>

#define DATE_FORMAT "YYYY-MM-DD"

bool isValidDateStr(const std::string &str);
bool isValidPositiveNumStr(const std::string &str);

class BitcoinExchange
{
 private:
	typedef struct PriceHistory {
		// BitcoinExchange 以外から触らせないため、コンテナ化は省略
		std::string date;
		double price;

		PriceHistory(const std::string &date, double price);
		PriceHistory(const PriceHistory &src);
		PriceHistory &operator=(const PriceHistory &src);

		static PriceHistory fromCsvLine(const std::string &line);
	} PriceHistory;

	std::vector<PriceHistory> _priceHistory;

 public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange &src);
	virtual ~BitcoinExchange();
	BitcoinExchange &operator=(const BitcoinExchange &src);

	double getLatestPriceAt(const std::string &date) const;

	static BitcoinExchange loadFromFile(const std::string &filePath);
};
