#include "Utils.h"
#include "HtmlGenerator.h"
#include "InternalException.h"

#include <fmt/format.h>

#include <cstdio>
#include <cstdlib>

#include <array>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace hokee::Utils
{
namespace
{
std::mutex _lastMessageMutex{};
std::vector<std::string> _lastMessages{};
int _uniqueId{0};

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
            if (closeTagPos != std::string::npos)
            {
                result = result.erase(openTagPos, closeTagPos - openTagPos + 1);
            }
        }
    } while (openTagPos != std::string::npos);

    return result;
}
} // namespace

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

void EditFile(const fs::path& file, const std::string& editor)
{
    std::string cmd = fmt::format("{} \"{}\"", editor, fs::absolute(file).string());
    Utils::PrintInfo(fmt::format("Run: {}", cmd));
    if (std::system(cmd.c_str()) < 0)
    {
        throw UserException(fmt::format("Could not open editor: {}", cmd));
    }
}

void OpenFolder(const fs::path& folder, const std::string& explorer)
{
    std::string cmd = fmt::format("{} \"{}\"", explorer, fs::absolute(folder).string());
    Utils::PrintInfo(fmt::format("Run: {}", cmd));
    if (std::system(cmd.c_str()) < 0)
    {
        throw UserException(fmt::format("Could not open folder: {}", cmd));
    }
}

void ResetIdGenerator()
{
    _uniqueId = 0;
}

int GenerateId()
{
    return ++_uniqueId;
}

void PrintTrace(std::string_view msg)
{
    std::cout << "TRACE: " << DropXmlTags(msg) << std::endl;
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

bool AskYesNoQuestion(const std::string& question, bool defaultYes, bool batchMode)
{
    if (batchMode)
    {
        return defaultYes;
    }

    char answer;
    if (defaultYes)
    {
        answer = 'Y';
        Utils::PrintInfo(fmt::format("{} [Y/n]", question));
    }
    else
    {
        answer = 'N';
        Utils::PrintInfo(fmt::format("{} [y/N]", question));
    }

    std::string input;
    std::getline(std::cin, input);
    if (!input.empty())
    {
        std::istringstream stream(input);
        stream >> answer;
    }
    return defaultYes ? (answer == 'Y' || answer == 'y') : (answer == 'N' || answer == 'n');
}

void RunAsync(const std::string& cmd)
{
    std::thread browserThread([=] {
        try
        {
            if (std::system(cmd.c_str()) < 0)
            {
                throw UserException("Could not open result.");
            }
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

std::string RunSync(const std::string& cmd)
{
    std::array<char, 256> buffer;
    std::string result;

#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
#endif
    if (!pipe)
    {
        Utils::PrintError(fmt::format("popen({}) failed!", cmd));
        return "";
    }
    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

std::string GetEnv(const std::string& name)
{
    const char* pValue = std::getenv(name.c_str());
    return pValue == nullptr ? "" : std::string(pValue);
}

fs::path GetHomePath()
{
#if defined(unix) || defined(__APPLE__)
    return Utils::GetEnv("HOME");
#elif defined(_WIN32)
    return fs::path(Utils::GetEnv("HOMEDRIVE") + Utils::GetEnv("HOMEPATH")) / "Documents";
#else
#error Unsupported OS
#endif
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

} // namespace hokee::Utils