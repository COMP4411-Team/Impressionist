/*
 * BayesianMatting.h
 * Implements Bayesian matting proposed by
 *
	@inproceedings{chuang2001bayesian,
	  title={A bayesian approach to digital matting},
	  author={Chuang, Yung-Yu and Curless, Brian and Salesin, David H and Szeliski, Richard},
	  booktitle={Proceedings of the 2001 IEEE Computer Society Conference on Computer Vision and Pattern Recognition. CVPR 2001},
	  volume={2},
	  pages={II--II},
	  year={2001},
	  organization={IEEE}
	}
 *
 *
 * References
 * https://github.com/praveenVnktsh/Bayesian-Matting
 * https://www.ctolib.com/topics-118580.html
 */

#pragma once

#include <vector>
#include <utility>
#include <Eigen/Dense>
#include "ImpBrush.h"

class ImpressionistDoc;

class BayesianMatting
{
public:
	class Pixel
	{
	public:
		Pixel(Eigen::Vector3f c, float w): color(c), weight(w) { }
		Eigen::Vector3f color;
		float weight;
		int cluster{0};
		float minDist{1e5f};
	};
	
	BayesianMatting(ImpressionistDoc* doc);

	void predict();

	int size{19};			// size of grid inspected
	float sigma{8.0f};		// sigma for gaussian kernel
	int minN{10};			// if knowns pixels in the grid is smaller than minN, continue
	int nClusters{5};		// num of clusters in k means
	float kmeansThreshold{1e-3f};
	int kmeansMaxIter{20};
	int optimizerMaxIter{10};
	float optimizerThreshold{1e-3f};
	float sigmaC{3.f};
	int passThreshold{5};	// if more passes than passThreshold are needed, increase the sliding window size

private:
	Eigen::MatrixXf getGaussianFilter();

	void getNeighbour(Point& coords);
	
	void kmeans(std::vector<Pixel>& input);

	void preprocessTrimap();
	
	void getUnknowns();

	void optimize(Eigen::Vector3f color, Eigen::Vector3f fgMean, Eigen::Vector3f bgMean, Eigen::Matrix3f fgCov, Eigen::Matrix3f bgCov);

	std::pair<Eigen::Vector3f, Eigen::Matrix3f> getMeanAndCov(std::vector<Pixel>& cluster);

	void display();
	
	ImpressionistDoc* doc;

	int height, width;
	
	std::vector<Point> unknowns;

	Eigen::MatrixXi mask;	// 0 for unknown, 1 for determined

	// row first
	std::vector<std::vector<Eigen::Vector3f>> foreground, background;
	std::vector<std::vector<float>> alpha;

	std::vector<Pixel> fgPixels, bgPixels;
	float alphaMean;

	Eigen::Vector3f optimumF, optimumB;
	float optimumAlpha, maxLikelihood{std::numeric_limits<float>::lowest()};
};

