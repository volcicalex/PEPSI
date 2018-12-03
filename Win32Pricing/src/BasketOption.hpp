//
// Created by lecalvmy on 9/18/18.
//

#include "Option.hpp"

/// \brief Option Basket h�rite de la classe abstraite option
class BasketOption : public Option
{
public:

	/*! Prix d'exercice de l'option basket */
	double strike_;

	/**
	* Constructeur de la classe
	* @param[in]  T : maturit�
	* @param[in]  nbTimeSteps : nombre de pas de temps de discr�tisation
	* @param[in]  size : dimension du mod�le
	* @param[in] weights : poids des actifs
	* @param[in]  strike : prix d'exercice de l'option
	*/
	BasketOption(double T, int nbTimeSteps, int size, PnlVect* weights, double strike);

	/**
	 * Calcule la valeur du payoff sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du mod�le telle que cr��e
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	double payoff(const PnlMat *path) override;
};

