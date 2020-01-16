#include <iostream>
#include <fstream>
#include <string>
#include "LabelDLL.h"

using namespace std;

int main()
{
	ifstream fout1("labelLevel.txt");
	HashTable levelTable;
	LabelProcess labelLevel(&fout1, levelTable);

	ifstream fout2("labelCompartments.txt");
	HashTable compartmentsTable;
	LabelProcess labelCompartments(&fout2, compartmentsTable);

	ifstream fout3("labelGroups.txt");
	HashTable groupTable;
	LabelProcess labelGroup(&fout3, groupTable);

	vector<array<vector<Label*>, 3>> clientLabel;
	array<HashTable*, 3> clientHash = { &levelTable, &compartmentsTable, &groupTable };
	ifstream foutClient("createLabel.txt");
	ClientLabelProcess clientTest(&foutClient, clientLabel, clientHash);

	auto it = clientLabel.begin();
	auto it2 = it + 1;

	CompareLabel compare(*it, *it2, &groupTable);

	if (compare.allowRead())
		printf("Read is allowed.\n");
	if (compare.allowWrite())
		printf("Write is allowed.\n");
}