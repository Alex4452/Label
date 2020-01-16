#include "pch.h"
#include "framework.h"
#include "LabelDLL.h"

LabelProcess::LabelProcess()
{
}

/******************************************************************************
 *
 *	Files with labels of available levels, compartments and groups are downloaded here.
 */

LabelProcess::LabelProcess(ifstream* labelFile, HashTable& newT)
{
	if (!labelFile->is_open()) 
	{
		printf("File dont open");
		return;
	}
	loadLabel(labelFile, newT);
}

/******************************************************************************
 *
 *	Object labels files are downloaded here.
 */

ClientLabelProcess::ClientLabelProcess(ifstream * labelFile, vector<array<vector<Label*>, 3>>& clientLabel, array<HashTable*, 3> tables)
{
	this->tables = tables;
	if (!labelFile->is_open())
	{
		printf("File dont open");
		return;
	}
	loadLabel(labelFile, clientLabel);
}

/******************************************************************************
 *
 *	Parsing a multi-line file with available labels, one line - one label
 */

void LabelProcess::loadLabel(ifstream* labelFile, HashTable& newT)
{
	const unsigned int n = 120;
	char* buff = new char[n];
	while (!labelFile->eof())
	{
		labelFile->getline(buff, n, '\n');
		Label* label = parseLabel(buff); 
		newT.insert(label);
	}
	delete buff;
}

/******************************************************************************
 *
 *	Parsing a multi-line file with object labels, one line - one label
 */

void ClientLabelProcess::loadLabel(ifstream * labelFile, vector<array<vector<Label*>, 3>>& clientLabel)
{
	const unsigned int n = 120;
	char* buff = new char[n];
	for (int i = 0; !labelFile->eof(); i++)
	{
		vector<Label*> level;
		vector<Label*> compartments;
		vector<Label*> groups;
		array<vector<Label*>, 3> client = { level, compartments, groups };
		labelFile->getline(buff, n, '\n');
		parseLabel(buff, client);
		clientLabel.push_back(client);
	}
	delete buff;
}

/******************************************************************************
 *
 *	When parsing available labels, the colon is taken into account, the first record 
 *  is the number, the second is the long record, the third is the short 
 *  record, the fourth is the parent group (relevant for the file with group labels)
 */

void LabelProcess::checkColon(string * temp, Label * label, int& count)
{
	switch (count)
	{
	case 0:
		label->numForm = atoi(temp->c_str());
		count++;
		break;
	case 1:
		label->longForm = *temp;
		count++;
		break;
	case 2:
		label->shortForm = *temp;
		count++;
		break;
	case 3:
		label->parentName = *temp;
		break;
	default:
		break;
	}
}

/******************************************************************************
 *
 *	Parsing recorded available labels line by line
 */

Label* LabelProcess::parseLabel(char* str)
{
	int startInd = 0;
	string input = str;
	string temp;
	int count = 0;

	Label* label = new Label();
	int i = 0;

	for (; i < input.size(); i++)
	{

		switch (input[i])
		{
		case ':':
			readLabel(temp, input, startInd, i);
			checkColon(&temp, label, count);
			break;
		default:
			break;
		}
	}
	readLabel(temp, input, startInd, i);
	checkColon(&temp, label, count);

	return label;
}

/******************************************************************************
 *
 *	When parsing object labels, both columns and commas are 
 *  taken into account to record a multi-dimensional label
 */

void ClientLabelProcess::checkColonComma(string * temp, array<vector<Label*>, 3>& clientLabel, int & count, bool checkComma)
{
	clientLabel[count].push_back(tables[count]->find(*temp));
	if(!checkComma)
		count++;
}

/******************************************************************************
 *
 *	Parsing recorded object labels line by line
 */

void ClientLabelProcess::parseLabel(char* str, array<vector<Label*>, 3>& clientLabel)
{
	int startInd = 0;
	string input = str;
	string temp;
	int count = 0;
	bool comma = false;

	int i = 0;
	for (; i < input.size(); i++)
	{
		switch (input[i])
		{
		case ':':
			comma = false;
			readLabel(temp, input, startInd, i);
			checkColonComma(&temp, clientLabel, count, comma);
			break;
		case ',':
			comma = true;
			readLabel(temp, input, startInd, i);
			checkColonComma(&temp, clientLabel, count, comma);
			break;
		default:
			break;
		}
	}
	readLabel(temp, input, startInd, i);
	checkColonComma(&temp, clientLabel, count, comma);
}

Process::Process()
{
}

/******************************************************************************
 *
 *	Clears a line from spaces, commas and colons
 */

void Process::clearString(string & str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ' || str[i] == ':' || str[i] == ',')
		{
			str.erase(i, 1);
			i--;
		}
	}
}

/******************************************************************************
 *
 *	Reads a value from a string
 */

void Process::readLabel(string & temp, string input, int & startInd, int & i)
{
	temp = input.substr(startInd, i - startInd);
	clearString(temp);
	startInd = i;
}

HashTable::HashTable()
{
	for (size_t i = 0; i < DEFAULT_NUM; i++)
	{
		table[i] = NULL;
	}
}

HashTable::~HashTable()
{
	for (size_t i = 0; i < DEFAULT_NUM; i++)
	{
		delete table[i];
	}
}

/******************************************************************************
 *
 *	Specifies an index in a hash table based on a long form of label writing
 */

int HashTable::hash(string str)
{
	unsigned int hashsumm = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		hashsumm += str[i];
	}
	return (hashsumm & 0x7fffffff) % DEFAULT_NUM;
}

/******************************************************************************
 *
 *	Adds a label to the hash table
 */

void HashTable::insert(Label* label)
{
	const unsigned int hashNum = hash(label->longForm);

	Label* placeLabel = table[hashNum];

	if (placeLabel == NULL)
	{
		table[hashNum] = label;
		return;
	}

	while (placeLabel->next != NULL)
	{
		placeLabel = placeLabel->next;
	}
	placeLabel->next = label;
}

/******************************************************************************
 *
 *	Finds a label to the hash table
 */

Label* HashTable::find(string longForm)
{
	const unsigned int hashNum = hash(longForm);
	Label* result = (table[hashNum]);
	if (!result)
	{
		printf("Element not found\n");
		return NULL;
	}
	while (result->longForm != longForm)
	{
		if (!result->next)
		{
			printf("Element not found\n");
			break;
		}
		result = result->next;
	}
	return result;
}

/******************************************************************************
 *
 *	Compares a label for a hash table
 */

bool Label::check(Label* labCheck)
{
	if (this->numForm == labCheck->numForm && this->longForm == labCheck->longForm &&
		this->shortForm == labCheck->shortForm)
	{
		return true;
	}
	return false;
}

CompareLabel::CompareLabel() : read(false), write(false)
{
}

/******************************************************************************
 *
 *	Checks whether the subject has read or write permission to the object
 */

CompareLabel::CompareLabel(array<vector<Label*>, 3> subjectLabel, array<vector<Label*>, 3> objectLabel,
	HashTable* parentGroup) :
	parentGroup(parentGroup)
{
	bool readLev, writeLev;
	compareLevel(subjectLabel[0], objectLabel[0], readLev, writeLev);
	bool compare = compareCompartments(subjectLabel[1], objectLabel[1]);
	compare = compareGroups(subjectLabel[2], objectLabel[2]);
	if (compare)
	{
		this->read = (readLev) ? true : false;
		this->write = (writeLev) ? true : false;
	}
	else
	{
		this->read = false;
		this->write = false;
	}
}

CompareLabel::~CompareLabel()
{
}

/******************************************************************************
 *
 *	Compare subject and object access levels
 */

void CompareLabel::compareLevel(vector<Label*> subject, vector<Label*> object, bool& read, bool& write)
{
	if (subject[2]->numForm >= object[0]->numForm)
		read = true;
	if (subject[0]->numForm >= object[0]->numForm ||
		subject[1]->numForm <= object[0]->numForm)
		write = true;
}

/******************************************************************************
 *
 *	Compare subject and object compartments
 */

bool CompareLabel::compareCompartments(vector<Label*> subject, vector<Label*> object)
{
	for (auto itSub = subject.begin(); itSub != subject.end(); itSub++)
	{
		for (auto itOb = object.begin(); itOb != object.end(); itOb++)
		{
			if ((*itSub)->check(*itOb))
				return true;
		}
	}
	return false;
}

/******************************************************************************
 *
 *	Compare subject and object groups
 */

bool CompareLabel::compareGroups(vector<Label*> subject, vector<Label*> object)
{
	for (auto itSub = subject.begin(); itSub != subject.end(); itSub++)
	{
		for (auto itOb = object.begin(); itOb != object.end(); itOb++)
		{
			if ((*itSub)->check(*itOb))
				return true;
			else
			{
				return compareParGr((*itSub)->longForm, (*itOb)->longForm);
			}
		}
	}
}

/******************************************************************************
 *
 *	Compare subject and object parent groups
 */

bool CompareLabel::compareParGr(string sub, string objPar)
{
	Label* parGr = parentGroup->find(objPar);
	if (sub == parGr->longForm)
		return true;
	else if (parGr == NULL)
		return false;
	else
		return compareParGr(sub, parGr->parentName);
}

