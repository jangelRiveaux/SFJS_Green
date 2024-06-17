#include <cstdarg>
#include "TestLogManager.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

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
	char str[1000];
	vsprintf_s(str,1000, latex_format.c_str(), arguments);
	FILE *f;
	errno_t err = fopen_s(&f, file_addrs.c_str(), "a");
	if (err == 0)
	{
		fprintf(f, str);
		if(fclose(f) != 0)
			printf("error closign file");
	}
}

void TestLogManager::PrintResults(int args_cnt,...)
{
	va_list arguments;                     // A place to store the list of arguments
	va_start(arguments, args_cnt);
	char str[1000];
	vsprintf_s(str,1000, string_format.c_str(), arguments);
	FILE *f;
	
	for (int i = 0; i < 5; i++)
	{
		errno_t err = fopen_s(&f, file_addrs.c_str(), "a");
		if (err == 0)
		{
			fprintf(f, str);
			int e = fclose(f);
			if (e != 0)
				printf("error: %d closign file", e);
			if (consoleOutput)
			{
				printf(str);
			}
			return;
		}
		else {
			Sleep(2000);
		}		
	}
}

void TestLogManager::PrintMessage(std::string message, bool _consoleOutput)
{
	FILE *f;
	for (int i = 0; i <1; i++)
	{
		errno_t err = fopen_s(&f, file_addrs.c_str(), "a");
		if (err == 0)
		{
			fprintf_s(f, message.c_str());
			fclose(f);
			if (consoleOutput)
			{
				printf_s(message.c_str());
			}
		}
		else {
			Sleep(2000);
		}		
	}
}

void TestLogManager::PrintConfiguration()
{
	throw;
}

void TestLogManager::PrintHeader(std::vector<std::string> names)
{
	FILE *f;
	printf(file_addrs.c_str());
	errno_t err = fopen_s(&f, file_addrs.c_str(), "a");
	for (size_t i = 0; i < names.size(); i++)
	{
		if (err == 0)
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
	if (err == 0)
	{
		fprintf(f, "\n");
	}
	if(err == 0) fclose(f);
}

void TestLogManager::PrintLatexHeader(std::vector<std::string> names)
{
	FILE *f;
	errno_t err = fopen_s(&f, file_addrs.c_str(), "a");
	for (size_t i = 0; i < names.size(); i++)
	{
		if (err == 0)
		{
			fprintf(f, "%s&", names[i].c_str());
		}
	}
	if (err == 0) fclose(f);
}

void TestLogManager::SetPrintFormat(std::string _string_format)
{	
	string_format = _string_format +  "\n";
	latex_format = _string_format + "\\\\\n";
	while (true)
	{
		size_t pos = latex_format.find("\t", 1);
		if (pos == string::npos) break;
		latex_format = latex_format.replace(pos, 1, "&");
	}
}

void TestLogManager::SetLatexFormat(std::string _latex_format)
{
	latex_format = _latex_format;	
}