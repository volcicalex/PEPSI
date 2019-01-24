#include "pnl/pnl_matrix.h"
#include <math.h>

#pragma once
class DeviationPath
{
public:
	static double standardDeviation(const PnlMat* path, const PnlMat* theoricPath);
};

double standardDeviation(const PnlMat* path, const PnlMat* theoricPath) {

	int deviation;
	int deviation_t = 0;
	int nbTimeSteps = path->m;
	int size = path->n;

	for (int i = 0; i < nbTimeSteps; i++) {
		deviation_t = 0;
		for (int d = 0; d < size; d++) {
			deviation_t += (MGET(path, i, d) - MGET(theoricPath, i, d), 2) * (MGET(path, i, d) - MGET(theoricPath, i, d), 2);
		}
		deviation += sqrt(deviation_t / size);
	}

	return deviation / nbTimeSteps;
}