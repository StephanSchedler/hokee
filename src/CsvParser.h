#pragma once

#include "CsvTable.h"
#include "CsvFormat.h"
#include "Utils.h"

#include <fstream>
#include <memory>
#include <type_traits>

namespace homebanking
{

class CsvParser
{
    void ValidateValue(const std::string value);

    int _lineCounter = 0;
    fs::path _file = {};
    std::ifstream _ifstream = {};
    CsvFormat _format = {};
    std::string _accountOwner = {};

    std::vector<std::string> SplitLine(const std::string& s, const CsvFormat& format);
    void AssignValue(std::string& value, std::vector<std::string> cells, size_t id);

    bool GetLine(CsvRowShared& item);
    bool ParseLine(CsvRowShared& item);

  public:
    CsvParser(const fs::path& file, const CsvFormat& format, const std::string accountOwner = "");
    ~CsvParser() = default;

    CsvParser(const CsvParser&) = delete;
    CsvParser& operator=(const CsvParser&) = delete;
    CsvParser(CsvParser&&) = delete;
    CsvParser& operator=(CsvParser&&) = delete;

    void Load(CsvTable& csvData);
};

} // namespace homebanking