/*
 * BayesianMatting.cpp
 * Implement Bayesian matting from
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
 */

#include "BayesianMatting.h"

#include "impressionistDoc.h"
#include "impressionistUI.h"

using namespace Eigen;

extern float frand();

BayesianMatting::BayesianMatting(ImpressionistDoc* doc): doc(doc), height(doc->m_nPaintHeight), width(doc->m_nPaintWidth),
	foreground(std::vector<std::vector<Vector3f>>(height, std::vector<Vector3f>(width, {0.f, 0.f, 0.f}))),
	background(std::vector<std::vector<Vector3f>>(height, std::vector<Vector3f>(width, {0.f, 0.f, 0.f}))),
	alpha(std::vector<std::vector<float>>(height, std::vector<float>(width, 0.f))),
	mask(MatrixXf::Zero(height, width))
{
	preprocessTrimap();
}

void BayesianMatting::getUnknowns()
{
	unknowns.clear();
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			if (!mask(i, j))
				unknowns.emplace_back(j, i);
}

void BayesianMatting::optimize(Vector3f color, Vector3f fgMean, Vector3f bgMean, Matrix3f fgCov, Matrix3f bgCov)
{
	float curAlpha = alphaMean, prevLikelihood = 0.f, sigmaC2 = sigmaC * sigmaC;
	Matrix3f fgCovInv = fgCov.inverse(), bgCovInv = bgCov.inverse();
	
	for (int i = 0; i < optimizerMaxIter; ++i)
	{
		// Optimize F and B
		MatrixXf A(6, 6);
		VectorXf b(6);

		A.block<3, 3>(0, 0) = fgCovInv + Matrix3f::Identity() * curAlpha * curAlpha / sigmaC2;
		A.block<3, 3>(0, 3) = Matrix3f::Identity() * curAlpha * (1 - curAlpha) / sigmaC2;
		A.block<3, 3>(3, 0) = A.block<3, 3>(0, 3);
		A.block<3, 3>(3, 3) = bgCovInv + Matrix3f::Identity() * std::pow(1 - curAlpha, 2) / sigmaC2;

		b.segment(0, 3) = fgCovInv * fgMean + color * curAlpha / sigmaC2;
		b.segment(3, 3) = bgCovInv * bgMean + color * (1 - curAlpha) / sigmaC2;

		MatrixXf x = A.colPivHouseholderQr().solve(b);

		Vector3f F = x.segment(0, 3), B = x.segment(3, 3);

		// Optimize alpha
		curAlpha = (color - B).dot(F - B) / (F - B).squaredNorm();
		
		// Calculate likelihood
		float curLikelihood = -(color - curAlpha * F - (1 - curAlpha) * B).squaredNorm() / sigmaC2;
		curLikelihood -= ((F - fgMean).transpose() * fgCovInv * (F - fgMean) / 2).sum();		// degenerate to 1x1 mat
		curLikelihood -= ((B - bgMean).transpose() * bgCovInv * (B - bgMean) / 2).sum();

		if (i && std::abs(curLikelihood - prevLikelihood) < optimizerThreshold)
			break;
		
		if (curLikelihood > maxLikelihood)
		{
			maxLikelihood = curLikelihood;
			optimumAlpha = curAlpha;
			optimumB = B;
			optimumF = F;
		}

		prevLikelihood = curLikelihood;
	}
	
}

void BayesianMatting::predict()
{
	getUnknowns();
	int nUnknowns = unknowns.size(), count = 0;
	
	while(count < nUnknowns)
	{
		for (auto& point : unknowns)
		{
			getNeighbour(point);
			if (fgPixels.size() < minN || bgPixels.size() < minN)
				continue;

			kmeans(fgPixels);
			kmeans(bgPixels);

			std::vector<std::vector<Pixel>> fgClusters(nClusters), bgClusters(nClusters);
			
			for (auto& pixel : fgPixels)
				fgClusters[pixel.cluster].push_back(pixel);

			for (auto& pixel : bgPixels)
				bgClusters[pixel.cluster].push_back(pixel);

			for (int i = 0; i < nClusters; ++i)
			{
				auto fg = getMeanAndCov(fgClusters[i]);
				auto bg = getMeanAndCov(bgClusters[i]);

				auto oriPixel = doc->GetOriginalPixel(point);
				Vector3f color(oriPixel[0], oriPixel[1], oriPixel[2]);
				optimize(color, fg.first, bg.first, fg.second, bg.second);
			}

			int x = point.x, y = point.y;
			foreground[y][x] = optimumF;
			background[y][x] = optimumB;
			alpha[y][x] = optimumAlpha;
			mask(y, x) = 1;
			++count;
		}

		getUnknowns();
	}

	display();
}

// Use k means instead of Orchard-Bouman clustering mentioned in the paper
void BayesianMatting::kmeans(std::vector<Pixel>& input)
{
	int count = 0;
	std::vector<Vector3f> clusters;
	for (int i = 0; i < nClusters; ++i)
	{
		clusters.push_back(input[frand() * input.size()].color);
	}

	while(count++ < kmeansMaxIter)
	{
		bool flag = false;
		std::vector<Vector3f> means;
		for (int i = 0; i < nClusters; ++i)
			means.emplace_back(0, 0, 0);
		
		for (auto& pixel : input)
		{
			for (int i = 0; i < nClusters; ++i)
			{
				float dist = (pixel.color - clusters[i]).norm();
				if (dist < pixel.minDist)
				{
					pixel.minDist = dist;
					pixel.cluster = i;
				}
			}
			means[pixel.cluster] += pixel.color;
		}

		for (int i = 0; i < nClusters; ++i)
		{
			float dist = (clusters[i] - means[i]).norm();
			if (dist > kmeansThreshold)
			{
				flag = true;
				break;
			}
		}

		if (!flag)	break;
		clusters = means;
	}
}

void BayesianMatting::preprocessTrimap()
{
	unsigned char* trimap = doc->m_ucPainting;
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int index = (i * width + j) * 3;
			int sum = trimap[index] + trimap[index + 1] + trimap[index + 2];

			if (!sum)
			{
				mask(i, j) = 1;
				unsigned char* color = doc->GetOriginalPixel(j, i);
				background[i][j] = {color[0], color[1], color[2]};
			}
			else if (sum == 255 * 3)
			{
				mask(i, j) = 1;
				unsigned char* color = doc->GetOriginalPixel(j, i);
				foreground[i][j] = {color[0], color[1], color[2]};
				alpha[i][j] = 1.f;
			}
		}
}

std::pair<Eigen::Vector3f, Eigen::Matrix3f> BayesianMatting::getMeanAndCov(std::vector<Pixel>& cluster)
{
	Vector3f mean(0, 0, 0);
	Matrix3f cov = Matrix3f::Zero();

	float totalWeight = 0.f;

	for (auto& pixel : cluster)
	{
		totalWeight += pixel.weight;
		mean += pixel.color * pixel.weight;
	}

	for (auto& pixel : cluster)
	{
		Vector3f delta = pixel.color - mean;
		cov += pixel.weight * delta * delta.transpose();
	}
	
	return {mean / totalWeight, cov / totalWeight};
}

void BayesianMatting::display()
{
	unsigned char* paint = doc->m_ucPainting;
	memset(paint, 0, height * width * 3);
	
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int index = (i * width + j) * 3;
			Vector3f& color = foreground[i][j];
			paint[index] = color(0);
			paint[index + 1] = color(1);
			paint[index + 2] = color(2);
		}

	doc->m_pUI->m_paintView->refresh();
}

Eigen::MatrixXf BayesianMatting::getGaussianFilter()
{
	MatrixXf kernel(size, size);
    float sum = 0.0, sigma2 = sigma * sigma;
    
    for (int i = 0; i < size; i++) 
        for (int j = 0; j < size; j++) 
		{
        		int x = j - size / 2, y = i - size / 2;
            kernel(i, j) = std::exp(-(x * x + y * y) / (2 * sigma2));
            sum += kernel(i, j);
		}
   
	return kernel / sum;
}

void BayesianMatting::getNeighbour(Point& coords)
{
	int len = size / 2;
	int xMin = max(coords.x - len, 0);
	int xMax = min(coords.x + len, width);
	int yMin = max(coords.y - len, 0);
	int yMax = min(coords.y + len, height);

	fgPixels.clear();
	bgPixels.clear();
	alphaMean = 0.f;
	int count = 0;

	auto kernel = getGaussianFilter();

	for (int i = yMin; i < yMax; ++i)
		for (int j = xMin; j < xMax; ++j)
		{
			if (!mask(i, j))
				continue;
			++count;

			float curAlpha = alpha[i][j];
			alphaMean += curAlpha;
			Vector3f fgColor = foreground[i][j], bgColor = background[i][j];

			float gaussianFalloff = kernel(i - coords.y + size, j - coords.x + size);

			if (fgColor.sum() > 0)
				fgPixels.emplace_back(fgColor, gaussianFalloff * curAlpha * curAlpha);

			if (bgColor.sum() > 0)
				bgPixels.emplace_back(bgColor, gaussianFalloff * (1 - curAlpha) * (1 - curAlpha));
		}

	if (count)
		alphaMean /= count;
}
