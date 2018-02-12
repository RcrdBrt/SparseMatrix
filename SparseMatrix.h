#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#ifdef DEBUG
	#include <iostream>
#endif

#include <algorithm>
#include <iterator> 
#include <cstddef>
#include <cassert>

/**
 Classe SparseMatrix. Crea una matrice sparsa con utilizzo di memoria minimale,
 solo gli elementi inseriti sono effettivamente memorizzati. Accetta dati di 
 tipo generico T. E' strutturata come una lista doppiamente linkata costituita da
 nodi e la struttura element contenente le informazioni di riga, colonna e dato T
 in quella posizione.

 @brief Definizione della classe templata SparseMatrix.
*/
template <typename T> ///< T = tipo generico
class SparseMatrix {
public:
	/**
	 Struttura dati pubblica, contenuta all'interno dei nodi della lista.
	 Indica l'elemento effettivo della matrice e la sua posizone.
	 
	 @brief l'elemento esposto dall'iteratore
	*/
	struct element {
		const int riga; ///< posizione riga
		const int colonna; ///< posizione colonna
		T dato; ///< dato della casella nella matrice
		/**
		 Costruttore dell'elemento
		 
		 @param r riga
		 @param c colonna
		 @param d dato
		*/
		element(const int r, const int c, const T& d) : riga(r), colonna(c), dato(d) {}
		
		// gli altri metodi fondamentali sono quelli di default
	};
private:
	/**
	 Struttura privata di supporto per la creazione della matrice sparsa.
	 E' costituita da un puntatore al nodo successivo e uno al precedente.
	 
	 @brief wrapper di element per creare una lista
	*/
	struct node {
		element e; ///< elemento della matrice
		node* next; ///< puntatore al nodo successivo
		node* prev; ///< puntatore al nodo precedente
		/**
		 Costruttore di default
		*/
		node() : next(0), prev(0), e(0) {}
		/**
		 Costruttore secondario che inizializza element e i puntatori
		 
		 @param k valore della casella della matrice
		 @param r riga
		 @param c colonna
		 @param n nodo successivo
		 @param p nodo precedente
		*/
		node(const T& k, const int r, const int c, node* n, node* p) : next(n), prev(p), e(r, c, k) {}
		
		// gli altri metodi fondamentali sono quelli di default
		
	};

	node* head; ///< puntatore alla testa della lista
	const int righe; ///< numero di righe della matrice
	const int colonne; ///< numero di colonne della matrice
	unsigned int size; ///< numero di elementi memorizzati nella matrice
	T D; ///< dato di default da ritornare se viene richiesto un elemento non presente nella matrice
	
	/**
	 Funzione helper di clear, cancella ricorsivamente la matrice a partire dal nodo passato fino alla fine
	 
	 @param n nodo da cui partire per la liberazione di memoria
	*/	
	void clear_helper(node* n) {
		if (n == 0)
			return;
		node* tmp = n->next;
		delete n;
		n = 0;
		clear_helper(tmp);
	}

	/**
	 Wrapper di clear_helper per cancellare l'intera matrice
	*/
	void clear() {
		clear_helper(head);
		head = 0;
		size = 0;
	}

public:
	typedef T value_type; ///< tipo di dato

	/**
	 Costruttore della matrice
	 
	 @param r numero di righe
	 @param c numero di colonne
	 @param d dato di default
	*/
	SparseMatrix(const int r, const int c, const T& d) : size(0), head(0), D(d), righe(r), colonne(c) {
#ifdef DEBUG
		std::cout << "Creazione matrice " << righe << "x" << colonne << std::endl;
#endif
		assert(r > 0);
		assert(c > 0);
	}
	
	/**
	 Distruttore, chiama clear() che chiama clear_helper() a partire da head
	*/
	~SparseMatrix() {
#ifdef DEBUG
		std::cout << "Distruzione matrice " << righe << "x" << colonne << std::endl;
#endif
		clear();
	}

	SparseMatrix& operator=(const SparseMatrix& other) {
		if (this != &other) {
			SparseMatrix tmp(other);
			std::swap(head, tmp.head);
			std::swap(righe, tmp.righe);
			std::swap(colonne, tmp.colonne);
			std::swap(D, tmp.D);
			std::swap(size, tmp.size);
		}

		return *this;
	}
	
	/**
	 Ritorna pubblicamente il numero di elementi attualmente inseriti
	*/
	unsigned int get_size() const {
		return size;
	}
	
	/**
	 Getter per le righe
	*/
	const int get_righe() const {
		return righe;
	}

	/**
	 Getter per le colonne
	*/
	const int get_colonne() const {
		return colonne;
	}
	
	/**
	 Getter per il dato di default
	*/
	const T& get_default() const {
		return D;
	}
	
	/**
	 Setter per il dato di default
	 
	 @param val nuovo valore del dato di default
	*/
	void set_default(const T& val) {
		D = val;
	}
	
	/**
	 Costruttore di copia, se fallisce ripristina lo stato della memoria
	 
	 @param other matrice da copiare
	 @throw eccezione di allocazione di memoria
	*/
	SparseMatrix(const SparseMatrix& other) : head(0), size(0), righe(other.righe), colonne(other.colonne), D(other.D) {
		const node* tmp = other.head;
		try {
			while (tmp != 0) {
				add(tmp->e.riga, tmp->e.colonna, tmp->e.dato);
				tmp = tmp->next;
			}
		}
		catch (...) {
			clear();
			throw;
		}

	}

	/**
	 Costruttore di copia secondario che copia da una matrice di tipi Q diversi da T.
	 Verifico la castabilita' a compile-time con static_cast.
	 Utilizzo gli iteratori per ricopiare i dati.
	 
	 @brief costruttore di copia secondario a partire da una matrice di tipi diversi.
	 
	 @param other matrice da copiare
	 @throw eccezione di allocazione di memoria
	*/
	template <typename Q>
	SparseMatrix(const SparseMatrix<Q>& other) : head(0), size(0), righe(other.get_righe()), colonne(other.get_colonne()) {
		SparseMatrix<Q> tmp(other);
		typename SparseMatrix<Q>::iterator Ib, Ie;
		static_cast<T>((*Ib).dato); //check di castabilita' @ compile-time
		D = (T)other.get_default();
		Ib = tmp.begin();
		Ie = tmp.end();
		try {
			for (; Ib != Ie; ++Ib) {
				add((*Ib).riga, (*Ib).colonna, (T)(*Ib).dato);
			}
		}
		catch (...) {
			clear();
			throw;
		}
	}
	
	/**
	 Metodo per aggiungere un elemento alla matrice. Crea un nuovo nodo e verifica di inserirlo in ordine naturale
	 (da sinistra a destra e dall'alto verso il basso).
	  Se la posizione esiste gia' lo scarta e si limita ad aggiornare il valore nel vecchio nodo.
	  
	  @param r riga
	  @param c colonna
	  @param value valore da mettere nella matrice, di tipo T
	*/
	void add(const int r, const int c, const value_type& value) {
		assert(r <= righe && r > 0);
		assert(c <= colonne && c > 0);
		assert(value != D);
		node* current = new node(value, r, c, 0, 0); ///< anche se fallisce, non ho ancora cambiato lo stato della classe quindi puo' fallire in sicurezza
		if (head == 0) {
#ifdef DEBUG
			std::cout << "Testa vuota, aggiungo l'elemento: " << value << std::endl;
#endif
			head = current;
			++size;
			return;
		}
		node* n = head;
		while (n != 0) {
			if (r < n->e.riga || r == n->e.riga && c < n->e.colonna) {
				if (n->prev == 0) { ///< aggiungo in testa
#ifdef DEBUG
					std::cout << "aggiungo in testa il val " << value << std::endl;
#endif
					current->prev = 0;
					current->next = head;
					head->prev = current;
					head = current;
					++size;
					break;
				}
				else { ///< aggiungo in mezzo
#ifdef DEBUG
					std::cout << "aggiungo in mezzo il val " << value << std::endl;
#endif
					current->next = n->prev->next;
					current->prev = n->prev;
					n->prev->next = current;
					n->prev = current;
					++size;
					break;
				}
			}
			if (r == n->e.riga && c == n->e.colonna) {
#ifdef DEBUG
				std::cout << "aggiorno valore in (" << r << ";" << c << ") con " << current->e.dato << std::endl;
#endif	
				n->e.dato = current->e.dato;
				delete current;
				break;
			}
			if (r > n->e.riga || r == n->e.riga && c > n->e.colonna) {
				if (n->next ==0) { ///< aggiungo in coda
#ifdef DEBUG
					std::cout << "aggiungo in coda il val " << value << std::endl;
#endif
					current->prev = n;
					current->next = 0;
					n->next = current;
					++size;
					break;
				}
				n = n->next;
			}
		}
	}
	
	/**
	 Definizione di operator() sulla matrice. alla richiesta della coppia riga;colonna,
	 ritorna il valore dell'elemento in quella posizione e, se non esistente, ritorna il
	 valore di default.
	 
	 @param r riga
	 @param c colonna
	*/
	const T& operator()(const int r, const int c) const {
		assert(r <= righe && r > 0);
		assert(c <= colonne && c > 0);
		node* n = head;
		while (n != 0) {
			if (n->e.riga == r && n->e.colonna == c)
				return n->e.dato;
			if (n->next == 0)
				return D;
			else
				n = n->next;
		}
		return D; ///< se la matrice e' vuota ritorna il valore di default
	}

#ifdef DEBUG
	/**
	 Metodo di debug per la stampa della matrice.
	*/
	void print() const {
		node* n = head;
		std::cout << "\n****STAMPA DI DEBUG****" << std::endl;
		std::cout << "head: " << head << std::endl;
		std::cout << "size: " << get_size() << std::endl;
		std::cout << "righe: " << get_righe() << std::endl;
		std::cout << "colonne: " << get_colonne() << std::endl;
		std::cout << "valore di default: " << get_default() << std::endl;
		std::cout << "| ";
		while (n != 0) {
			std::cout << n->e.dato << " | ";
			n = n->next;
		}
		std::cout << std::endl << std::endl;
	}
#endif

	/*
	#############
	# ITERATORS #
	#############
	*/
	// Solo se serve anche const_iterator aggiungere la seguente riga
	class const_iterator; // forward declaration
	
	/**
	 Iteratore per lettura e scrittura della matrice
	*/
	class iterator {
		node* n;
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef element value_type;
		typedef ptrdiff_t difference_type;
		typedef element* pointer;
		typedef element& reference;

	
		iterator() : n(0) {}
		
		iterator(const iterator &other) : n(other.n) {}

		iterator& operator=(const iterator &other) {
			n = other.n;

			return *this;
		}

		~iterator() {}

		// Ritorna il dato riferito dall'iteratore (dereferenziamento)
		reference operator*() const {
			return n->e;
		}

		// Ritorna il puntatore al dato riferito dall'iteratore
		pointer operator->() const {
			return &(n->e);
		}

		// Operatore di iterazione post-incremento
		iterator operator++(int) {
			iterator tmp(*this);
			n = n->next;
			
			return tmp;
		}

		// Operatore di iterazione pre-incremento
		iterator& operator++() {
			n = n->next;
			
			return *this;
		}

		// Uguaglianza
		bool operator==(const iterator &other) const {
			return (n == other.n);
		}

		// Diversita'
		bool operator!=(const iterator &other) const {
			return (n != other.n);
		}

		// Solo se serve anche const_iterator aggiungere le seguenti definizioni
		friend class const_iterator;

		// Uguaglianza
		bool operator==(const const_iterator &other) const {
			return (n == other.n);
		}

		// Diversita'
		bool operator!=(const const_iterator &other) const {
			return (n != other.n);
		}

		// Solo se serve anche const_iterator aggiungere le precedenti definizioni

	private:
		//Dati membro

		// La classe container deve essere messa friend dell'iteratore per poter
		// usare il costruttore di inizializzazione.
		friend class SparseMatrix;

		// Costruttore privato di inizializzazione usato dalla classe container
		// tipicamente nei metodi begin e end
		iterator(node* nn) : n(nn) {}
		
		// !!! Eventuali altri metodi privati
		
	}; // classe iterator
	
	/**
	 Ritorna l'iteratore all'inizio della sequenza dati
	*/
	iterator begin() {
		return iterator(head);
	}

	/**
	 Ritorna l'iteratore alla fine della sequenza dati
	*/
	iterator end() {
		return iterator(0);
	}
	
	/**
	 Iteratore costante della matrice (sola lettura)
	*/
	class const_iterator {
		node* n;
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef element value_type;
		typedef ptrdiff_t difference_type;
		typedef const element* pointer;
		typedef const element& reference;

	
		const_iterator() : n(0) {}
		
		const_iterator(const const_iterator &other) : n(other.n) {}

		const_iterator& operator=(const const_iterator &other) {
			n = other.n;

			return *this;
		}

		const_iterator& operator=(const iterator& other) {
			n = other.n;

			return *this;
		}

		~const_iterator() {}

		// Ritorna il dato riferito dall'iteratore (dereferenziamento)
		reference operator*() const {
			return n->e;
		}

		// Ritorna il puntatore al dato riferito dall'iteratore
		pointer operator->() const {
			return &(n->e);
		}
		
		// Operatore di iterazione post-incremento
		const_iterator operator++(int) {
			const_iterator tmp(*this);
			n = n->next;
			return tmp;
		}

		// Operatore di iterazione pre-incremento
		const_iterator& operator++() {
			n = n->next;

			return *this;
		}

		// Uguaglianza
		bool operator==(const const_iterator &other) const {
			return (n == other.n);
		}
		
		// Diversita'
		bool operator!=(const const_iterator &other) const {
			return (n != other.n);
		}

		// Solo se serve anche iterator aggiungere le seguenti definizioni
		
		friend class iterator;

		// Uguaglianza
		bool operator==(const iterator &other) const {
			return (n == other.n);
		}

		// Diversita'
		bool operator!=(const iterator &other) const {
			return (n != other.n);
		}

		// Solo se serve anche iterator aggiungere le precedenti definizioni

	private:
		//Dati membro

		// La classe container deve essere messa friend dell'iteratore per poter
		// usare il costruttore di inizializzazione.
		friend class SparseMatrix; // !!! Da cambiare il nome!

		// Costruttore privato di inizializzazione usato dalla classe container
		// tipicamente nei metodi begin e end
		const_iterator(node* nn) : n(nn) {}
		
		// !!! Eventuali altri metodi privati
		
	}; // classe const_iterator
	
	/**
	 Ritorna l'iteratore constante all'inizio della sequenza dati
	*/
	const_iterator begin() const {
		return const_iterator(head);
	}
	
	/**
	 Ritorna l'iteratore costante alla fine della sequenza dati
	*/
	const_iterator end() const {
		return const_iterator(0);
	}

};

/**
 Funzione globale che, data una SparseMatrix di tipo T e un funtore P,
 conta quanti elementi verificano il predicato descritto dal funtore.
 
 @param M SparseMatrix di tipo T
 @param p predicato
*/
template <typename T, typename P>
const int evaluate(SparseMatrix<T>& M, P& p) {
	int counter = 0;
	for (int i = 1; i <= M.get_righe(); ++i) {
		for (int j = 1; j <= M.get_colonne(); ++j) {
#ifdef DEBUG
			std::cout << "testing (" << i << ";" << j << ")" << std::endl;
#endif
			if (p(M(i, j))) {
				++counter;
#ifdef DEBUG
				std::cout << "Found matching on (" << i << ";" << j << ")" << std::endl;
#endif
			}
		}
	}
	
	return counter;
}

#endif
