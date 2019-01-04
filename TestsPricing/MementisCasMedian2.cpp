#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"

/* Tests les dividendes reçus par l'investisseur dans le "cas defavorable" (cf brochure) */
TEST(propMementis, CasMedian2) {

	printf("=== Cas Median 2 === \n");

	const char *infile = "../data/mementis_casMedian2.txt";
	const PnlMat *path = pnl_mat_create_from_file(infile);

	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	printf("--- Performance du panier d'actions --- \n");

	mementis->fill_performances(path);

	// Resultats calcules dans un fichier excel annexe
	const PnlVect *perf_theorique = pnl_vect_create_from_list(13, 0.000000, 0.917200, 0.930400, 0.911600, 0.911200, 0.960800, 1.019200, 
		1.046400, 1.074000, 1.115200, 1.123200, 1.065600, 1.029600);

	ASSERT_TRUE(pnl_vect_isequal(mementis->performances_, perf_theorique, 0.001));

	printf("--- Point d'entree --- \n");

	mementis->PE();

	// Resultat calcule dans un fichier excel annexe
	double pe_theorique = 0.9112;

	ASSERT_LE(mementis->PE_, pe_theorique + 0.001);
	ASSERT_GE(mementis->PE_, pe_theorique - 0.001);

	printf("--- Dividendes --- \n");

	mementis->fill_dividendes(path);

	// Resultats calcules dans un fichier excel annexe
	const PnlVect *div_theorique = pnl_vect_create_from_list(13, 0.000000, 5.600000, 5.600000, 5.600000, 5.600000, 4.804801, 6.667200,
		9.641600, 9.950400, 9.910399, 9.955199, 9.955199, 8.392002);

	ASSERT_TRUE(pnl_vect_isequal(mementis->dividendes_, div_theorique, 0.001));

	printf("--- Payoff --- \n");
	double payoff = mementis->payoff(path);

	// Resultat calcule dans un fichier excel annexe
	double payoff_theorique = 100;

	ASSERT_LE(payoff, payoff_theorique + 0.001);
	ASSERT_GE(payoff, payoff_theorique - 0.001);

	/* 04/01/19 : soucis dans le calcul du payoff, on obtient 167 alors qu'on devrait avoir 0 : erreur dans la formule ou dans la brochure ? */
}