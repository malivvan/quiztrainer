all: quiztrainer

quiztrainer: 
	@g++ -static -o quiztrainer quiztrainer.cpp
clean:
	@rm -f quiztrainer
