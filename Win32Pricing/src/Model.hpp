#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include <ctime>
#include <math.h>

/// \brief Classe Model abstraite
class Model
{
public:
	/*! nombre d'actifs du modèle */
	int size_;
	/*! taux d'intérêt */
	double r_;
	/*!  paramètre de corrélation */
	double rho_;
	/*! vecteur de volatilités */
	PnlVect *sigma_;
	/*!  valeurs initiales des sous-jacents */
	PnlVect *spot_;
	/*! vecteur des tendances */
	PnlVect *trend_;

	/**
	 * Génère une trajectoire du modèle et la stocke dans path
	 *
	 * @param[out] path contient une trajectoire du modèle.
	 * C'est une matrice de taille (nbTimeSteps+1) x d
	 * @param[in] T  maturité
	 * @param[in] rng generateur de nombres aleatoires
	 * @param[in] nbTimeSteps nombre de dates de constatation
	 */
	virtual void asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) = 0;

	/* destructeur par défaut pour le modele */
	virtual ~Model() = default;
};