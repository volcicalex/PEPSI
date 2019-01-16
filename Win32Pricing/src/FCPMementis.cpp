#include "FCPMementis.hpp"

/**
* Constructeur de la classe
*/
FCPMementis::FCPMementis(int nbTimeSteps) {
	T_ = 12.0;
	nbTimeSteps_ = nbTimeSteps;
	size_ = 25;
	weights_ = pnl_vect_create_from_scalar(size_, 0.04);

	/* Taux continu calcule a partir du taux annuel donne dans la brochure*/
	taux_capitalisation_ = log(1+0.042);

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
 * Calcule le point d'entrée la trajectoire avec le vecteur de performances
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
* Remplit le vecteur de dividendes pour chaque année
*/
void FCPMementis::fill_dividendes(const PnlMat *path) {

	// Dividendes année 1 à 4
	for (int i = 1; i < 5; i++) {
		pnl_vect_set(dividendes_, i, VLO_ * 0.056);
	}

	float dividende_prec = GET(dividendes_, 4);

	fill_performances_plaf(path);

	// Dividendes année 5 à 12
	for (int i = 5; i < nbTimeSteps_ + 1; i++) {
		dividende_prec = VLO_ * fmax(0.6 * dividende_prec/VLO_, GET(performances_plaf_, i));
		pnl_vect_set(dividendes_, i, dividende_prec);
	}
}

/**
* Calcule la valeur de la plus value sur la trajectoire
*
* @param[in] path est une matrice de taille (N+1) x d
* contenant une trajectoire du modèle telle que créée
* par la fonction asset.
* @return plus value
*/
double FCPMementis::calcul_plus_value(const PnlMat *path) {

	/* Remplit les différentes vecteurs nécessaires */
	fill_performances(path);
	PE();
	fill_performances_plaf(path);
	fill_dividendes(path);

	// Vecteur du niveau de dividendes en pourcentage
	PnlVect *niveau_div = pnl_vect_copy(dividendes_);
	pnl_vect_div_scalar(niveau_div, VLO_);

	double somme_dividende = pnl_vect_sum(niveau_div);
	double somme_perf = pnl_vect_sum(performances_plaf_);
	return fmax(somme_perf - somme_dividende, 0);
}


/**
* Calcule la valeur du gain total de l'investisseur sur la trajectoire
*
* @param[in] path est une matrice de taille (N+1) x d
* contenant une trajectoire du modèle telle que créée
* par la fonction asset.
* @return remboursement comprenant dividendes 
*/
double FCPMementis::payoff(const PnlMat *path) {

	double plus_value = calcul_plus_value(path);
	
	PnlVect *flux_capitalises = pnl_vect_create_from_scalar(nbTimeSteps_ + 1, 0);


	for (int annee = 0; annee < nbTimeSteps_; annee++)
	{
		pnl_vect_set(flux_capitalises, annee, GET(dividendes_, annee)*exp(taux_capitalisation_*(T_-annee)));
		/*printf("iteration %i \n", annee);
		printf("dividende : %f \n", GET(dividendes_, annee));
		printf("temps : %f \n", T_-annee);

		printf("taux : %f \n", taux_capitalisation_);
		pnl_vect_print(flux_capitalises);*/
	}
	pnl_vect_set(flux_capitalises, nbTimeSteps_, (GET(dividendes_, nbTimeSteps_)+ VLO_ * (1 + plus_value)));
	/*printf("iteration %i \n", nbTimeSteps_);
	pnl_vect_print(flux_capitalises);*/
	return pnl_vect_sum(flux_capitalises);
}



/**
* Calcule la valeur du remboursement sur la trajectoire
*
* @param[in] path est une matrice de taille (N+1) x d
* contenant une trajectoire du modèle telle que créée
* par la fonction asset.
* @return payoff = phi(trajectoire)
*/
double FCPMementis::remboursement_echeance(const PnlMat *path) {

	double plus_value = calcul_plus_value(path);

	return VLO_ * (1 + plus_value);
}
