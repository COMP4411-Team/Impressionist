/*
 * Implement KNN matting
 *
 * Original paper:
 *  @article{chen2013knn,
	  title={KNN matting},
	  author={Chen, Qifeng and Li, Dingzeyu and Tang, Chi-Keung},
	  journal={IEEE transactions on pattern analysis and machine intelligence},
	  volume={35},
	  number={9},
	  pages={2175--2188},
	  year={2013},
	  publisher={IEEE}
	}
 *
 * Reference: https://github.com/MarcoForte/knn-matting
 */

#pragma once

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <vector>
#include <utility>

class ImpressionistDoc;

class KnnMatting
{
	using Data = std::pair<Eigen::VectorXd, int>;

public:
	KnnMatting(ImpressionistDoc* doc);

	void runMatting(int k, int lambda);

private:
	void processTrimap();

	void display(Eigen::VectorXd alpha);

	ImpressionistDoc* doc;

	int height, width;

	Eigen::SparseMatrix<double> D;
	Eigen::VectorXd v;
	
	std::vector<Data> featureVec;
};

