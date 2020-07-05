#include "csv/CsvSettings.h"
#include "csv/CsvParser.h"
#include "csv/CsvTable.h"
#include "UserException.h"

#include <fmt/format.h>

namespace hokee
{
CsvSettings::CsvSettings(const fs::path& file)
{
    if (!fs::exists(file))
    {
        throw UserException(fmt::format("File does not exist: {}", file.string()));
    }
    CsvParser csv (file, Utils::GetCsvFormat("Settings"));
    
    CsvTable csvData;
    csv.Load(csvData);

    for (auto& item : csvData)
    {
        settings[item->Type] = item->Value;
    }
}

} // namespace hokee