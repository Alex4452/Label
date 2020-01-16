#ifdef LABELDLL_EXPORTS
#define LABELDLL_API __declspec(dllexport)
#else
#define LABELDLL_API __declspec(dllimport)
#endif

#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <map>

#define DEFAULT_NUM 29

using namespace std;

class LABELDLL_API Label;
class LABELDLL_API LabelProcess;
class LABELDLL_API HashTable;
class LABELDLL_API CreateLabel;

class LABELDLL_API HashTable
{
private:
	Label *table[DEFAULT_NUM];
	int hash(string str);

public:
	HashTable();
	~HashTable();

	void insert(Label* label);
	Label* find(string longForm);
};

class LABELDLL_API Process
{
public:
	Process();
	void clearString(string& str);
	void readLabel(string& temp, string input, int& startInd, int& i);
};

class LABELDLL_API ClientLabelProcess : public Process
{
public:
	ClientLabelProcess(ifstream* labelFile, vector<array<vector<Label*>, 3>>& clientsLabel, array<HashTable*, 3> tables);

private:
	void checkColonComma(string* temp, array<vector<Label*>, 3>& clientsLabel, int& count, bool checkComma);
	void loadLabel(ifstream* labelFile, vector<array<vector<Label*>, 3>>& clientsLabel);
	void parseLabel(char* str, array<vector<Label*>, 3>& clientLabel);

private:
	array<HashTable*, 3> tables;
};

class LABELDLL_API LabelProcess : public Process
{
public:
	LabelProcess();
	LabelProcess(ifstream* labelFile, HashTable& newT);

private:
	void checkColon(string* temp, Label* label, int& count);
	void loadLabel(ifstream* labelFile, HashTable& newT);
	Label* parseLabel(char* str);
};

class LABELDLL_API Label
{
public:
	Label() 
	{
		next = NULL;
	}

	Label(unsigned int numForm, string longForm, string shortForm) :
		numForm(numForm), longForm(longForm), shortForm(shortForm), parentName(NULL), next(NULL)
	{}

	Label(unsigned int numForm, string longForm, string shortForm, string (parentLabel)) :
		numForm(numForm), longForm(longForm), shortForm(shortForm), parentName(parentLabel), next(NULL)
	{}

	~Label()
	{
		if (this->next)
		{
			delete next;
		}
	}

	friend class HashTable;
	
	bool check(Label* labCheck);

	unsigned int numForm;
	string longForm;
	string shortForm;
	string parentName;
	Label* next;
};

class LABELDLL_API CompareLabel
{
public:
	CompareLabel();
	CompareLabel(array<vector<Label*>, 3> subjectLabel, array<vector<Label*>, 3> objectLabel,
		HashTable* parentGroup);
	~CompareLabel();

	bool allowRead() { return read; }
	bool allowWrite() { return write; }

private:

	void compareLevel(vector<Label*> subject, vector<Label*> object, bool& read, bool& write);
	bool compareCompartments(vector<Label*> subject, vector<Label*> object);
	bool compareGroups(vector<Label*> subject, vector<Label*> object);
	bool compareParGr(string sub, string objPar);
	bool read;
	bool write;
	HashTable* parentGroup;
};