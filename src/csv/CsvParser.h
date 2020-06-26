#pragma once

#include "csv/CsvTable.h"
#include "csv/CsvFormat.h"
#include "Utils.h"

#include <fstream>
#include <memory>
#include <type_traits>

namespace hokee
{

class CsvParser
{
    void ValidateValue(const std::string value);

    size_t _lineCounter = 0;
    fs::path _file = {};
    std::ifstream _ifstream = {};
    CsvFormat _format = {};
    std::string _accountOwner = {};

    std::vector<std::string> SplitLine(const std::string& s, const CsvFormat& format);
    void AssignValue(std::string& value, std::vector<std::string> cells, size_t id);

    bool GetItem(CsvRowShared& item);
    bool ParseItem(CsvRowShared& item);

  public:
    CsvParser(const fs::path& file, const CsvFormat& format, const std::string accountOwner = "");
    ~CsvParser() = default;

    CsvParser(const CsvParser&) = delete;
    CsvParser& operator=(const CsvParser&) = delete;
    CsvParser(CsvParser&&) = delete;
    CsvParser& operator=(CsvParser&&) = delete;

    void Load(CsvTable& csvData);
};

} // namespace hokee