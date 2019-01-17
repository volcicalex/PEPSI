//
// Created by lecalvmy on 9/18/18.
//

#include "PerformanceOption.hpp"

/**
* Constructeur de la classe
* @param[in] T : maturit�
* @param[in] nbTimeSteps : nombre de pas de temps de discr�tisation
* @param[in] size : dimension du mod�le
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
 * contenant une trajectoire du mod�le telle que cr��e
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
	
	/*
	double payOff = 1;
	PnlVect* vecteur = pnl_mat_mult_vect(path, weights_);

	PnlVect* vecteur2 = pnl_vect_create_subvect(vecteur, 1, vecteur->size-1);
	pnl_vect_resize(vecteur2, vecteur->size);
	pnl_vect_div_vect_term(vecteur2,vecteur);

	for (int i = 1; i <= nbTimeSteps_; i++)
	{	
		payOff += fmax(pnl_vect_get(vecteur2, i-1) - 1.0, 0.0);
	}
	return payOff;
	pnl_vect_free(&vecteur);
	pnl_vect_free(&vecteur2);
	*/

}

