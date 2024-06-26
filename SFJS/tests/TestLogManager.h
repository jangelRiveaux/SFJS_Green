#pragma once
#include <string>
#include <vector>
#ifndef TestLogManager_H
#define TestLogManager_H

class TestLogManager
{
private:
	std::string file_addrs;
	std::string table_format;
	std::string latex_format;
	bool consoleOutput = true;
	std::string string_format;

public:
	TestLogManager(std::string _file_addrs, std::string _string_format, bool _consoleOutput = true);
	~TestLogManager();
	void PrintLatexFormat(int args_cnt, ...);
	void PrintResults(int args_cnt, ...);
	void PrintMessage(std::string message, bool _consoleOutput = true);
	void PrintConfiguration();
	void SetFile(std::string new_file_addrs) { file_addrs = new_file_addrs; }
	void SetConsoleOutput(bool consoleOutputValue) { consoleOutput = consoleOutputValue; }
	void PrintHeader(std::vector<std::string> names);
	void PrintLatexHeader(std::vector<std::string> names);
	void SetPrintFormat(std::string _string_format);
	void SetLatexFormat(std::string _latex_format);
};


TestLogManager::TestLogManager(std::string _file_addrs, std::string _string_format, bool _consoleOutput):
    file_addrs(_file_addrs), consoleOutput(_consoleOutput)
{
    SetPrintFormat(_string_format);
    //Print the date and time of the experiments
}


TestLogManager::~TestLogManager()
{
}

void TestLogManager::PrintLatexFormat(int args_cnt, ...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, args_cnt);
    char str[300];
    snprintf(str, 300,latex_format.c_str(), arguments);
    FILE *f = fopen(file_addrs.c_str(), "a");
    if (f == 0)
    {
        fprintf(f, "%s", str);
        if(fclose(f) != 0)
            printf("error closign file");
    }
}

void TestLogManager::PrintResults(int args_cnt,...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, args_cnt);
    char str[300];
    snprintf(str, 300,string_format.c_str(), arguments);
    
    va_end(arguments);
    FILE *f = fopen(file_addrs.c_str(), "a");
    
    if (consoleOutput)
    {
        printf("%s", str);
    }
    if (f != 0)
    {
        fprintf(f, "%s", str);
        int e = fclose(f);
        if (e != 0)
            printf("error: %d closign file",e);
    }
}

void TestLogManager::PrintMessage(std::string message, bool _consoleOutput)
{
    FILE *f = fopen(file_addrs.c_str(), "a");
    if (f != 0)
    {
        fprintf(f, "%s", message.c_str());
        fclose(f);
    }
    if (consoleOutput)
    {
        printf("%s", message.c_str());
    }
}

void TestLogManager::PrintConfiguration()
{
    throw;
}

void TestLogManager::PrintHeader(std::vector<std::string> names)
{
    
    printf("%s", file_addrs.c_str());
    FILE *f= fopen(file_addrs.c_str(), "a");
    for (size_t i = 0; i < names.size(); i++)
    {
        if (f != 0)
        {
            fprintf(f, "%s\t", names[i].c_str());
        }
        if (consoleOutput)
        {
            printf("%s\t", names[i].c_str());
        }
    }
    if (consoleOutput)
    {
        printf("\n");
    }
    if (f != 0)
    {
        fprintf(f, "\n");
    }
    if(f != 0) fclose(f);
}

void TestLogManager::PrintLatexHeader(std::vector<std::string> names)
{
    FILE *f = fopen(file_addrs.c_str(), "a");
    for (size_t i = 0; i < names.size(); i++)
    {
        if (f != 0)
        {
            fprintf(f, "%s&", names[i].c_str());
        }
    }
    if (f != 0) fclose(f);
}

void TestLogManager::SetPrintFormat(std::string _string_format)
{
    string_format = _string_format +  "\n";
    latex_format = _string_format + "\\\\\n";
    while (true)
    {
        size_t pos = latex_format.find("\t", 1);
        if (pos == std::string::npos) break;
        latex_format = latex_format.replace(pos, 1, "&");
    }
}

void TestLogManager::SetLatexFormat(std::string _latex_format)
{
    latex_format = _latex_format;
}



#endif
