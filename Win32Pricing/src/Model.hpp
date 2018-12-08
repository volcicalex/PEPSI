#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include <ctime>
#include <math.h>

/// \brief Classe Model abstraite
class Model
{
public:
	/*! nombre d'actifs du mod�le */
	int size_;
	/*! taux d'int�r�t */
	double r_;
	/*!  param�tre de corr�lation */
	double rho_;
	/*! vecteur de volatilit�s */
	PnlVect *sigma_;
	/*!  valeurs initiales des sous-jacents */
	PnlVect *spot_;
	/*! vecteur des tendances */
	PnlVect *trend_;

	/**
	 * G�n�re une trajectoire du mod�le et la stocke dans path
	 *
	 * @param[out] path contient une trajectoire du mod�le.
	 * C'est une matrice de taille (nbTimeSteps+1) x d
	 * @param[in] T  maturit�
	 * @param[in] rng generateur de nombres aleatoires
	 * @param[in] nbTimeSteps nombre de dates de constatation
	 */
	virtual void asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) = 0;

	/* destructeur par d�faut pour le modele */
	virtual ~Model() = default;
};