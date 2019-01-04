#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"

/* Tests les dividendes reçus par l'investisseur dans le "cas favorable" (cf brochure) */
TEST(propMementis, CasFavorable) {

	printf("=== Cas favorable === \n");

	const char *infile = "../data/mementis_casFav.dat";
	const PnlMat *path = pnl_mat_create_from_file(infile);

	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	printf("--- Performance du panier d'actions --- \n");

	mementis->fill_performances(path);

	// Resultats calcules dans un fichier excel annexe
	const PnlVect *perf_theorique = pnl_vect_create_from_list(13, 0.000000, 1.075600, 1.065600, 1.074000, 1.086000, 1.064800, 1.081600,
		1.074800, 1.090800, 1.104400, 1.104000, 1.114000, 1.138400);

	ASSERT_TRUE(pnl_vect_isequal(mementis->performances_, perf_theorique, 0.001));

	printf("--- Point d'entree --- \n");

	mementis->PE();

	// Resultat calcule dans un fichier excel annexe
	double pe_theorique = 1;

	ASSERT_LE(mementis->PE_, pe_theorique + 0.001);
	ASSERT_GE(mementis->PE_, pe_theorique - 0.001);

	printf("--- Dividendes --- \n");

	mementis->fill_dividendes(path);

	// Resultats calcules dans un fichier excel annexe
	const PnlVect *div_theorique = pnl_vect_create_from_list(13, 0.000000, 5.600000, 5.600000, 5.600000, 5.600000, 5.640000, 6.919999, 6.680001, 
		8.440000, 9.279999, 9.199999, 9.120000, 9.520001);

	ASSERT_TRUE(pnl_vect_isequal(mementis->dividendes_, div_theorique, 0.001));

	printf("--- Payoff --- \n");
	double payoff = mementis->payoff(path);

	// Resultat calcule dans un fichier excel annexe
	double payoff_theorique = 121.75;

	ASSERT_LE(payoff, payoff_theorique + 0.001);
	ASSERT_GE(payoff, payoff_theorique - 0.001);
}