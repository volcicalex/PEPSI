#include "FCPMementis.hpp"

/**
* Constructeur de la classe
*/
FCPMementis::FCPMementis(int nbTimeSteps) {
	T_ = 12.0;
	nbTimeSteps_ = nbTimeSteps;
	size_ = 25;
	weights_ = pnl_vect_create_from_scalar(size_, 0.04);

	VLO_ = 100;
	dividendes_ = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);
	performances_ = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);
	performances_plaf_ = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);

}

/**
* Remplit le vecteur de performances
*/
void FCPMementis::fill_performances(const PnlMat *path) {
	double perf;

	for (int i = 1; i < nbTimeSteps_ + 1; i++) {

		perf = 0;
		for (int d=0; d<size_; d++) {

			perf += MGET(path, i, d) / MGET(path, 0, d);
		}
		pnl_vect_set(performances_, i, perf / size_);
	}
}

/**
 * Calcule le point d'entr�e la trajectoire avec le vecteur de performances
 */
void FCPMementis::PE() {
	PE_ = 1.0;
	float get;
	for (int i=1; i<5; i++) {
		get = GET(performances_, i);
		if (get < PE_) PE_ = get;
	}
}

/**
* Remplit le vecteur de performances
*/
void FCPMementis::fill_performances_plaf(const PnlMat *path) {
	double perf_plaf;
	float So_d;
	for (int i = 1; i < nbTimeSteps_ + 1; i++) {

		perf_plaf = 0;
		for (int d = 0; d < size_; d++) {
			So_d = MGET(path, 0, d);
			perf_plaf += fmin(0.1, (MGET(path, i, d) - So_d * PE_) / So_d);
		}
		pnl_vect_set(performances_plaf_, i, perf_plaf / size_);
	}
}

/**
* Remplit le vecteur de dividendes pour chaque ann�e
*/
void FCPMementis::fill_dividendes(const PnlMat *path) {

	// Dividendes ann�e 1 � 4
	for (int i = 1; i < 5; i++) {
		pnl_vect_set(dividendes_, i, VLO_ * 0.056);
	}

	float dividende_prec = GET(dividendes_, 4);

	fill_performances_plaf(path);

	// Dividendes ann�e 5 � 12
	for (int i = 5; i < nbTimeSteps_ + 1; i++) {
		dividende_prec = VLO_ * fmax(0.6 * dividende_prec/VLO_, GET(performances_plaf_, i));
		pnl_vect_set(dividendes_, i, dividende_prec);
	}
}

/**
* Calcule la valeur du remboursement sur la trajectoire
*
* @param[in] path est une matrice de taille (N+1) x d
* contenant une trajectoire du mod�le telle que cr��e
* par la fonction asset.
* @return payoff = phi(trajectoire)
*/
double FCPMementis::payoff(const PnlMat *path) {

	/* Remplit les diff�rentes vecteurs n�cessaires */
	fill_performances(path);
	PE();
	fill_performances_plaf(path);
	fill_dividendes(path);

	// Vecteur du niveau de dividendes en pourcentage
	PnlVect *niveau_div = pnl_vect_copy(dividendes_);
	pnl_vect_div_scalar(niveau_div, VLO_);

	double somme_dividende = pnl_vect_sum(niveau_div);
	double somme_perf = pnl_vect_sum(performances_plaf_);
	double plus_value = fmax(somme_perf - somme_dividende, 0);

	return VLO_ * (1 + plus_value);
}
