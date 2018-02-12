#include "SparseMatrix.h"
#include <iostream>
#include <stdexcept>
#include <string>

/**
 Funtore che verifica la divisibilita' per 3.
*/
template <typename T>
struct divis_per_3 {
	bool operator()(const T& val) {
		if (val % 3 == 0)
			return true;
		return false;
	}
};

/**
 Funtore che verifica la divisibilita' per 7 di unsigned int.
*/
struct divis_per_7 {
	bool operator()(const unsigned int val) {
		if (val % 7 == 0)
			return true;
		return false;
	}
};

/**
 Funtore che verifica se il primo carattere di una std::string e' la
 lettera 'a'.
*/
struct first_char_is_a {
	bool operator()(const std::string& s) {
		if (s[0] == 'a')
			return true;
		return false;
	}
};

int main() {
	SparseMatrix<int> I(3, 2, 999);
	
	// test add()
	I.add(2, 2, 4);
	I.add(2, 2, 14);
	I.add(1, 2, 2);
	I.add(2, 2, 5);
#ifdef DEBUG
	I.print();
#endif
	I.add(1, 1, 3);
	I.add(3, 2, 6);
	I.add(3, 1, 5);
	I.add(2, 1, 3);
#ifdef DEBUG
	I.print();
#endif
	// test operator()
	std::cout << "Valore in (2;2): " << I(2, 2) << std::endl;
	std::cout << "Valore in (3;2): " << I(3, 2) << std::endl;
#ifdef DEBUG
	I.print();
#endif
	
	// test constructors
	SparseMatrix<int> J(I);
#ifdef DEBUG
	J.print();
#endif
	SparseMatrix<int> K = J;
#ifdef DEBUG
	K.print();
#endif
	SparseMatrix<long> l(5, 5, 999999);
	SparseMatrix<double> d(l);
	
	// test iterators
	SparseMatrix<int>::iterator Ia, Ib, Ie, If;
	Ib = I.begin();
	Ie = I.end();
	Ia = Ib;
	If = I.begin();
	++If;
	If++;
	SparseMatrix<int>::iterator Ic(Ie);
	SparseMatrix<int>::const_iterator I_c;
	SparseMatrix<int>::element el = *Ib;
	I_c = I.begin();
	//(*I_c).dato = 77;
	
	SparseMatrix<int>::element e(*(I_c));
	std::cout << "elemento: " << (*Ib).dato << " casella: " << I(1, 1) << std::endl;
	
	// test static_cast
	SparseMatrix<double> D(5, 5, 999);
	D.add(1, 1, 150);
	SparseMatrix<int> first(D);
	SparseMatrix<std::string> S(12, 12, "abaco");
	S.add(1, 2, "blah");
	SparseMatrix<double> second_double(first);
	SparseMatrix<unsigned int> third(second_double);
	std::cout << third(1, 1) << std::endl;
	std::cout << third.get_default() << std::endl;
	// SparseMatrix<int> O(S); <--- error
	
	
	// test predicato
	divis_per_3<int> funct;
	std::cout << funct(3) << std::endl;
	std::cout << "evaluate function for divis_per_3 on I: " << evaluate(I, funct) << std::endl;
	SparseMatrix<unsigned int> W(10, 10, 999999);
	SparseMatrix<unsigned int>::iterator Ib_un_int, Ie_un_int;
	Ib_un_int = W.begin();
	Ie_un_int = W.end();
	for (int i = 1; i <= W.get_righe(); ++i)
		for (int j = 1; j <= W.get_colonne(); ++j)
			W.add(i, j, i+j);
	for (; Ib_un_int != Ie_un_int; ++Ib_un_int)
		W.add((*Ib_un_int).riga, (*Ib_un_int).colonna, (*Ib_un_int).dato + 1);
	divis_per_7 funct2;
	SparseMatrix<unsigned int> test(5, 5, 7777777);
	std::cout << "Val in (1;1): " << test(1, 1) << std::endl;
	std::cout << "divisibili per 7 in W: " << evaluate(test, funct2) << std::endl;
	(*Ib).dato = 2000;
	std::cout << (*Ib).dato << std::endl;
	first_char_is_a funct3;
	std::cout << "Posizioni con a iniziale su matrice S: " << evaluate(S, funct3) << std::endl;
}
