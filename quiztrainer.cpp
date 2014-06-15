#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <time.h>

using namespace std;

/*
 * a single question
 */
typedef struct question question;
struct question {
	char *qustn;	// question
	char *a1;	// right answer
	char *a2;	// wrong answer
	char *a3;	// wrong answer
	char *a4;	// wrong answer
	question *next;
};

/*
 * catalog of questions
 */
typedef struct questions questions;
struct questions {
	char *topic;	 	// topic
	size_t count;		// count
	question *first;
};

/*
 * Generates a non recurring random sequence of numbers with the lenght "lenght" 
 * Each number is between 0 and "max". 
 * It returns 0 if "lenght" > "max" avoiding endless loop
 */
size_t *randSequence(size_t lenght, size_t max)
{
	size_t *array;	// array for the random sequence
	size_t current;	// cursor for random sequence
	size_t check;	// check cursor for random sequence
	short done;	// done indicator

	// ensure c not larger than a -> endless loop
	if (lenght > max) return 0;

	// initialize random sequence, indicator and random source
	srand((unsigned)time(NULL));
	array = new size_t[lenght];

	// generate random sequence
	for(current = 0; current < lenght ; current++){
		do {
			done = 0;
			array[current] = rand() % max;
			for(check = 0; check < current; check++) {
				if(array[current] == array[check]){
					done = 1;
					break;
				}
			}
		} while(done);
	}
	return array;
}

/*
 * read in of a question file
 */
questions* readQuestions(char *name, char *path)
{
	char *l;		// holder for converted cstring
	char *tmp;		// holder for extracted part of cstring l
	size_t MAXSIZE;		// maximal bytes that are allowed in a line
	question *q;		// a single question
	questions *qs;		// question catalog that will be returned
	question *cur;		// current question
	string line;		// line in the document
   	ifstream infile;	// infile pointer

   	MAXSIZE = 1024;
	l = new char[MAXSIZE];
	tmp = new char[MAXSIZE];

	// create and initialize question catalog
	qs = new questions;
	qs->first = 0;
	qs->topic = name;
	qs->count = 0;
	
	// read lines of file and write them into question catalog
	infile.open(path);
   	while(getline(infile, line)){
		if(count(line.begin(), line.end(), '|') != 4) continue;
		strncpy(l, line.c_str(), MAXSIZE);
		q = new question;
		
		tmp = strtok(l, "|");
		q->qustn = new char[MAXSIZE];
		strncpy(q->qustn, tmp, MAXSIZE);	
		
		tmp = strtok(NULL, "|");
		q->a1 = new char[MAXSIZE];
		strncpy(q->a1, tmp, MAXSIZE);

		tmp = strtok(NULL, "|");
		q->a2 = new char[MAXSIZE];
		strncpy(q->a2, tmp, MAXSIZE);

		tmp = strtok(NULL, "|");
		q->a3 = new char[MAXSIZE];
		strncpy(q->a3, tmp, MAXSIZE);

		tmp = strtok(NULL, "|");
		q->a4 = new char[MAXSIZE];
		strncpy(q->a4, tmp, MAXSIZE);

		if(qs->first == 0) {
			qs->first = q;
			cur = q;
		} else {
			cur->next = q;
			cur = cur->next;
		}

		cur->next = 0;
		qs->count++;
	}
   	infile.close();

	// if there is not a single question -> returns 0
	if (qs->count == 0){
		return 0;
	}

	return qs;
}

/*
 * will free every allocated byte
 */
void cleanup(questions **qsa, size_t qsac)
{
	size_t x;	//counter of question catalogs
	size_t y;	//counter of question in question catalog
	question *q;	//current question
	question *qn;	//pointer to next question

	for(x = 0; x < qsac; x++) {
		q = qsa[x]->first;
		for (y = 0; y < qsa[x]->count; y++) {
			qn = q->next;
			free(q->qustn);
			free(q->a1);
			free(q->a2);
			free(q->a3);
			free(q->a4);
			q = qn;
		}
		free(qsa[x]);
	}
}

/*
 * asks a question q and returns if answers was right(1) or wrong(0)
 */
size_t ask(question *q)
{
	int answer;	// answer of the Player
	size_t *random_number;	
	size_t counter;

	// displays question
	cout << q->qustn << endl;
	
	random_number = randSequence(4, 4);

	// display answers in random adjustment
	for(counter = 0 ; counter < 4 ; counter++) {
		cout << counter+1 << "  ";
		if (random_number[counter] == 0) cout << q->a1 << endl;
		if (random_number[counter] == 1) cout << q->a2 << endl;
		if (random_number[counter] == 2) cout << q->a3 << endl;
		if (random_number[counter] == 3) cout << q->a4 << endl;
	}

	// wait for valid response
	cout << endl << "Bitte geben Sie eine Antwort (1-4) ein: ";
	do { cin >> answer; } 
		while(answer != 1 && answer != 2 && answer != 3 && answer != 4);

	if(random_number[answer-1] == 0){
		cout << "richtig" << endl;
		free(random_number);
		return 1;
	} else {
		cout << "falsch, richtig ist: " << q->a1 << endl;
	}
	cout << endl;
	
	free(random_number);
	return 0;
}

/*
 * random quiz mode - will return number of right answers - will adjust cc and
 * cq if numbers are to high and there are not enough questions or question catalogs
 *
 * qsa - all the question catalogs
 * qsac - amount of question catalogs
 * cc - amount of question catalogs to use (set by Player)
 * cq - amount of questions of a question catalog to ask (set by Player) 
 */
size_t randQuiz(questions **qsa, size_t qsac)
{
	size_t *r_categories;	// random sequence of question catalogs (categories)
	size_t *r_question;	// random sequence of questions
	question *q;		// temporary question
	size_t c_count;		// question  catalog (categorie) counter
	size_t q_count;		// question counter
	size_t tmp;		// temporary cursor for question list
	size_t correct;		// number of correct answers
	size_t categorie;	//amount of question catalogs (categories) (set by Player)
	size_t quest;		//amount of questions per categories (set by Player)
	
	// intialize correct answer counter
	correct = 0;
	cout << "Zufallsquiz" << endl;
	cout << "Aus wie vielen Kategorien sollen die Fragen ausgesucht werden?" << endl;
	cin >> categorie;
	cout << "Wie viele Fragen möchtest du je Kategorie?" << endl;
	cin >> quest;
	
	// if there are not enough categories adjust "categorie"
	if (categorie > qsac) categorie = qsac;
	
	// get a random sequence from the question catalog loaded
	r_categories = randSequence(categorie, qsac);

	// ask for all the question catalogs
	for (c_count = 0; c_count < categorie; c_count++) {

		cout << "Kategorie: " << qsa[r_categories[c_count]]->topic << endl;

		// if there are not enough questions adjust "quest"
		if (quest > qsa[r_categories[c_count]]->count) {
		quest = qsa[r_categories[c_count]]->count;
		}
		
		// get a random sequence of all the questions to ask
		r_question = randSequence(quest, qsa[r_categories[c_count]]->count);

		// ask questions of this question catalog
		for(q_count = 0; q_count < quest; q_count++) {

			// scroll the question catalog to wanted question
			q = qsa[r_categories[c_count]]->first;
			for(tmp = 0; tmp < r_question[q_count]; tmp++) {
				q = q->next;
			}
			correct += ask(q);
		}
	}
	cout << endl << "==========================================" << endl;
	cout << correct << " von " << quest*categorie << " richtig." << endl;
}

size_t trainingQuiz(questions **qsa, size_t qsac)
{
	size_t i;
	size_t categorie;	// The categorie the player choosed
	size_t *r_question;	// random Sequence for questions
	question *q;		// temporary question
	size_t tmp;		// temporary cursor for question list
	size_t q_count;		// question counter
	size_t right;		// 0 = answer wrong, 1 = answer right
	size_t correct;		// amount of correct answers
	
	correct = 0;
	
	cout << "Training" << endl;
	cout << "Suche eine Kategorie aus und gebe die Zahl dahinter ein" << endl;

	for(i = 0; i < qsac; i++){
		cout << qsa[i]->topic << " (" << i << ")" << endl;
	}
	
	cin >> categorie;
	
	r_question = randSequence(qsa[categorie]->count, qsa[categorie]->count);
	
	for(q_count = 0; q_count < qsa[categorie]->count; q_count++) {
		
		q = qsa[categorie]->first;
		
			for(tmp = 0; tmp < r_question[q_count]; tmp++) {
				q = q->next;
			}
			right = ask(q);
			correct += right;
			if (right == 0)break;
		}
	
	cout << endl << "==========================================" << endl;
	cout << correct << " richtige Antworten" << endl;
		
}

int main()
{
	questions *qs;		// single question catalog
	questions *qsa[100];	// all the question catalogs (not more than 100)
	size_t qsac;		// cursor of the question catalogs
	size_t selection;    	//Game player choosed

	// intialize counter
	qsac = 0;

	// read in of all the questions
	qs = readQuestions("Computerspiele","./db/Computerspiele");
	if (qs != 0) {
		qsa[qsac] = qs;
		qsac++;
	}
	qs = readQuestions("Geschichte","./db/Geschichte");
	if (qs != 0) {
		qsa[qsac] = qs;
		qsac++;
	}
	qs = readQuestions("Wirtschaft","./db/Wirtschaft");
	if (qs != 0) {
		qsa[qsac] = qs;
		qsac++;
	}
	qs = readQuestions("Mensch und Tier","./db/Mensch und Tier");
	if (qs != 0) {
		qsa[qsac] = qs;
		qsac++;
	}
	// MENU
	cout << "Suche aus und gebe die Zahl ein:" << endl;
	cout << "Training (1)" << endl;
	cout << "Zufalls Quiz (2)" << endl;
	cout << "Beenden (3)" << endl;
	cin >> selection;
	// 1 Training
	//Spieler sucht eine Kategorie aus und spielt, bis er 3 falsche hat
	// 2 Zufalls Quiz 
	//Spieler sucht anzahl Kategorien/anzahl der Fragen je Kategorie aus

	// TODO free all the questions to avoid memory leak

	switch (selection){
		case 1:	trainingQuiz(qsa, qsac);
			break;
		case 2:	randQuiz(qsa, qsac);
			break;
		case 3: break;
		default: cout << "Bitte eine Zahl aussuchen" << endl;
	}
	cleanup(qsa, qsac);

	return 0;
}
