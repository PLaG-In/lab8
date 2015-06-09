#include "stdafx.h"

bool FlagBrackets;
bool flagComment;

using namespace std;

string delete_line;
vector <__int64> vec_del;
__int64 g_freq, g_showTime, g_findTime, g_summVecTime; // глобальная переменная

void Init()
{
	LARGE_INTEGER s;
	QueryPerformanceFrequency(&s);
	g_freq = s.QuadPart;
}

__int64 GetTicks()
{
	LARGE_INTEGER s;
	QueryPerformanceCounter(&s);
	return s.QuadPart;
}


__int64 GetMicroTickCount()		// время в mcs
{
	return (GetTicks() * 1000000 / g_freq);
}

__int64 SummVec(vector<__int64> vect)
{
	__int64 begin = GetMicroTickCount();
	__int64 summ = 0;
	for (size_t i = 0; i < vect.size(); i++)
	{
		summ = summ + vect[i];
	}
	g_summVecTime = GetMicroTickCount() - begin;
	return summ;
}

void fillTestsFirst(vector<string> &testInData, vector<string> &testOutData)
{
	testInData.push_back("alert(\"Hello /* abc */ \"); // Comment");
	testInData.push_back("alert(\" // Test \"); // Comment");
	testInData.push_back("// 12345 \" qwerty \" ");
	testInData.push_back(" test(); // abc");
	testInData.push_back("/* 12345 */");
	testInData.push_back(" a = 2; /* line 1");
	testInData.push_back(" line 2 ");
	testInData.push_back(" line 3 */ rest();");
	testInData.push_back("line /*");
	testInData.push_back("line /*");
	testInData.push_back("line /*");

	testOutData.push_back("alert(\"Hello /* abc */ \"); ");
	testOutData.push_back("alert(\" // Test \"); ");
	testOutData.push_back("");
	testOutData.push_back(" test(); ");
	testOutData.push_back("");
	testOutData.push_back(" a = 2; ");
	testOutData.push_back("");
	testOutData.push_back(" rest();");
	testOutData.push_back("line ");
	testOutData.push_back("");
	testOutData.push_back("");
}
map<string, int> FindCommonWord(string line)
{
	__int64 begin = GetMicroTickCount();
	unsigned int count = 0;
	int symbol;
	string word = "";
	map<string, int> Dictionary;
	do
	{
		symbol = line[count];
		if ((symbol < 65 || symbol > 122) || (symbol > 90 && symbol < 97))
		{
			if (word != "")
			{
				map<string, int>::iterator it = Dictionary.find(word);
				if (it != Dictionary.end())
				{
					Dictionary[word]++;
				}
				else
				{
					Dictionary[word] = 1;
				}
			}
			word = "";
		}
		else
		{
			word = word + line[count];
		}
		count++;
	} while (count < line.size());
	g_findTime = GetMicroTickCount() - begin;
	return Dictionary;
}

char* ShowDictionary(map<string, int> Dictionary)
{
	__int64 begin = GetMicroTickCount();
	int Value = 0;
	string Key;
	char* Result;
	map<string, int>::iterator it;
	map<string, int>::iterator itEnd = Dictionary.end();

	it = Dictionary.begin();

	while (it != itEnd)
	{
		if (Value < it->second)
		{
			Value = it->second;
			Key = it->first;
		}
		it++;
	}
	Result = new char[Key.length() + 1];
	strcpy_s(Result, Key.length() + 1, Key.c_str());
	g_showTime = GetMicroTickCount() - begin;
	return Result;
}

string DeletedComment(string str)
{
	__int64 begin = GetMicroTickCount();
	string t;
	char* CopySymbol;
	char* CommentSymbol = CopySymbol = strdup(str.c_str());

	for (size_t i = 0; i < str.length(); ++i) 
	{
		*CopySymbol = *CommentSymbol;

		if ((*CommentSymbol == '(') && (*(CommentSymbol + 1) == '"'))
		{
			FlagBrackets = true;
			t += *CopySymbol;
			*CopySymbol++;
		}
		else if ((*(CommentSymbol + 1) == ')') && ((*CommentSymbol == '"')))
		{
			FlagBrackets = false;
			t += *CopySymbol;
			*CopySymbol++;
		}
		else if ((*CommentSymbol == '/') && (*(CommentSymbol + 1) == '/') && FlagBrackets != true) // удаляем однострочные коментарии
		{
			delete_line = delete_line + str;
			for (CommentSymbol += 2; (*CommentSymbol) && (*CommentSymbol != '\n'); *CommentSymbol++);
			if (!*CommentSymbol)
			{
				*CopySymbol = *CommentSymbol;
				break;
			}

		}
		else if ((*CommentSymbol == '/') && (*(CommentSymbol + 1) == '*') && FlagBrackets != true) // удаляем многострочные коментарии
		{
			flagComment = true;
		}
		else if (flagComment == true)
		{
			delete_line = delete_line + str;

			for (CommentSymbol += 2; *CommentSymbol; *CommentSymbol++)
			{
				if ((*CommentSymbol == '*') && (*(CommentSymbol + 1) == '/'))
				{
					flagComment = false;
					break;
				}
			}
			if (!*CommentSymbol)
			{
				*CopySymbol = *CommentSymbol;
				break;
			}
			*CommentSymbol++;
		}
		else
		{
			t += *CopySymbol;
			*CopySymbol++;
		}
		*CommentSymbol++;
	}
	/*cout << begin;*/
	vec_del.push_back(GetMicroTickCount() - begin);
	return t;
}

void unit_test_for_delete_comment()
{
	vector<string> tests;
	tests.reserve(9);
	tests.push_back(DeletedComment("normal string"));
	tests.push_back(DeletedComment("line comment here ===>//try"));
	tests.push_back(DeletedComment("//must show nothing"));
	tests.push_back(DeletedComment("comment===>/*jgbfkjg*/"));
	tests.push_back(DeletedComment("/*FIFtH*/<===comment"));
	tests.push_back(DeletedComment("comment with lcomment inside===>/*jnfjgfgkm//nkgmfklg*/"));
	tests.push_back(DeletedComment("\t(\" // Test \")jkkk"));
	tests.push_back(DeletedComment(""));
	tests.push_back(DeletedComment("lc===>//j/*j*/q/*jj*/"));

	assert(!strcmp(tests[0].c_str(), "normal string"));
	assert(!strcmp(tests[1].c_str(), "line comment here ===>"));
	assert(!strcmp(tests[2].c_str(), ""));
	assert(!strcmp(tests[3].c_str(), "comment===>"));
	assert(!strcmp(tests[4].c_str(), "<===comment"));
	assert(!strcmp(tests[5].c_str(), "comment with lcomment inside===>"));
	assert(!strcmp(tests[6].c_str(), "\t(\" // Test \")jkkk"));
	assert(!strcmp(tests[7].c_str(), ""));
	assert(!strcmp(tests[8].c_str(), "lc===>"));
	printf("Success\n");
}

bool unitTest(vector<string> & testInData, vector<string> const & testOutData)
{
	vector<string> testInResult;
	for (auto elem : testInData)
	{
		testInResult.push_back(DeletedComment(elem));
	}
	for (size_t i = 0; i < testInData.size(); ++i)
	{
		if (testInResult[i] != testOutData[i])
		{
			cout << "Error";
			return false;
		}
	}
	cout << "ok" << endl;
	return true;
}

int main(int argc, char* argv[])
{
	Init();
	unit_test_for_delete_comment();
	vector<string> testInData;
	//vector<string> testOutData;
	//
	//fillTestsFirst(testInData, testOutData);
	//if (!unitTest(testInData, testOutData))
	//{
	//	return 1;
	//}

	__int64 begin = GetMicroTickCount();
	if (argc != 3) {
		printf("Usage: lab1 [input file] [output file] \n");
		return 0;
	}
	string line;
	string text;
	string name_inp = argv[1];
	string name_out = argv[2];
	char *cstr;
	vector <string> a; // куда будем считывать строки
	FlagBrackets = false;
	flagComment = false;
	ifstream input(name_inp);
	if (!input.is_open())
	{
		cout << "Error: can't load input file\n";
		system("pause");
		return 0;
	}
	ofstream output(name_out);
	if (!output.is_open())
	{
		cout << "Error: cannot create output file\n";
		system("pause");
		return 0;
	}
	if (input.is_open())
	{
		while (!input.eof())
		{
			getline(input, line);
			text += line;
			text += "\n";
			a.push_back(DeletedComment(text));
			text = "";
		}
		input.close();
	}
	else
	{
		cout << "Unable to open file\n";
		system("pause");
		return 1;
	}
		
	// печать указателей в которых строки уже отфильтрованы
	
	for (size_t i = 0; i < a.size(); i++)
	{
		output << a[i].c_str();
	}

	map<string, int> MassOfWord;
	MassOfWord = FindCommonWord(delete_line);
	/*system("cls");*/
	/*__int64 summ = SummVec(vec_del);*/
	/*__int64 average = summ / vec_del.size();*/
	//for (size_t i = 0; i < vec_del.size(); i++)
	//{
	//	cout << vec_del[i] << " ";
	//}
	output.close();
	input.close();
	__int64 res = (GetMicroTickCount() - begin);
	cout << "The most used word is: " << ShowDictionary(MassOfWord) << endl;
	/*cout << "Total time of ShowDictionary function(mcs) : " << g_showTime << endl;
	cout << "Total time of FindCommonWord function(mcs) : " << g_findTime << endl;
	cout << "Total time of DeletedComment function(mcs): " << summ << endl;
	cout << "Average time of DeletedComment function(mcs): " << average << endl;
	cout << "Total time of SummVec function(mcs) : " << g_summVecTime << endl;
	cout << "Total time of program(mcs): " << res << endl;*/
	return 0;
}


