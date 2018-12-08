#include "Model.hpp"

/// \brief Modèle de Black Scholes
class BlackScholesModel : public Model
{
public:
	/* Matrice de Cholesky */
	PnlMat *L;
	/* Matrice de loi normale*/
	PnlMat *G;
	/* Vecteur de L*/
	PnlVect *Ld;
	/* Vecteur de G */
	PnlVect *Gi;

	/**
	* Constructeur de la classe
	* @param[in] size : nombre d'actifs du modèle
	* @param[in]  r : taux d'intérêt
	* @param[in]  rho : paramètre de corrélation
	* @param[in]  sigma : vecteur de volatilités
	* @param[in]  spot : valeurs initiales des sous-jacents
	* @param[in]  trend : tendance du modèle
	*/
	BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend);

	/**
	 * Génère une trajectoire du modèle et la stocke dans path
	 *
	 * @param[out] path contient une trajectoire du modèle.
	 * C'est une matrice de taille (nbTimeSteps+1) x d
	 * @param[in] T  maturité
	 * @param[in] rng generateur de nombres aleatoires
	 * @param[in] nbTimeSteps nombre de dates de constatation
	 */
	void asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) override;

	/**
	 * Calcule une trajectoire du sous-jacent connaissant le
	 * passé jusqu' à la date t
	 *
	 * @param[out] path  contient une trajectoire du sous-jacent
	 * donnée jusqu'à l'instant t par la matrice past
	 * @param[in] t date jusqu'à laquelle on connait la trajectoire.
	 * t n'est pas forcément une date de discrétisation
	 * @param[in] nbTimeSteps nombre de pas de constatation
	 * @param[in] T date jusqu'à laquelle on simule la trajectoire
	 * @param[in] rng generateur de nombres aleatoires
	 * @param[in] past trajectoire réalisée jusqu'a la date t
	 */
	void asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past);

	/* Destructeur pour le model de BlackScholes */
	~BlackScholesModel();
};

