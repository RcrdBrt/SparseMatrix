main.exe: main.cpp SparseMatrix.h
	g++ main.cpp -static-libgcc -static-libstdc++ -pedantic -o main.exe

debug:
	g++ main.cpp -static-libgcc -static-libstdc++ -pedantic -D DEBUG -o main.exe
