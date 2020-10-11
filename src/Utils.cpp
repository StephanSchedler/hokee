#include "Utils.h"
#include "InternalException.h"
#include "hokee.h"
#include "html/HtmlGenerator.h"

#include <fmt/format.h>

#include <cstdio>
#include <cstdlib>

#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>


namespace hokee::Utils
{
namespace
{
std::mutex _lastMessageMutex{};
std::vector<std::string> _lastMessages{};
int _uniqueId{0};
bool _verbose{false};

const std::string DropXmlTags(std::string_view msg)
{
    std::string result(msg);

    size_t openTagPos = std::string::npos;
    do
    {
        openTagPos = result.find('<');
        if (openTagPos != std::string::npos)
        {
            size_t closeTagPos = result.find('>', openTagPos);
            if (closeTagPos == std::string::npos)
            {
                break;
            }
            result = result.erase(openTagPos, closeTagPos - openTagPos + 1);
        }
    } while (openTagPos != std::string::npos);

    return result;
}

std::string ReadFileObfuscated(const fs::path& path, bool obfuscate)
{
    char c;
    std::stringstream output;
    std::ifstream inputStream(path, std::ios::binary);

    char cipher[256];
    for (int i = 0; i < 256; ++i)
    {
        cipher[i] = static_cast<char>(i);
        if (i >= 48 && i <= 57)
        {
            cipher[i] = '1';
        }
        if ((i >= 65 && i <= 90) || (i >= 97 && i <= 122))
        {
            cipher[i] = (rand() % 26) + 65; // Random character A-Z
        }
    }

    while (inputStream.get(c))
    {
        if (obfuscate)
        {
            c = cipher[static_cast<unsigned char>(c)];
        }
        output << c;
    }
    return output.str();
}
} // namespace

std::string GenerateTimestamp()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", timeinfo);
    return std::string(buffer);
}

const std::vector<std::string> GetLastMessages()
{
    std::scoped_lock lock(_lastMessageMutex);
    const std::vector<std::string> result = _lastMessages;
    return result;
}

std::string ToLower(const std::string& str)
{
    std::string result = str;
    for (char& c : result)
    {
        c = static_cast<char>(std::tolower(c));
    }
    return result;
}

std::string ToUpper(const std::string& str)
{
    std::string result = str;
    for (char& c : result)
    {
        c = static_cast<char>(std::toupper(c));
    }
    return result;
}

std::vector<std::string> SplitLine(const std::string& s, char delimiter, bool hasTrailingDelimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    // Check for last cell empty if !HasTrailingDelimiter
    if (!hasTrailingDelimiter && !s.empty() && s[s.size() - 1] == delimiter)
    {
        tokens.push_back("");
    }
    return tokens;
}

bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing)
{
    if (missing.size() > original.size())
    {
        throw UserException(
            fmt::format("Modified cell \"{}\" must be shorter than original cell \"{}\"!", missing, original));
    }

    size_t s = 0;
    while (true)
    {
        if (s < missing.size() && missing[s] == original[s])
        {
            ++s;
        }
        else
        {
            break;
        }
    }

    size_t eo = original.size() - 1;
    size_t em = missing.size() - 1;
    while (em != ~0ull && missing[em] == original[eo])
    {
        --eo;
        --em;
    }

    extracted = original.substr(s, eo - s + 1);
    return !extracted.empty();
}

void ResetIdGenerator()
{
    _uniqueId = 0;
}

int GenerateId()
{
    return ++_uniqueId;
}

void SetVerbose(bool verbose)
{
    _verbose = verbose;
}

void PrintTrace(std::string_view msg)
{
    if (_verbose)
    {
        std::cout << "TRACE: " << DropXmlTags(msg) << std::endl;
    }
}

void PrintInfo(std::string_view msg)
{
    std::cerr << DropXmlTags(msg) << std::endl;

    std::scoped_lock lock(_lastMessageMutex);
    _lastMessages.emplace(_lastMessages.begin(), msg);
    while (_lastMessages.size() > 8)
    {
        _lastMessages.pop_back();
    }
}

void PrintWarning(std::string_view msg)
{
    std::cerr << fmt::format("WARN:  {}", DropXmlTags(msg)) << std::endl;

    std::scoped_lock lock(_lastMessageMutex);
    _lastMessages.emplace(_lastMessages.begin(), msg);
    while (_lastMessages.size() > 8)
    {
        _lastMessages.pop_back();
    }
}

void PrintError(std::string_view msg)
{
    std::cerr << fmt::format("ERROR: {}", DropXmlTags(msg)) << std::endl;

    std::scoped_lock lock(_lastMessageMutex);
    _lastMessages.emplace(_lastMessages.begin(), msg);
    while (_lastMessages.size() > 8)
    {
        _lastMessages.pop_back();
    }
}

bool AskYesNoQuestion(const std::string& question)
{
    char answer = 'Y';
    Utils::PrintInfo(fmt::format("{} [Y/n]", question));

    std::string input;
    std::getline(std::cin, input);
    if (!input.empty())
    {
        std::istringstream stream(input);
        stream >> answer;
    }
    return answer == 'Y' || answer == 'y';
}

void RunAsync(const std::string& cmd, const std::vector<std::string>& args)
{
    std::thread browserThread([=] {
        try
        {
            Utils::RunSync(cmd.c_str(), args);
        }
        catch (const UserException& e)
        {
            Utils::TerminationHandler(e);
        }
        catch (const std::exception& e)
        {
            Utils::TerminationHandler(e);
        }
        catch (...)
        {
            Utils::TerminationHandler();
        }
    });
    browserThread.detach();
}

void RunSync(const std::string& cmd, const std::vector<std::string>& args)
{
    std::string command = cmd;
    for (const auto& arg : args)
    {
        command.append(fmt::format(" \"{}\"", arg));
    }

    Utils::PrintInfo(fmt::format("Run command '{}'", command));
    int exitCode = std::system(command.c_str());
    if (exitCode != 0)
    {
        Utils::PrintWarning(fmt::format("Command '{}' returned non-zero exit code {}", command, exitCode));
    }
}

std::string GetEnv(const std::string& name)
{
    const char* pValue = std::getenv(name.c_str());
    return pValue == nullptr ? "" : std::string(pValue);
}

fs::path GetHomeDir()
{
#if defined(unix) || defined(__APPLE__)
    return Utils::GetEnv("HOME");
#elif defined(_WIN32)
    return fs::path(Utils::GetEnv("HOMEDRIVE") + Utils::GetEnv("HOMEPATH")) / "Documents";
#else
#error Unsupported OS
#endif
}

fs::path GetTempDir()
{
    // Detect Temporary Directory
#ifdef _MSC_VER
    static fs::path tempDir = fs::path(Utils::GetEnv("TEMP")) / "hokee";
#elif __APPLE__
    static fs::path tempDir = fs::path("/") / "var" / "tmp" / "hokee";
#else
    static fs::path tempDir = fs::path("/") / "tmp" / "hokee";
#endif
    if (!fs::exists(tempDir))
    {
        fs::create_directories(tempDir);
    }
    return tempDir;
}

void TerminationHandler()
{
    Utils::PrintError("!!! Unhandled exception !!!");

    // Try to decode exception message
    try
    {
        if (std::current_exception())
        {
            std::rethrow_exception(std::current_exception());
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "=> " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "=> ???" << std::endl;
    }

    std::cerr << std::endl;
    std::abort();
}

void TerminationHandler(const std::exception& e)
{
    Utils::PrintError("!!! Caught exception !!!");
    std::cerr << "=> " << e.what() << std::endl;
    std::cerr << std::endl;
    std::abort();
}

void TerminationHandler(const UserException& e)
{
    Utils::PrintError(e.what());
    std::abort();
}

std::string EscapeHtml(std::string text)
{
    std::stringstream buffer{};
    for (auto& character : text)
    {
        switch (character)
        {
            case '&':
                buffer << "&amp;";
                break;
            case '\"':
                buffer << "&quot;";
                break;
            case '\'':
                buffer << "&apos;";
                break;
            case '<':
                buffer << "&lt;";
                break;
            case '>':
                buffer << "&gt;";
                break;
            default:
                buffer << character;
                break;
        }
    }
    return buffer.str();
}

std::string GenerateSupportMail(const fs::path& ruleSetFile, const fs::path& inputDir)
{
    std::stringstream mail{};
    mail << "=============================================" << std::endl;
    mail << fmt::format("Problem report for hokee {}", PROJECT_VERSION) << std::endl;
    mail << "=============================================" << std::endl;
    mail << std::endl;
    mail << "Problem Description:" << std::endl;
    mail << std::endl;
    mail << "  >>> ADD SHORT PROBLEM DESCRIPTION <<<" << std::endl;
    mail << std::endl;
    mail << "Further Infos:" << std::endl;
    mail << std::endl;
#ifdef _MSC_VER
    mail << "  Windows >>> ADD OS VERSION <<<" << std::endl;
#elif __APPLE__
    mail << "  MacOS >>> ADD OS VERSION <<<" << std::endl;
#else
    mail << "  Linux >>> ADD DISTRO & VERSION <<<" << std::endl;
#endif
    mail << std::endl;

    mail << "=============================================" << std::endl;
    mail << ruleSetFile << std::endl;
    mail << "=============================================" << std::endl;
    mail << ReadFileObfuscated(ruleSetFile, true) << std::endl;

    for (const auto& file : fs::recursive_directory_iterator(inputDir))
    {
        if (fs::is_regular_file(file.path()))
        {
            mail << "=============================================" << std::endl;
            mail << file.path() << std::endl;
            mail << "=============================================" << std::endl;
            mail << ReadFileObfuscated(file.path(), file.path().extension() != ".ini") << std::endl;
        }
    }

    return mail.str();
}

std::string ReadFileContent(const fs::path& file)
{
    std::ifstream ifstream(file, std::ios::binary);
    std::stringstream sstream{};
    sstream << ifstream.rdbuf();
    return sstream.str();
}

void WriteFileContent(const fs::path& file, const std::string& content)
{
    std::ofstream ofstream(file, std::ios::binary);
    ofstream << content;
}

bool CompareFiles(const fs::path& file1, const fs::path& file2)
{
    char c1, c2;
    int line = 1;
    int col = 0;
    std::ifstream fileStream1(file1);
    std::ifstream fileStream2(file2);
    while (fileStream1.get(c1) && fileStream2.get(c2))
    {
        col++;
        if (c1 == '\n')
        {
            line++;
            col = 0;
        }
        if (c1 != c2)
        {
            Utils::PrintError(fmt::format("Files {}, {} differ in line {}:{}. (expected: '{}', found: '{}')",
                                          file1.string(), file2.string(), line, col, c1, c2));
            return false;
        }
    }
    if (!fileStream1.eof())
    {
        Utils::PrintError(fmt::format("File {} is larger than {}.", file1.string(), file2.string()));
        return false;
    }
    if (!fileStream2.get(c2).eof())
    {
        Utils::PrintError(fmt::format("File {} is smaller than {}.", file1.string(), file2.string()));
        return false;
    }

    return true;
}
} // namespace hokee::Utils