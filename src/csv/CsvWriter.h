#pragma once

#include "csv/CsvTable.h"
#include "Utils.h"

namespace hokee
{

class CsvWriter
{
  public:
    explicit CsvWriter() = delete;
    virtual ~CsvWriter() = delete;

    CsvWriter(const CsvWriter&) = delete;
    CsvWriter& operator=(const CsvWriter&) = delete;
    CsvWriter(CsvWriter&&) = delete;
    CsvWriter& operator=(CsvWriter&&) = delete;

    static void Write(const fs::path& file, const CsvTable& data);
};

} // namespace hokee