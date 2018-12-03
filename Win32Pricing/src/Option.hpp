#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <math.h>


/// \brief Classe Option abstraite
class Option
{
public:
	/*! maturité de l'option */
	double T_;
	/*!  nombre de pas de temps de discrétisation */
	int nbTimeSteps_;
	/*! dimension du modèle, redondant avec BlackScholesModel::size_ */
	int size_;
	/*! poids des sous-jacents */
	PnlVect* weights_;
	/**
	 * Calcule la valeur du payoff sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du modèle telle que créée
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	virtual double payoff(const PnlMat *path) = 0;

	/* destructeur par défaut pour l'option */
	virtual ~Option() = default;
};

