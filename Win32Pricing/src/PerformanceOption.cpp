//
// Created by lecalvmy on 9/18/18.
//

#include "PerformanceOption.hpp"

/**
* Constructeur de la classe
* @param[in] T : maturité
* @param[in] nbTimeSteps : nombre de pas de temps de discrétisation
* @param[in] size : dimension du modèle
* @param[in] weights : poids des actifs
*/
PerformanceOption::PerformanceOption(double T, int nbTimeSteps, int size, PnlVect* weights) {
	T_ = T;
	nbTimeSteps_ = nbTimeSteps;
	size_ = size;
	weights_ = weights;
}

/**
 * Calcule la valeur du payoff sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
 */
double PerformanceOption::payoff(const PnlMat *path) {
	
	double payOff = 1;
	PnlVect* vecteur = pnl_mat_mult_vect(path, weights_);
	double a0 = pnl_vect_get(vecteur, 0);
	double aNext;
	for (int i = 1; i <= nbTimeSteps_; i++)
	{
		aNext = pnl_vect_get(vecteur, i);
		payOff += fmax(( aNext/ a0 )-1.0, 0.0);
		a0 = aNext;
	}
	return payOff;
	pnl_vect_free(&vecteur);
}

