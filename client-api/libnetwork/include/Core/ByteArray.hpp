#ifndef CORE_BYTEARRAY_H
#define CORE_BYTEARRAY_H


#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>

#include "ArgumentException.hpp"


namespace Core
{
	//! Classe permettant de créer et de manipuler un tableau de bytes
	class ByteArray
	{
		private:
		
		int					_capacity;
		int					_size;
		void*				_data;
		
		
		public:
		
		//! @brief Contruit de tableau de bytes vide. 
		ByteArray();
		
		//! @brief Contruit un tableau de bytes définit par sa taille. 
		//! @param n Taille du tableau
		ByteArray(int n);
		
		//! @brief Contruit un tableau de bytes définit par sa taille 
		//!	et atribut une valeur à chacun de ces bytes. 
		//! @param n Taille du tableau
		//! @param value Valeur initiale des éléments.
		ByteArray(int n, char value);
		
		//! @brief Construit un tableau de bytes contenant un seul 
		//! et unique caractère.
		//! @param data Caractère contenu dans le tableau
		ByteArray(char data);
		
		//! @brief Construit de tableau de bytes à partir d'un 
		//! tableau de taille fixé. 
		//! @param data Tableau à copier
		//! @param len Taille du tableau à copier
		ByteArray(const void* data, int len);
		
		//! @brief Contruit un tableau de bytes en utilisant 
		//! la méthode fromCString. 
		//!
		//! Attention, on ne peut pas utiliser ce constructeur pour 
		//! rechercher le caractère nul ou des chaines qui le contient.
		//! @param data Chaine de caractères à copier
		ByteArray(const char* data);
		
		//! @brief Contruit un tableau de bytes en utilisant 
		//! la méthode fromString. 
		//!
		//! Attention, on ne peut pas utiliser ce constructeur pour 
		//! rechercher le caractère nul ou des chaines qui le contient.
		//! @param data Chaine de caractères à copier
		ByteArray(const std::string& data);
		
		//! @brief Contruit un tableau de bytes en utilisant 
		//! la méthode fromVector. 
		//! @param data Vecteur à copier
		ByteArray(const std::vector<char>& data);
		
		//! @brief Constructeur de copie de tableau de bytes. 
		//! @param data Tableau dynamique à copier
		ByteArray(const ByteArray& data);
		
		//! Destructeur de tableau de bytes. 
		virtual ~ByteArray();
		
		//! @brief Redimentionne le tableau dynamique.
		//!
		//! Si le tableau est agrandit, la valeur des bytes 
		//! ajoutés à la fin du tableau est aléatoire.
		//! @param size Nouvelle taille du tableau
		void resize(int size) throw(ArgumentException);
		
		//! @brief Redimentionne le tableau dynamique. 
		//!
		//! Si la nouvelle taille est plus grande que l'ancienne, 
		//! les valeurs ajoutées prennent la valeur de 'value'. 
		//! @param size Nouvelle taille du tableau
		//! @param value Valeur ajoutée à la fin du tableau si il est agrandit
		void resize(int size, char value) throw(ArgumentException);
		
		//! @brief Permet de connaitre la taille du tableau dynamique. 
		//!
		//! Cette méthode est identique à la méthode size. 
		//! @return Taille du tableau dynamique.
		int count() const;
		
		//! @brief Compte le nombre d'occurences d'un byte 
		//! dans le tableau de bytes. 
		//!
		//! @param byte Valeur à compter
		//! @param from Position où la méthode commence à compter
		//! @return Nombre d'occurences trouvées.
		int count(char byte, int from = 0) const;
		
		//! @brief Compte le nombre d'occurences d'un sous-tableau 
		//! dans le tableau de bytes. 
		//! @param ba Sous-tableau à compter
		//! @param from Position où la méthode commence à compter
		//! @return Nombre d'occurences trouvées.
		int count(const ByteArray& ba, int from = 0) const;
		
		//! @brief Recherche un byte dans le tableau de bytes. 
		//! @return Position du byte trouvée ou -1 
		//!			si il est introuvable.
		int find(char byte, int from = 0) const;
		
		//! @brief Recherche un sous-tableau dans le tableau de bytes. 
		//! @return Position du sous-tableau trouvée 
		//!			ou -1 si il est introuvable.
		int find(const ByteArray& ba, int from = 0) const;
		
		//! @brief Permet de connaitre la taille du tableau dynamique. 
		//!
		//! Cette méthode est identique à la méthode count. 
		//! @return Taille du tableau dynamique
		int size() const;
		
		//! @brief Permet de connaitre la capacité du tableau dynamique. 
		//!
		//! La capacité du tableau est la taille réel allouée pour stocker 
		//! les données. <br />
		//! Remarque : l'égallité suivante est toujours respecté : <br />
		//! capacity()/2 < size() <= capacity()
		//! @return Capacité du tableau.
		int capacity() const;
		
		//! @brief Permet de savoir si le tableau de byte est vide ou non. 
		//!
		//! Un tableau est vide si et seulement si la taille est nulle. 
		//! @return True si la taille du tableau est nulle, 
		//!         false dans le cas contraire.
		bool empty() const;
		
		//! @brief Renvoie un sous-tableau débutant à gauche 
		//! du tableau dynamique
		//! @param length Taille du sous-tableau
		//! @return Sous-tableau extrait.
		ByteArray left(int length) const;
		
		//! @brief Renvoie un sous-tableau finissant à droite 
		//! du tableau dynamique
		//! @param length Taille du sous-tableau
		//! @return Sous-tableau extrait.
		ByteArray right(int length) const;
		
		//! @brief Renvoie un sous-tableau du tableau dynamique
		//! @param from Position du sous-tableau dans le tableau dynamique
		//! @param length Taille du sous-tableau
		//! @return Sous-tableau extrait.
		ByteArray mid(int from, int length) const;
		
		//! @brief Permet de définir un tableau de bytes à partir 
		//! d'une chaine de caractères de type C 
		//!
		//! (le caractère nul terminal n'est pas copié). 
		//! Attention, on ne peut pas utiliser cette méthode pour 
		//! rechercher le caractère nul ou des chaines qui le contient.
		//! @param data Chaine de caractères à convertir
		void fromCString(const char* data);
		
		//! @brief Permet de définir un tableau de bytes à partir d'une chaine 
		//! de caractères (le caractère nul terminal n'est pas copié). 
		//!
		//! Attention, on ne peut pas utiliser cette méthode pour 
		//! rechercher le caractère nul ou des chaines qui le contient.
		//! @param data Chaine de caractères à convertir
		void fromString(const std::string& data);
		
		//! @brief Permet de définir un tableau de bytes à partir d'un vecteur. 
		//! @param data Vecteur à convertir
		void fromVector(const std::vector<char>& data);
		
		//! @brief Permet de convertir un tableau de bytes 
		//! en une chaine de caractères. 
		//! @return Chaine de caractères convertie. 
		std::string toString() const;
		
		//! @brief Permet de convertir un tableau de bytes en vecteur. 
		//! @return Vecteur convertie. 
		std::vector<char> toVector() const;
		
		//! @brief Compare un tableau dynamique avec le tableau courant.
		//! @param other Element avec lequel on compare l'élement courant
		//! @return True si les tableaux sont de même taille et 
		//!			contiennent exactement les même valeurs. 
		//!			False dans le cas contraire.
		bool operator==(const ByteArray& other) const;
		
		//! @brief Compare un tableau dynamique avec le tableau courant.
		//! @param other Element avec lequel on compare l'élement courant
		//! @return True si les tableaux sont de tailles différentes ou 
		//!			si il contiennent des valeurs différentes. 
		//!			False dans le cas contraire.
		bool operator!=(const ByteArray& other) const;
		
		//! @brief Concatène un tableau dynamique à la fin de celui-ci.
		//! @param other Tableau dynamique à concatener
		//! @return Référence sur l'élement Concaténé.
		ByteArray& operator+=(const ByteArray& other);
		
		ByteArray& operator=(const ByteArray& other);
		
		//! @brief Accède à une case du tableau dynamique 
		//! à partir de son index et permet sa modification.
		//! @param index Index de la case ciblée
		//! @return Valeur de la case ciblée (modifiable).
		char& operator[](int index);
		
		//! @brief Lit une case du tableau dynamique 
		//! à partir de son index.
		//! @param index Index de la case ciblée
		//! @return Valeur de la case ciblée.
		const char& operator[](int index) const;
		
		
		private:
		
		//! @brief Permet de trouver la puissance de deux supérieure 
		//! ou égale à un nombre.
		//! @param n Nombre dont on veut trouver la puissance de deux 
		//!          supérieure ou égale
		//! @return Puissance de deux supérieure ou égale à n
		unsigned int pow2ge(unsigned int n);
		
		
		friend ByteArray operator+(const ByteArray& ba1, const ByteArray& ba2);
	};
}


#endif


