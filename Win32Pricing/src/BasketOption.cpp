//
// Created by lecalvmy on 9/18/18.
//

#include "BasketOption.hpp"

/**
* Constructeur de la classe
* @param[in]  T : maturit�
* @param[in]  nbTimeSteps : nombre de pas de temps de discr�tisation
* @param[in]  size : dimension du mod�le
* @param[in] weights : poids des actifs
* @param[in]  strike : prix d'exercice de l'option
*/
BasketOption::BasketOption(double T, int nbTimeSteps, int size, PnlVect* weights, double strike) {
	T_ = T;
	nbTimeSteps_ = nbTimeSteps;
	size_ = size;
	weights_ = weights;
	strike_ = strike;
}

/**
 * Calcule la valeur du payoff sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du mod�le telle que cr��e
 * par la fonction asset.
 * @return phi(trajectoire)
 */
double BasketOption::payoff(const PnlMat *path) {
	PnlVect *vecteur = pnl_vect_new();
	pnl_mat_get_row(vecteur,path,nbTimeSteps_);
	double sum = pnl_vect_scalar_prod(weights_,vecteur);
	pnl_vect_free(&vecteur);
	return fmax(sum - strike_, 0.0);
}
