program:
	g++ -o ejecutable GVRP.cpp

.PHONY: run
run: 
	./ejecutable
clean:
	rm ./ejecutable.exe