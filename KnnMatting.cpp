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

#include "KnnMatting.h"

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include "KdTree.h"

using namespace std;
using namespace Eigen;

using SpMat = SparseMatrix<double>;

KnnMatting::KnnMatting(ImpressionistDoc* doc): doc(doc), height(doc->m_nPaintHeight), width(doc->m_nPaintWidth),
	D(height * width, height * width)
{
	v = VectorXd::Zero(height * width);
	processTrimap();
}

void KnnMatting::runMatting(int k, int lambda)
{
	doc->m_pUI->mattingProgress->value(0);
	doc->m_pUI->mattingProgress->label("step 1/4: construct kd-tree");
	Fl::check();
	
	KdTree kdtree(featureVec, 5);

	doc->m_pUI->mattingProgress->value(25);
	doc->m_pUI->mattingProgress->label("step 2/4: calculate knn");
	Fl::check();
	
	// Construct sparse matrix A
	SpMat A(height * width, height * width);
	vector<Triplet<double>> triplets;
	VectorXd sums = VectorXd::Zero(height * width);
	for (auto& vec : featureVec)
	{
		auto knn = kdtree.getKnn(vec.first, k);
		for (auto& neighbour : knn)
		{
			double val = 1.0 - (neighbour.first - vec.first).lpNorm<1>() / 5.0;
			triplets.emplace_back(vec.second, neighbour.second, val);
			sums(vec.second) += val;
		}
	}
	A.setFromTriplets(triplets.begin(), triplets.end());

	doc->m_pUI->mattingProgress->value(50);
	doc->m_pUI->mattingProgress->label("step 3/4: construct sparse matrices");
	Fl::check();
	
	// Construct sparse matrix D_p and L
	SpMat D_p(height * width, height * width);
	vector<Triplet<double>> diag;
	for (int i = 0; i < height * width; ++i)
		diag.emplace_back(i, i, sums(i));
	D_p.setFromTriplets(diag.begin(), diag.end());
	SpMat L = D_p - A;

	// Construct vector c and Matrix H
	VectorXd c = 2 * lambda * v.transpose();
	SpMat H = 2 * (L + lambda * D);

	doc->m_pUI->mattingProgress->value(75);
	doc->m_pUI->mattingProgress->label("step 4/4: solve for alpha");
	Fl::check();
	
	// Solve for alpha
	VectorXd alpha;
	SparseLU<SpMat, COLAMDOrdering<int>> sparseLU(H);
	alpha = sparseLU.solve(c);
	if (sparseLU.info() != Success)		// failed, try least square
	{
		SparseQR<SpMat, COLAMDOrdering<int>> sparseQR(H);
		alpha = sparseQR.solve(c);
		if (sparseQR.info() != Success)
			throw runtime_error("failed to solve for alpha");
	}

	display(alpha);

	doc->m_pUI->mattingProgress->value(100);
	doc->m_pUI->mattingProgress->label("done!");
	Fl::check();
}

void KnnMatting::processTrimap()
{
	auto* trimap = doc->m_ucPainting;
	auto* orig = doc->m_ucBitmap;
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int index = i * width + j;
			auto* mask = trimap + index * 3;
			auto* pixel = orig + index * 3;
			int sum = mask[0] + mask[1] + mask[2];

			if (sum == 0)
			{
				D.insert(index, index) = 1.0;
			}
			else if (sum == 255 * 3)
			{
				D.insert(index, index) = 1.0;
				v(index) = 1.0;
			}

			VectorXd vec(5);
			vec << pixel[0], pixel[1], pixel[2], i, j;
			featureVec.push_back({vec, index});
		}
}

void KnnMatting::display(Eigen::VectorXd alpha)
{
	auto* paint = doc->m_ucPainting;
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int index = i * width + j;
			double curAlpha = max(0.0, alpha(index));
			curAlpha = min(curAlpha, 1.0);

			auto* pixel = paint + index * 3;
			pixel[0] = pixel[1] = pixel[2] = curAlpha * 255;
		}
	doc->m_pUI->m_paintView->refresh();
}
