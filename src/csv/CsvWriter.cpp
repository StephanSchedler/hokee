#include "csv/CsvWriter.h"
#include "Utils.h"

#include <fmt/format.h>

#include <sstream>
#include <fstream>

namespace hokee
{

void CsvWriter::Write(const fs::path& file, const CsvTable& data)
{
    std::ofstream csvFile;

    Utils::PrintInfo(fmt::format("  Write CSV {}", file.string()));
    csvFile.open(file);
    
    for (const auto& line : data.GetCsvHeader())
    {
        csvFile << line << std::endl;
    }

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


    for (const auto& line : data.GetCsvHeader())
    {
        std::cerr << line << std::endl;
    }

    std::cerr << "Category;";
    std::cerr << "Payer/Payee;";
    std::cerr << "Description;";
    std::cerr << "Type;";
    std::cerr << "Date;";
    std::cerr << "Account;";
    std::cerr << "Value"; 
    std::cerr << std::endl;
    
    for (auto& row : data)
    {
        std::cerr << row->Category << ';';
        std::cerr << row->PayerPayee << ';';
        std::cerr << row->Description << ';';
        std::cerr << row->Type << ';';
        std::cerr << row->Date << ';';
        std::cerr << row->Account << ';';
        std::cerr << row->Value;
        std::cerr << std::endl;
    }


    csvFile.close();
}

} // namespace hokee