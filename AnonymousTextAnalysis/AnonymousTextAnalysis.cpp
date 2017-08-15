// AnonymousTextAnalysis.cpp : Defines the entry point for the console application.
//
//references
//http://scikit-learn.org/stable/modules/naive_bayes.html#bernoulli-naive-bayes
//https://www.analyticsvidhya.com/blog/2015/09/naive-bayes-explained/
//https://en.wikipedia.org/wiki/Naive_Bayes_classifier
//
//Written by Trenton Thompson
//Lasted updated April 5, 2017

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct wordCount {
	string word;
	int count;
};

struct Person {
	string name;//identifier
	vector<string> messages;//all recorded messages of person
	vector<wordCount> wordlist;
};

struct TestMessage {
	string name;//identifier
	string msg;//typed message
};

void addPerson(vector<Person>* peopleList, string name);
int addMsgToPerson(vector<Person>* peopleList, string person, string msg);
double compareMsgToPerson(Person person, string msg);
vector<string> getWords(string);
vector<wordCount> getWordCount(vector<string>);
void getWordCount(vector<string> words, vector<wordCount>* wordCountList);
int getCountForPerson(Person person, string word);
long double calcProbOfPersonGivenMessage(Person person, string msg, int vocabularySize);
void addToVocab(string words, vector<string>* vocabulary);
void addToVocab(vector<string>, vector<string>*);
int loadPeople(vector<Person>*, string);
string loadAnonMsg(string filename);
vector<TestMessage> loadTestMsgs(string filename);

int main()
{
	vector<Person> listOfPeople;
	vector<string> vocabulary;

	int errNum = loadPeople(&listOfPeople, "database.txt");

	//make sure database was loaded
	if (errNum < 0) {
		cout << "Could not load database.\n\n";
		system("PAUSE");
		return errNum;
	}

	//get the word count for each person
	for (int i = 0; i < listOfPeople.size(); i++) {
		for (int j = 0; j < listOfPeople.at(i).messages.size(); j++) {
			getWordCount(getWords(listOfPeople.at(i).messages.at(j)), &listOfPeople.at(i).wordlist);
		}
	}

	//put together vocabulary
	for (int i = 0; i < listOfPeople.size(); i++) {
		for (int j = 0; j < listOfPeople.at(i).messages.size(); j++) {
			addToVocab(listOfPeople.at(i).messages.at(j), &vocabulary);
		}
	}

	string anonMsg;// = loadAnonMsg("anonymous message.txt");

	do {
		for (int i = 0; i < listOfPeople.size(); i++) {
			cout << listOfPeople.at(i).name << " word count: " << listOfPeople.at(i).wordlist.size() << "\n";
		}
		cout << "Enter anonymous message (type ~exit to exit, ~test to load test msgs): ";
		getline(cin, anonMsg);

		if (anonMsg != "~exit" && anonMsg != "~test") {
			addToVocab(anonMsg, &vocabulary);

			//do math
			vector<long double> posteriorValues;
			long double denominator = 0.0L;
			int highestValue = 0;//index in posteriorValues that holds the highest posterior value

			//obtain posterior values for all people
			for (int i = 0; i < listOfPeople.size(); i++) {
				posteriorValues.push_back(calcProbOfPersonGivenMessage(listOfPeople.at(i), anonMsg, vocabulary.size()));
				denominator += posteriorValues.at(i);
				cout << "Posterior value for " << listOfPeople.at(i).name << ": " << posteriorValues.at(i) << "\n";
			}

			//compare posterior values to find the highest of those values;
			for (int i = 1; i < posteriorValues.size(); i++) {
				if (posteriorValues.at(i) >= posteriorValues.at(highestValue)) {
					highestValue = i;
				}
			}
			//cease math

			//who done it
			if (listOfPeople.size() > highestValue)
			{
				cout << "The person most likely to have said this is " << listOfPeople.at(highestValue).name << "\n";
				cout << "The proportion is " << posteriorValues.at(highestValue) / denominator << "\n";
			}

			system("PAUSE");
			system("CLS");
		}
		else if (anonMsg == "~test") {
			vector<TestMessage> testMsgs = loadTestMsgs("test.txt");
			int correct = 0;
			int wrong = 0;

			for (int testValue = 0; testValue < testMsgs.size(); testValue++) {
				anonMsg = testMsgs.at(testValue).msg;

				cout << "Message: " << anonMsg << "\n";

				addToVocab(anonMsg, &vocabulary);

				//do math
				vector<long double> posteriorValues;
				long double denominator = 0.0L;
				int highestValue = 0;//index in posteriorValues that holds the highest posterior value

									 //obtain posterior values for all people
				for (int i = 0; i < listOfPeople.size(); i++) {
					posteriorValues.push_back(calcProbOfPersonGivenMessage(listOfPeople.at(i), anonMsg, vocabulary.size()));
					denominator += posteriorValues.at(i);
					cout << "Posterior value for " << listOfPeople.at(i).name << ": " << posteriorValues.at(i) << "\n";
				}

				//compare posterior values to find the highest of those values;
				for (int i = 1; i < posteriorValues.size(); i++) {
					if (posteriorValues.at(i) >= posteriorValues.at(highestValue)) {
						highestValue = i;
					}
				}
				//cease math

				//who done it
				if (denominator > 0) {
					if (listOfPeople.size() > highestValue)
					{
						cout << "The person most likely to have said this is " << listOfPeople.at(highestValue).name << "\n";
						cout << "The proportion is " << posteriorValues.at(highestValue) / denominator << "\n";
					}

					if (listOfPeople.at(highestValue).name == testMsgs.at(testValue).name.substr(0, testMsgs.at(testValue).name.length()-1)) {
						cout << "This algorithm guessed correctly.\n";
						correct++;
					}
					else {
						cout << "This algorithm was wrong.\n";
						wrong++;
					}
				}
				else {
					cout << "Calculation for this message could not be made.\n";
				}

				/*
				addMsgToPerson(&listOfPeople, testMsgs.at(testValue).name, anonMsg);

				//calculate word count for everyone
				for (int i = 0; i < listOfPeople.size(); i++) {
					for (int j = 0; j < listOfPeople.at(i).messages.size(); j++) {
						getWordCount(getWords(listOfPeople.at(i).messages.at(j)), &listOfPeople.at(i).wordlist);
					}
				}*/

				//system("PAUSE");
				//system("CLS");
			}

			if (wrong + correct > 0) {
				cout << "\nThis algorithm was correct " << ((double)correct * 100) / (wrong + correct)
					<< "% of the time out of " << correct + wrong << " messages.\n\n";
			}
			else {
				cout << "There were no test messages.\n";
			}
		}
	} while (anonMsg != "~exit");

    return 0;
}

void addPerson(vector<Person>* list, string name)
{
	Person newPerson;
	newPerson.name = name;
	list->push_back(newPerson);
}

//adds a message to a person's message history.
//returns 0 on success, or a negative value on failure
int addMsgToPerson(vector<Person>* peopleList, string name, string msg) {
	//attempt to find person in list of people
	for (int i = 0; i < peopleList->size(); i++) {
		//if we find the person
		if (peopleList->at(i).name == name) {
			peopleList->at(i).messages.push_back(msg);
			return 0;
		}
	}

	return -1;
}

//Takes a person structure and an anonymous message as input
//returns percent chance that the message was written by the person

double compareMsgToPerson(Person person, string msg)
{
	int numOfMsgs = person.messages.size();

	for (int i = 0; i < numOfMsgs; i++) {

	}

	return 0.0;
}

long double calcProbOfPersonGivenMessage(Person person, string msg, int vocabularySize) {
	vector<long double> posteriorValues;
	vector<string> msgWords = getWords(msg);

	for (int i = 0; i < msgWords.size(); i++) {
		//number of times that word in msg appears in person's messages + 1
		//divided by
		//number of unique words in person's vocabulary + all unique words in database
		posteriorValues.push_back(((long double)getCountForPerson(person, msgWords.at(i)) + 1.0)
			/ (person.wordlist.size() + vocabularySize));
	}

	if (posteriorValues.size() > 0) {
		long double chance = posteriorValues.at(0);
		for (int i = 1; i < posteriorValues.size(); i++) {
			chance *= posteriorValues.at(i);
		}
		return chance;
	}

	return 0.0L;
}

int getCountForPerson(Person person, string word) {
	for (int i = 0; i < person.wordlist.size(); i++) {
		if (person.wordlist.at(i).word == word) {
			return person.wordlist.at(i).count;
		}
	}

	return 0;
}

vector<string> getWords(string msg) {
	string extractedWord;
	vector<string> list;

	for (int i = 0; i < msg.length(); i++) {
		//if the next character is a 
		if (((int)msg.at(i) >= (int)'a' && (int)msg.at(i) <= (int)'z') ||
			((int)msg.at(i) >= (int)'A' && (int)msg.at(i) <= (int)'Z') ||
			((int)msg.at(i) >= (int)'0' && (int)msg.at(i) <= (int)'9') ||
			msg.at(i) == '@' || msg.at(i) == '$' || msg.at(i) == '&' ||
			msg.at(i) == '\'' || msg.at(i) == '-') {
			extractedWord.push_back(msg.at(i));
		}
		else if (extractedWord.length() > 0) {
			list.push_back(extractedWord);
			extractedWord.clear();
		}
	}

	if (extractedWord.length() > 0) {
		list.push_back(extractedWord);
	}

	return list;
}

//constructs a wordCount vector given a vector of words
//calling this function again creates a new wordCount vector and does not overcount
vector<wordCount> getWordCount(vector<string> words) {
	vector<wordCount> wordCountList;
	wordCount keypair;

	//go through each word in the list of words
	for (int i = 0; i < words.size(); i++) {
		//if it's the first word, start up the wordCountList
		if (wordCountList.size() == 0) {
			keypair.word = words.at(i);
			keypair.count = 1;
			wordCountList.push_back(keypair);
		}
		//if the list has words in it already, check to see if the word is already in the list
		else {
			for (int j = 0; j < wordCountList.size(); j++) {
				//if you find the word, increment the count value and exit the loop
				if (wordCountList.at(j).word == words.at(i)) {
					wordCountList.at(j).count++;
					j = wordCountList.size();
				}
				//if we're at the last word and didn't find it
				else if (j == wordCountList.size() - 1) {
					//add it to the wordCountList
					keypair.word = words.at(i);
					keypair.count = 1;
					wordCountList.push_back(keypair);
					j++;
				}
			}
		}
	}

	return wordCountList;
}

void getWordCount(vector<string> words, vector<wordCount>* wordCountList) {
	wordCount keypair;

	//go through each word in the list of words
	for (int i = 0; i < words.size(); i++) {
		//if it's the first word, start up the wordCountList
		if (wordCountList->size() == 0) {
			keypair.word = words.at(i);
			keypair.count = 1;
			wordCountList->push_back(keypair);
		}
		//if the list has words in it already, check to see if the word is already in the list
		else {
			for (int j = 0; j < wordCountList->size(); j++) {
				//if you find the word, increment the count value and exit the loop
				if (wordCountList->at(j).word == words.at(i)) {
					wordCountList->at(j).count++;
					j = wordCountList->size();
				}
				//if we're at the last word and didn't find it
				else if (j == wordCountList->size() - 1) {
					//add it to the wordCountList
					keypair.word = words.at(i);
					keypair.count = 1;
					wordCountList->push_back(keypair);
					j++;
				}
			}
		}
	}
}

void addToVocab(string words, vector<string>* vocabulary) {
	vector<string> wordlist = getWords(words);

	for (int i = 0; i < wordlist.size(); i++) {
		for (int j = 0; j < vocabulary->size(); j++) {
			if (wordlist.at(i) == vocabulary->at(j)) {
				j = vocabulary->size();
			}
			else if (j == vocabulary->size() - 1) {
				vocabulary->push_back(wordlist.at(i));
				j++;
			}
		}
	}
}

void addToVocab(vector<string> wordlist, vector<string>* vocabulary) {
	for (int i = 0; i < wordlist.size(); i++) {
		for (int j = 0; j < vocabulary->size(); j++) {
			if (wordlist.at(i) == vocabulary->at(j)) {
				j = vocabulary->size();
			}
			else if (j == vocabulary->size() - 1) {
				vocabulary->push_back(wordlist.at(i));
				j++;
			}
		}
	}
}

int loadPeople(vector<Person>* peopleList, string filename) {
	ifstream load;
	string name = "";//name of person
	string msg = "";//message from person
	char loadedChar;
	Person personToAdd;

	//clear list
	peopleList->clear();

	load.open(filename);

	if (load) {
		//while we still have characters to load
		while (!load.eof()) {
			load.get(loadedChar);
			//if we're still loading the name
			if (name.size() == 0 || (name.at(name.size() - 1) != '|' && loadedChar != '\n')) {
				name.push_back(loadedChar);
			}
			//if we're still loading the message
			else if (loadedChar != '\n' && !load.eof()) {
				msg.push_back(loadedChar);
			}

			//we loaded both name and message, save them to memory
			else {
				//name and message are fully loaded
				//search peopleList for the person's name
				for (int i = 0; i < peopleList->size(); i++) {
					//if the person's name exists in our database, add the message to that name
					if (peopleList->at(i).name == name.substr(0, name.length() - 1)) {
						peopleList->at(i).messages.push_back(msg);
						i = peopleList->size();
					}
					else if (i == peopleList->size() - 1) {
						personToAdd.name = name.substr(0, name.length() - 1);
						personToAdd.messages.push_back(msg);
						peopleList->push_back(personToAdd);
						i = peopleList->size();
					}
				}
				if (peopleList->size() == 0) {
					personToAdd.name = name.substr(0, name.length() - 1);
					personToAdd.messages.push_back(msg);
					peopleList->push_back(personToAdd);
				}

				//always clear once we hit a newline character
				name.clear();
				msg.clear();
				personToAdd.messages.clear();
				personToAdd.name.clear();
			}
		}

		load.close();
		return 0;
	}

	load.close();
	return -1;
}

string loadAnonMsg(string filename)
{
	ifstream load(filename);
	string msg;
	char loadedChar;

	if (load) {
		while (!load.eof()) {
			load.get(loadedChar);

			if (loadedChar != '\n' && !load.eof()) {
				msg.push_back(loadedChar);
			}
		}
	}

	load.close();

	return msg;
}

//load test messages to test against the program.
//does not parse names away from the messages.
vector<TestMessage> loadTestMsgs(string filename)
{
	ifstream load(filename);
	string name;
	string msg;
	vector<TestMessage> testMsgs;
	TestMessage emptyTestMessage;
	char loadedChar;

	emptyTestMessage.msg = "";
	emptyTestMessage.name = "";

	if (load) {
		//while we still have characters to load
		while (!load.eof()) {
			load.get(loadedChar);
			//if we're still loading the name
			if (name.size() == 0 || (name.at(name.size() - 1) != '|' && loadedChar != '\n')) {
				name.push_back(loadedChar);
			}
			//if we're still loading the message
			else if (loadedChar != '\n' && !load.eof()) {
				msg.push_back(loadedChar);
			}

			//if we come to a new line, expand the list
			else if (loadedChar == '\n' || load.eof()) {
				//add the new test message to the list
				testMsgs.push_back(emptyTestMessage);
				testMsgs.at(testMsgs.size() - 1).msg = msg;
				testMsgs.at(testMsgs.size()-1).name = name;
				msg.clear();
				name.clear();
			}
		}
	}
	else {
		cout << "Problem loading from file\n";
	}

	cout << testMsgs.size() << "\n";

	load.close();
	//if we couldn't load, msgs will be empty
	return testMsgs;
}

