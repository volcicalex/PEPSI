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
* Remplit le vecteur de performances plafonnées dans le cas de differents marchés
* tq path contient : domestique + etrangers en domestique + sans risque en domestique
*/
void FCPMementis::fill_performances_plaf_CR(const PnlMat *path) {
	float So_d;
	double perf_plaf;
	int myForeignAssets;
	int myCurrentAssets;
	double step = T_/(path->m-1);

	for (int i = 1; i < nbTimeSteps_ + 1; i++) {
		perf_plaf = 0;
		myForeignAssets = 0;
		myCurrentAssets = pnl_vect_int_get(nbAssetsPerMarket_, 0); // nombre d'actifs domestiques
		for (int d = 0; d < myCurrentAssets; d++) { // performance des actifs domestiques reste inchangée
			So_d = MGET(path, 0, d);
			perf_plaf += fmin(0.1, (MGET(path, i, d) - So_d * PE_) / So_d);
		}
		for (int m = 1; m < nbMarkets_; m++) {
			myForeignAssets = pnl_vect_int_get(nbAssetsPerMarket_, m);
			for (int d = 0; d < myForeignAssets; d++) {
				So_d = MGET(path, 0, myCurrentAssets + d);
				perf_plaf += fmin(0.1, (MGET(path, i, myCurrentAssets + d) / So_d )
					*(MGET(path, 0, size_ + m - 1) / MGET(path, i, size_ + m - 1))
					*exp(pnl_vect_get(risklessrates_, m) * (i * step)) - PE_);
			}
			myCurrentAssets = myCurrentAssets + myForeignAssets;
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
		dividende_prec = fmax(0.6 * dividende_prec, VLO_ * GET(performances_plaf_, i));
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
	fill_dividendes(path);

	return fmax(pnl_vect_sum(performances_plaf_) - pnl_vect_sum(dividendes_) /VLO_, 0);
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
	if (nbMarkets_  == 1) {
		double res = -VLO_ * exp(taux_capitalisation_ * T_) + remboursement_echeance(path) + GET(dividendes_, nbTimeSteps_);

		for (int annee = 1; annee < nbTimeSteps_; annee++)
			res += GET(dividendes_, annee)*exp(taux_capitalisation_*(T_ - annee));

		return res;
	}
	else {
		//printf("a \n");
		payoff_CR(path);
	}
	
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

/*Change Rate Functions*/

double FCPMementis::payoff_CR(const PnlMat *path) {

	double res = -VLO_ * exp(taux_capitalisation_ * T_) + remboursement_echeance_CR(path) + GET(dividendes_, nbTimeSteps_);

	for (int annee = 1; annee < nbTimeSteps_; annee++)
		res += GET(dividendes_, annee)*exp(taux_capitalisation_*(T_ - annee));

	return res;
}

double FCPMementis::remboursement_echeance_CR(const PnlMat *path) {

	double plus_value = calcul_plus_value_CR(path);

	return VLO_ * (1 + plus_value);
}

double FCPMementis::calcul_plus_value_CR(const PnlMat *path) {

	/* Remplit les différentes vecteurs nécessaires */
	fill_performances_CR(path);
	//printf("fillperf \n");
	PE();
	//printf("pe fait \n");

	fill_dividendes_CR(path);
	//printf("div fait \n");

	return fmax(pnl_vect_sum(performances_plaf_) - pnl_vect_sum(dividendes_) / VLO_, 0);
}

void FCPMementis::fill_dividendes_CR(const PnlMat *path) {

	// Dividendes année 1 à 4
	for (int i = 1; i < 5; i++) {
		pnl_vect_set(dividendes_, i, VLO_ * 0.056);
	}

	float dividende_prec = GET(dividendes_, 4);

	fill_performances_plaf_CR(path);

	// Dividendes année 5 à 12
	for (int i = 5; i < nbTimeSteps_ + 1; i++) {
		dividende_prec = fmax(0.6 * dividende_prec, VLO_ * GET(performances_plaf_, i));
		pnl_vect_set(dividendes_, i, dividende_prec);
	}
}

/**
* Remplit le vecteur de performances dans le cas de differents marchés
* tq path contient : domestique + etrangers en domestique + sans risque en domestique
*/
void FCPMementis::fill_performances_CR(const PnlMat *path) {
	double perf;
	int myForeignAssets;
	int myCurrentAssets;
	double step = T_/(path->m -1);
	
	for (int i = 1; i < nbTimeSteps_ + 1; i++) {
		perf = 0;
		myForeignAssets = 0;
		
		myCurrentAssets = pnl_vect_int_get(nbAssetsPerMarket_, 0); // nombre d'actifs domestiques
		for (int d = 0; d < myCurrentAssets; d++) { // performance des actifs domestiques reste inchangée
			perf += MGET(path, i, d) / MGET(path, 0, d);
		}
		for (int m = 1; m < nbMarkets_; m++) {
			myForeignAssets = pnl_vect_int_get(nbAssetsPerMarket_, m);
			for (int d = 0; d < myForeignAssets; d++) {

				perf += (MGET(path, i, myCurrentAssets + d) / MGET(path, 0, myCurrentAssets + d))
					*(MGET(path, 0, size_ + m -1) / MGET(path, i, size_ + m - 1))
					*exp(pnl_vect_get(risklessrates_,m) * (i * step));
			}
			myCurrentAssets = myCurrentAssets + myForeignAssets;
		}
		pnl_vect_set(performances_, i, perf / size_);
	}
}
