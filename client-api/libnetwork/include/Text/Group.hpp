#ifndef REGULAREXPRESSIONS_GROUP_H
#define REGULAREXPRESSIONS_GROUP_H


#include <string>
#include <cstring>


namespace RegularExpressions
{
	//! @brief Classe permettant de manipuler une occurrence d'un 
	//!			groupe capturant dans une expression régulière
	class Group
	{
		private:
		
		int					_start;
		int					_end;
		std::string			_value;
		
		//! Contruit un groupe capturant par défaut
		Group();
		
		//! Contruit un groupe capturant
		//! @param start Position du premier caractère capturé
		//! @param end Position du dernier caractère capturé
		//! @param value Valeur capturé
		Group(int start, int end, const std::string& value);
		
		
		public:
		
		//! @brief Récupère la position du premier 
		//! caractère capturé du groupe capturant.
		//! @return Position du premier caractère capturé.
		int start() const;
		
		//! @brief Récupère la position du dernier
		//! caractère capturé du groupe capturant.
		//! @return Position du premier caractère capturé.
		int end() const;
		
		//! @brief Récupère la valeur capturée par le groupe capturant.
		//! @return Valeur capturée.
		const std::string& value() const;
		
		
		friend class Regex;
		friend class Match;
	};
}


#endif


