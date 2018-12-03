#include "FCPMementis.hpp"

/**
* Constructeur de la classe
* @param[in]  T : maturit�
* @param[in]  nbTimeSteps : nombre de pas de temps de discr�tisation
* @param[in]  size : dimension du mod�le
* @param[in] weights : poids des actifs
* @param[in]  strike : prix d'exercice de l'option
*/
FCPMementis::FCPMementis() {
	T_ = 12;
	nbTimeSteps_ = 12;
	size_ = 25;
	weights_ = pnl_vect_create_from_scalar(size_, 0.04);

	VLO_ = size_ * 100;
	dividendes_ = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);
	performances_ = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);
}

/**
* Remplit le vecteur de performances
*
* @return phi(trajectoire)
*/
void FCPMementis::fill_performances(PnlMat *path) {
	double perf;

	for (int i = 1; i < nbTimeSteps_ + 1; i++) {

		perf = 0;
		for (int d=0; d<size_; d++) {

			perf += MGET(path, i, d) / MGET(path, 0, d);
		}
		pnl_vect_set(performances_, i, perf / 25);
	}
}

/**
 * Calcule le point d'entr�e la trajectoire avec le vecteur de performances
 *
 * @return phi(trajectoire)
 */
void FCPMementis::PE() {
	PE_ = 1.0;
	double get;

	for (int i=1; i<5; i++) {

		get = GET(performances_, i);
		if (get < PE_) PE_ = get;
	}
}

/**
* Remplit le vecteur de dividendes pour chaque ann�e
*
* @return phi(trajectoire)
*/
double FCPMementis::fill_dividendes(PnlMat *path) {

	// Dividendes ann�e 1 � 4
	for (int i = 1; i < 5; i++) {
		pnl_vect_set(dividendes_, i, VLO_ * 0.056);
	}

	double dividende_prec = GET(dividendes_, 4);
	double So_d;
	double performance_plafonnee;

	// Dividendes ann�e 5 � 12
	for (int i = 5; i < nbTimeSteps_ + 1; i++) {

		for (int d = 0; d < size_; d++) {
			So_d = MGET(path, 0, d);
			performance_plafonnee += fmin(0.1, (MGET(path, i, d) - So_d * PE_) / So_d);
		}

		dividende_prec = VLO_ * fmax(0.6 * dividende_prec, performance_plafonnee / 25);
		pnl_vect_set(dividendes_, i, dividende_prec);
	}
}

/**
* Calcule la valeur du remboursement sur la trajectoire
*
* @param[in] path est une matrice de taille (N+1) x d
* contenant une trajectoire du mod�le telle que cr��e
* par la fonction asset.
* @return phi(trajectoire)
*/
double FCPMementis::payoff(const PnlMat *path) {

	double payoff;
	double somme_dividende = pnl_vect_sum(dividendes_);
	double mean_perf = pnl_vect_sum(performances_) / nbTimeSteps_;

	return VLO_ * (1 + fmax(mean_perf - 1 - somme_dividende, 0));
}
