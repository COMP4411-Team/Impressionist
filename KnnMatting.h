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

