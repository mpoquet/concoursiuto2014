#ifndef REGULAREXPRESSIONS_REGEX_H
#define REGULAREXPRESSIONS_REGEX_H


#include <string>
#include <cstring>

extern "C"
{
	#include <regex.h>
}

#include "../Core/ArgumentException.hpp"
#include "Match.hpp"


/// @brief Namespace définissant les outils nécessaires aux traitements 
/// et à la recherche de chaines de caractères
namespace RegularExpressions
{
	//! Classe permettant de manipuler une expression régulière
	class Regex
	{
		private:
		
		regex_t				_preg;
		
		
		public:
		
		//! @brief Construit une expression régulière compilée à partir 
		//! d'une chaine de caractère.
		//!
		//! Si une erreur d'analyse de l'expression régulière s'est 
		//! produite ou qu'un flag est invalide. 
		//! L'exception ArgumentException est dégagée.
		//! @param regex Chaine de caractère contenant l'expression régulière
		//! @param flags Flags modifiant l'expression régulière.
		//!					Celui-ci peut prendre les même valeurs que
		//!					le flags de la fonction regcomp de regex.h
		//!					Les valeurs peuvent être : REG_EXTENDED, 
		//!					REG_ICASE, REG_NOSUB et REG_NEWLINE.
		Regex(const std::string& regex, int flags = 0) throw(Core::ArgumentException);
		
		//! @brief Destructeur de la regex
		//! 
		//! Libère toutes les ressources utilisées par la regex
		~Regex();
		
		//! @brief Indique si la chaîne de caractères spécifiée 
		//! correspond avec l'expression régulière.
		//! @param str Chaîne de caractères dans laquelle une 
		//!				correspondance doit être recherchée. 
		//! @param flags Flags modifiant la correspondance. 
		//!					Celui-ci peut prendre les même valeurs que
		//!					le flags de la fonction regexec de regex.h
		bool isMatch(const std::string& str, int flags = 0) const;
		
		//! @brief Recherche dans la chaîne de caractères spécifiée 
		//!	la première occurrence de l'expression régulière.
		//! @param str Chaîne de caractères dans laquelle une 
		//!				correspondance doit être recherchée. 
		//! @param flags Flags modifiant la correspondance. 
		//!					Celui-ci peut prendre les même valeurs que
		//!					le flags de la fonction regexec de regex.h
		//! @return Objet contenant des informations sur la correspondance.
		Match match(const std::string& str, int flags = 0) const;
		
		//! @brief Remplace dans la chaîne de caractères spécifiée 
		//!			toutes les occurrences de l'expression régulière
		//!			par une autre chaine de caractères.
		//! @param str Chaine de caractère dans laquelle est recherché 
		//!				une occurence de l'expression régulière
		//! @param replace Chaine de caractères de remplacement
		//! @param flags Flags modifiant la recherche des occurrences. 
		//!					Celui-ci peut prendre les même valeurs que
		//!					le flags de la fonction regexec de regex.h
		//! @return Chaine de caractère obtenue après remplacement des 
		//!			occurrences l'expression régulière trouvées.
		std::string replace(const std::string& str, const std::string& replace, int flags = 0) const;
	};
}


#endif


