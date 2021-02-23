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
 * https://github.com/EpsAvlc/GreenScreenMatting/blob/master/src/bayesian_matting.cpp
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
		Pixel(Eigen::Vector3d c, double w): color(c), weight(w) { }
		Eigen::Vector3d color;
		double weight;
		int cluster{0};
		double minDist{1e5f};
	};
	
	BayesianMatting(ImpressionistDoc* doc);

	void predict();

	int size{19};			// size of grid inspected
	double sigma{8.0};		// sigma for gaussian kernel
	int minN{10};			// if known pixels in the grid is smaller than minN, continue
	int nClusters{5};		// num of clusters in k means
	double kmeansThreshold{1e-3};
	int kmeansMaxIter{20};
	int optimizerMaxIter{50};
	double optimizerThreshold{1e-5};
	double sigmaC{3.0};
	int passThreshold{5};	// if more passes than passThreshold are needed, increase the sliding window size

private:
	void getGaussianFilter();

	void getNeighbour(Point& coords);
	
	void kmeans(std::vector<Pixel>& input);

	void preprocessTrimap();
	
	void getUnknowns();

	void optimize(Eigen::Vector3d color, Eigen::Vector3d fgMean, Eigen::Vector3d bgMean, Eigen::Matrix3d fgCov, Eigen::Matrix3d bgCov);

	std::pair<Eigen::Vector3d, Eigen::Matrix3d> getMeanAndCov(std::vector<Pixel>& cluster);

	void display();

	void setPaintColor(int row, int col, Eigen::Vector3d& color);
	
	ImpressionistDoc* doc;

	int height, width;
	
	std::vector<Point> unknowns;

	Eigen::MatrixXi mask;	// 0 for unknown, 1 for background, 2 for foreground, 3 for predicted
	Eigen::MatrixXd gaussianFilter;

	// row first
	std::vector<std::vector<Eigen::Vector3d>> foreground, background;
	std::vector<std::vector<double>> alpha;

	std::vector<Pixel> fgPixels, bgPixels;
	double alphaMean;

	Eigen::Vector3d optimumF, optimumB;
	double optimumAlpha, maxLikelihood{std::numeric_limits<double>::lowest()};
};

