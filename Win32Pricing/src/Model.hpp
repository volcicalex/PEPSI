#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"

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

	/**
	 * Calcule une trajectoire du sous-jacent connaissant le
	 * pass� jusqu' � la date t
	 *
	 * @param[out] path  contient une trajectoire du sous-jacent
	 * donn�e jusqu'� l'instant t par la matrice past
	 * @param[in] t date jusqu'� laquelle on connait la trajectoire.
	 * t n'est pas forc�ment une date de discr�tisation
	 * @param[in] nbTimeSteps nombre de pas de constatation
	 * @param[in] T date jusqu'� laquelle on simule la trajectoire
	 * @param[in] rng generateur de nombres aleatoires
	 * @param[in] past trajectoire r�alis�e jusqu'a la date t
	 */
	virtual void asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) = 0;

	/**
	 * Shift d'une trajectoire du sous-jacent
	 *
	 * @param[in]  path contient en input la trajectoire
	 * du sous-jacent
	 * @param[out] shift_path contient la trajectoire path
	 * dont la composante d a �t� shift�e par (1+h)
	 * � partir de la date t.
	 * @param[in] t date � partir de laquelle on shift
	 * @param[in] h pas de diff�rences finies
	 * @param[in] d indice du sous-jacent � shifter
	 * @param[in] timestep pas de constatation du sous-jacent
	 */
	virtual void shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) = 0;
};