#include "CsvWriter.h"
#include "Utils.h"

#include <fmt/format.h>

#include <sstream>
#include <fstream>

namespace hokeeboo
{

void CsvWriter::Write(const fs::path& file, const CsvTable& data)
{
    std::ofstream csvFile;

    Utils::PrintInfo(fmt::format("  Write CSV {}", file.string()));
    csvFile.open(file);
    
    csvFile << "Category;";
    csvFile << "Payer/Payee;";
    csvFile << "Description;";
    csvFile << "Type;";
    csvFile << "Date;";
    csvFile << "Account;";
    csvFile << "Value"; 
    csvFile << std::endl;
    
    for (auto& row : data)
    {
        csvFile << row->Category << ';';
        csvFile << row->PayerPayee << ';';
        csvFile << row->Description << ';';
        csvFile << row->Type << ';';
        csvFile << row->Date << ';';
        csvFile << row->Account << ';';
        csvFile << row->Value;
        csvFile << std::endl;
    }

    csvFile.close();
}

} // namespace hokeeboo