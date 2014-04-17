all: quiztrainer

quiztrainer: 
	g++ -o quiztrainer quiztrainer.cpp
clean:
	rm -f quiztrainer
