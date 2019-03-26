//
// Created by kettily.
//

#include "QuantoOption.hpp"
#include "pnl/pnl_finance.h"

/**
* Constructeur de la classe
* @param[in]  T : maturit�
* @param[in]  nbTimeSteps : nombre de pas de temps de discr�tisation
* @param[in]  size : dimension du mod�le
* @param[in] weights : poids des actifs
* @param[in]  strike : prix d'exercice de l'option
*/
QuantoOption::QuantoOption(double T, int nbTimeSteps, int size, double rf, PnlVect* weights, double strike) {
	T_ = T;
	rf_ = rf;
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
double QuantoOption::payoff(const PnlMat *path) {
	pnl_mat_print(path);
	double prix = MGET(path, nbTimeSteps_, 0);
	double strike = MGET(path, nbTimeSteps_, 1)*exp(-rf_ * T_)*strike_;
	return fmax(prix - strike, 0);
}

/**
*Calcul du prix exacte par la formule fermée
*@param[in] Le modele de black-scholes
*/
double QuantoOption::prixExact0(Model *bs, double rho) {
	double S0 = pnl_vect_get(bs->spot_, 0)*exp(T_*(-bs->r_ + rf_ + rho * pnl_vect_get(bs->sigma_, 0)*pnl_vect_get(bs->sigma_, 0)));
	return pnl_bs_call(S0, strike_, T_, bs->r_, 0, pnl_vect_get(bs->sigma_, 0));
}
