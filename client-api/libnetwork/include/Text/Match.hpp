#ifndef REGULAREXPRESSIONS_MATCH_H
#define REGULAREXPRESSIONS_MATCH_H


#include <vector>
#include <stdexcept>

extern "C"
{
	#include <regex.h>
}

#include "Group.hpp"


namespace RegularExpressions
{
	//! @brief Classe contenant des informations d'un test de 
	//!			correspondance d'une expression régulière 
	//!			avec une chaine de caractères
	class Match
	{
		private:
		
		std::vector<Group>	_groups;

		Match(int nbGroups);
		
		
		public:
		
		//! @brief Permet de savoir si la chaine "matché" correspond avec 
		//! l'expression régulière qui l'a "matché".
		//! @return true si la chaine correspond. 
		//!			false dans le cas contraire.
		bool success() const;
		
		//! @brief Compte le nombre de groupes capturés
		//! @return Nombre de groupes capturés.
		int countGroups() const;
		
		//! @brief Récupère un groupe capturé à partir de sa position 
		//! dans l'expression régulière.
		//! @param id position du groupe
		//! @return Groupe à la position 'id'.
		const Group& group(int id) const throw(std::out_of_range);
		
		//! @brief Récupère l'ensemble des groupes capturés.
		//! @return Vecteur contenant l'ensemble des groupes capturés.
		const std::vector<Group>& groups() const;
		
		
		friend class Regex;
	};
}


#endif


