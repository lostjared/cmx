
	
	

objects=parse.o scanner.o token.o syntax.o parser.o
	
%.o: %.cc
	$(CXX) -Wall -c $< -o $@
all: $(objects)
	$(CXX)  $(objects) -o parseTest
clean:
	rm -rf *.o
