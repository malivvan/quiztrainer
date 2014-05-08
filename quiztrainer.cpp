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
	char *c;	// question
	char *o1;	// right answer
	char *o2;	// wrong answer
	char *o3;	// wrong answer
	char *o4;	// wrong answer
	question *next;
};

/*
 * catalog of questions
 */
typedef struct questions questions;
struct questions {
	char *t;	 	// topic
	size_t c;		// count
	question *first;
};

/*
 * generates a non recurring random sequence of numbers up to "a" and with
 * the lenght of "c". will return 0 if c < a avoiding endless loop
 */
size_t *randSequence(size_t c, size_t a)
{
	size_t *r;	// array for the random sequence
	size_t x;	// write cursor for random sequence
	size_t y;	// check cursor for random sequence
	short d;	// done indicator

	// ensure c not larger than a -> endless loop
	if (c > a) return 0;

	// initialize random sequence, indicator and random source
	srand((unsigned)time(NULL));
	r = new size_t[c];

	// generate random sequence
	for(x = 0; x < c ; x++){
		do {
			d = 0;
			r[x] = rand() % a;
			for(y = 0; y < x; y++) {
				if(r[x] == r[y]){
					d = 1;
					break;
				}
			}
		} while(d);
	}
	return r;
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
	question *cur;		// cursor needed for linking
	string line;		// line
   	ifstream infile;	// infile pointer

   	MAXSIZE = 2048;
	l = new char[MAXSIZE];
	tmp = new char[MAXSIZE];

	// create and initialize question catalog
	qs = new questions;
	qs->first = 0;
	qs->c = 0;
	qs->t = name;

	// read lines of file and write them into question catalog
	infile.open(path);
   	while(getline(infile, line)){
		if(count(line.begin(), line.end(), '|') != 4) continue;
		strncpy(l, line.c_str(), MAXSIZE);
		q = new question;

		tmp = strtok(l, "|");
		q->c = new char[strlen(tmp)];
		strncpy(q->c, tmp, strlen(tmp));

		tmp = strtok(NULL, "|");
		q->o1 = new char[strlen(tmp)];
		strncpy(q->o1, tmp, strlen(tmp));

		tmp = strtok(NULL, "|");
		q->o2 = new char[strlen(tmp)];
		strncpy(q->o2, tmp, strlen(tmp));

		tmp = strtok(NULL, "|");
		q->o3 = new char[strlen(tmp)];
		strncpy(q->o3, tmp, strlen(tmp));

		tmp = strtok(NULL, "|");
		q->o4 = new char[strlen(tmp)];
		strncpy(q->o4, tmp, strlen(tmp));

		if(qs->first == 0) {
			qs->first = q;
			cur = q;
		} else {
			cur->next = q;
			cur = cur->next;
		}

		cur->next = 0;
		qs->c++;
	}
   	infile.close();

	// if there is not a single question just return 0
	if (qs->c == 0){
		return 0;
	}

	return qs;
}

/*
 * will free every allocated byte
 */
void cleanup(questions **qsa, size_t qsac)
{
	size_t x;
	size_t y;
	question *q;
	question *qn;

	for(x = 0; x < qsac; x++) {
		q = qsa[x]->first;
		for (y = 0; y < qsa[x]->c ; y++) {
			qn = q->next;
			free(q->c);
			free(q->o1);
			free(q->o2);
			free(q->o3);
			free(q->o4);
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
	// a = answer
	// r = random sequence
	// c = counter
	int a;
	size_t *r;
	int c;

	// ask the question
	cout << q->c << endl;

	// get a random sequence
	r = randSequence(4, 4);

	// display answers
	for(c = 0 ; c < 4 ; c++) {
		cout << c+1 << "  ";
		if (r[c] == 0) cout << q->o1 << endl;
		if (r[c] == 1) cout << q->o2 << endl;
		if (r[c] == 2) cout << q->o3 << endl;
		if (r[c] == 3) cout << q->o4 << endl;
	}

	// free random sequence
	free(r);

	// wait for valid response
	cout << endl << "Bitte geben Sie eine Antwort (1-4) ein: ";
	do { cin >> a; } while(a != 1 && a != 2 && a != 3 && a != 4);

	if(r[a-1] == 0){
		cout << "richtig" << endl;
		return 1;
	} else {
		cout << "falsch, richtig ist: " << q->o1 << endl;
	}
	cout << endl;

	return 0;
}

/*
 * training quiz mode - will return number of right answers
 *
 * qsa - all the questions
 * qsac - number of categories
 */
size_t trainingQuiz(questions **qsa, size_t qsac)
{
	return 0;
}


/*
 * random quiz mode - will return number of right answers - will adjust cc and
 * cq if numbers are to high and there are not enough questions or categories
 *
 * qsa - all the questions
 * qsac - number of categories
 * cc - number of categories to use
 * cq - number of questions of a categorie to ask
 */
size_t randQuiz(questions **qsa, size_t qsac, size_t cc, size_t cq)
{
	size_t *ccr;	// random sequence of categories
	size_t *cqr;	// random sequence of questions
	question *q;	// tmp question var
	size_t x;	// categorie counter
	size_t y;	// question counter
	size_t z;	// tmp cursor for question list
	size_t r;	// number of correct answers

	// intialize correct answer counter
	r = 0;

	// if there are no enough categories adjust cc
	if (cc > qsac) cc = qsac;

	// get a random sequence from the categories loaded
	ccr = randSequence(cc, qsac);

	// ask for all the categories
	for (x = 0; x < cc; x++) {

		// display categorie header
		cout << "Kategorie: " << qsa[ccr[x]]->t << endl;

		// if there are not enough questions adjust cc
		if (cq > qsa[ccr[x]]->c) cq = qsa[ccr[x]]->c;

		// get a random sequence of all the questions to ask
		cqr = randSequence(cq, qsa[ccr[x]]->c);

		// ask questions of this categorie
		for(y = 0; y < cq; y++) {

			// scroll the question catalog to wanted question
			q = qsa[ccr[x]]->first;
			for(z = 0; z < cqr[y]; z++) {
				q = q->next;
			}

			r += ask(q);

		}

	}
	return r;
}


int main()
{
	questions *qs;		// single question catalog
	questions *qsa[100];	// all the question catalogs (not more than 100)
	size_t qsac;		// cursor of the question catalogs
	size_t c; 		// tmp counter
	size_t r;		// correct answer counter

	// intialize counter
	qsac = 0;
	r = 0;

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

	// MENU
	// 1 Training
	// 2 Zufalls Quiz
	// 3 Beenden

	// TODO free all the questions to avoid memory leak

	// prototype training

	// prototype random quiz
	r = randQuiz(qsa, qsac, 100, 100);
	cout << endl << "==========================================" << endl;
	cout << r << " von " << 3*3 << " richtig." << endl;

	cleanup(qsa, qsac);

	return 0;
}
