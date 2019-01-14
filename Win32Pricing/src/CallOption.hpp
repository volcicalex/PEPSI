//
// Created by lecalvmy on 9/18/18.
//

#include "Option.hpp"

/// \brief Option Call hérite de la classe abstraite option
class CallOption : public Option
{
public:

	/*! Prix d'exercice de l'option Call */
	double strike_;

	/**
	* Constructeur de la classe
	* @param[in]  T : maturité
	* @param[in]  nbTimeSteps : nombre de pas de temps de discrétisation
	* @param[in]  size : dimension du modèle
	* @param[in] weights : poids des actifs
	* @param[in]  strike : prix d'exercice de l'option
	*/
	CallOption(double T, int nbTimeSteps, int size, PnlVect* weights, double strike);

	/**
	 * Calcule la valeur du payoff sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du modèle telle que créée
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	double payoff(const PnlMat *path) override;
};
