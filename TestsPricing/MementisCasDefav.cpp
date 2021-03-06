#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"

/* Tests les dividendes re�us par l'investisseur dans le "cas defavorable" (cf brochure) */
TEST(propMementis, CasDefavorable2) {

	//printf("=== Cas defavorable === \n");

	const char *infile = "../data/mementis_casDefav.txt";
	const PnlMat *path = pnl_mat_create_from_file(infile);

	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	//printf("--- Performance du panier d'actions --- \n");

	mementis->fill_performances(path);

	// Resultats calcules dans un fichier excel annexe
	const PnlVect *perf_theorique = pnl_vect_create_from_list(13, 0.000000, 1.103600, 1.111600, 1.106400,
		1.099600, 0.960800, 0.930000, 0.888400, 0.909600, 0.894400, 0.918400, 0.887200, 0.913600);

	ASSERT_TRUE(pnl_vect_isequal(mementis->performances_, perf_theorique, 0.001));

	mementis->fill_performances_plaf(path);

	//printf("--- Point d'entree --- \n");

	mementis->PE();


	// Resultat calcule dans un fichier excel annexe
	double pe_theorique = 1;

	ASSERT_LE(mementis->PE_, pe_theorique + 0.001);
	ASSERT_GE(mementis->PE_, pe_theorique - 0.001);

	//printf("--- Dividendes --- \n");

	mementis->fill_dividendes(path);


	// Resultats calcules dans un fichier excel annexe
	const PnlVect *div_theorique = pnl_vect_create_from_list(13, 0.000000, 5.600000, 5.600000, 5.600000, 5.600000, 3.360000,
		2.016000, 1.209600, 0.725760, 0.435456, 0.261274, 0.156764, 0.094058);

	ASSERT_TRUE(pnl_vect_isequal(mementis->dividendes_, div_theorique, 0.001));

	//printf("--- Payoff echeance --- \n");
	double remb_echeance = mementis->remboursement_echeance(path);

	// Resultat calcule dans un fichier excel annexe
	double remb_echeance_theorique = 100;
	ASSERT_LE(remb_echeance, remb_echeance_theorique + 0.001);
	ASSERT_GE(remb_echeance, remb_echeance_theorique - 0.001);

	//printf("--- Gains actualises --- \n");

	double payoff = mementis->payoff(path);

	payoff *= exp(-mementis->taux_capitalisation_* mementis->T_);

	// Resultat calcule dans un fichier excel annexe
	double payoff_theorique = -12.36146998;

	ASSERT_LE(payoff, payoff_theorique + 0.001);
	ASSERT_GE(payoff, payoff_theorique - 0.001);



}