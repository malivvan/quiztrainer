#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

// zentale Fragestruktur
typedef struct question question;
struct question {	
	char *c;	 	// Fragestellung
	char *o1;	 	// immer richtige Antwort
	char *o2;	 	// falsche Antwort
	char *o3;	 	// falsche Antwort
	char *o4;	 	// falsche Antwort
	question *next; 	// naechste Antwort
};

// Menge an Fragen
typedef struct questions questions;
struct questions {
	char *t;	 	// Thema der Fragen
	long *c;	 	// Anzahl der Fragen
	question *first;	// Pointer auf die erste Frage
};

// gibt den Pointer auf eine Fragestruktur zurueck
question* new_question(char *c)
{
	question *q;	 		// deklariere eine frage q
	q = new question;	 	// beziehe Speicher fuer die Fragestruktur
	q->c = new char[sizeof(c)];	// beziehe Speicher fuer die Frage
	strncpy(q->c, c, sizeof(c));	// uebertrage inhalt von c
	return q;
}

// liest eine Fragendatei ein
// maximal 2048 bytes pro Zeile (das ist mehr als genug)
questions* read_questions(char *name, char *path)
{
	// definiere MAXSIZE
	size_t MAXSIZE = 2048;

	// definiere Datei Verwaltungs variablen
	string line;
   	ifstream infile;

	// definiere und initialisiere Hilfsvariablen
	char *l;
	l = new char[MAXSIZE];
	char *tmp;
	tmp = new char[MAXSIZE];

	// definiere Fragevariablen
	question *q;
	question *cur;

	// definiere und initialisiere Fragenkatalog
	questions *qs;
	qs = new questions;
	qs->first = 0;
	qs->c = 0;
	qs->t = name;

	// oeffne datei und lese Zeile fuer Zeile
   	infile.open(path);
   	while(getline(infile, line)){

		// wenn die Zeile nicht korrekt formatiert ist ueberspringe
		if(count(line.begin(), line.end(), '|') != 4) continue;

		// konvertiere string zu c_string (l ist jetzt char array)
		strncpy(l, line.c_str(), MAXSIZE);

		// create a new question
		q = new question;

		// Frage
		tmp = strtok(l, "|");	 	 // springe zu erstem Token
		q->c = new char[MAXSIZE];	 // beziehe den Speicher
		strncpy(q->c, tmp, MAXSIZE);	 // kopiere die Informationenq

		// richtige Antwort
		tmp = strtok(NULL, "|");	 // springe zum naechsten Token
		q->o1 = new char[MAXSIZE];	 // beziehe den Speicher
		strncpy(q->o1, tmp, MAXSIZE);	 // kopiere die Informationen

		// falsche Antowort 1
		tmp = strtok(NULL, "|");	 // springe zum naechsten Token
		q->o2 = new char[MAXSIZE];	 // beziehe den Speicher
		strncpy(q->o2, tmp, MAXSIZE);	 // kopiere die Informationen

		// falsche Antowort 2
		tmp = strtok(NULL, "|");	 // springe zum naechsten Token
		q->o3 = new char[MAXSIZE];	 // beziehe den Speicher
		strncpy(q->o3, tmp, MAXSIZE);	 // kopiere die Informationen 

		// falsche Antowort 3
		tmp = strtok(NULL, "|");	 // springe zum naechsten Token
		q->o4 = new char[MAXSIZE];	 // beziehe den Speicher
		strncpy(q->o4, tmp, MAXSIZE);	 // kopiere die Informationen 

		// wenn das die erste Frage ist setze den entsprechenden Pointer
		if(qs->first == 0) {
			qs->first = q;
			cur = q;

		// wenn das nicht die erste Frage ist gehe nach dem cur Pointer
		} else {
			cur->next = q;
			cur = cur->next;
		}

		// setze die next variable auf 0
		cur->next = 0;

		// erhoehe den Fragenzaehler
		qs->c++;
		}
   	infile.close();

	// wenn keine Fragen im Katalog sind gebe 0 zurueck
	if (qs->c == 0){
		return 0;
	}

	return qs;
}


int main()
{
	questions *qs;
	question *q;
	qs = read_questions("Computerspiele","./db/Computerspiele");

	int c;

	if (qs != 0){
		//cout << qs->t << endl;
		q = qs->first;
		while(true){
			cout << string(q->c) << endl;
			if(q->next == 0) {
				break;
			} else {
				q = q->next;
			}
		} 
	}

	getchar();
	return 0;
}
