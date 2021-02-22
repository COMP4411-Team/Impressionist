/*
 * BayesianMatting.cpp
 * Implement Bayesian matting proposed by
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

#include "BayesianMatting.h"

#include "impressionistDoc.h"
#include "impressionistUI.h"

using namespace Eigen;

extern float frand();

BayesianMatting::BayesianMatting(ImpressionistDoc* doc): doc(doc), height(doc->m_nPaintHeight), width(doc->m_nPaintWidth),
	foreground(std::vector<std::vector<Vector3d>>(height, std::vector<Vector3d>(width, {0.0, 0.0, 0.0}))),
	background(std::vector<std::vector<Vector3d>>(height, std::vector<Vector3d>(width, {0.0, 0.0, 0.0}))),
	alpha(std::vector<std::vector<double>>(height, std::vector<double>(width, 0.0))),
	mask(MatrixXi::Zero(height, width))
{
	/*
	for (auto& vec : alpha)
		for (auto& value : vec)
			assert(value == 0);
	 */
	
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

void BayesianMatting::optimize(Vector3d color, Vector3d fgMean, Vector3d bgMean, Matrix3d fgCov, Matrix3d bgCov)
{
	double curAlpha = alphaMean, prevLikelihood = std::numeric_limits<double>::lowest(), sigmaC2 = sigmaC * sigmaC;
	Matrix3d fgCovInv = fgCov.inverse(), bgCovInv = bgCov.inverse();
	
	for (int i = 0; i < optimizerMaxIter; ++i)
	{
		// Optimize F and B
		MatrixXd A(6, 6);
		VectorXd b(6);

		A.block<3, 3>(0, 0) = fgCovInv + Matrix3d::Identity() * curAlpha * curAlpha / sigmaC2;
		A.block<3, 3>(0, 3) = Matrix3d::Identity() * curAlpha * (1 - curAlpha) / sigmaC2;
		A.block<3, 3>(3, 0) = A.block<3, 3>(0, 3);
		A.block<3, 3>(3, 3) = bgCovInv + Matrix3d::Identity() * (1 - curAlpha) * (1 - curAlpha) / sigmaC2;

		b.segment(0, 3) = fgCovInv * fgMean + color * curAlpha / sigmaC2;
		b.segment(3, 3) = bgCovInv * bgMean + color * (1 - curAlpha) / sigmaC2;

		VectorXd x = A.colPivHouseholderQr().solve(b);

		Vector3d F = x.segment(0, 3), B = x.segment(3, 3);

		// Optimize alpha
		curAlpha = (color - B).dot(F - B) / (F - B).squaredNorm();

		curAlpha = max(0.0, curAlpha);
		curAlpha = min(1.0, curAlpha);
		
		// Calculate likelihood
		double curLikelihood = -(color - curAlpha * F - (1 - curAlpha) * B).squaredNorm() / sigmaC2;
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
	int nUnknowns = unknowns.size(), count = 0, pass = 0, checkpoint = nUnknowns / 20;
	
	while(count < nUnknowns)
	{	
		for (auto& point : unknowns)
		{
			getNeighbour(point);
			if (fgPixels.size() < minN || bgPixels.size() < minN)
				continue;

			auto oriPixel = doc->GetOriginalPixel(point);
			Vector3d color(oriPixel[0], oriPixel[1], oriPixel[2]);

			kmeans(fgPixels);
			kmeans(bgPixels);

			std::vector<std::vector<Pixel>> fgClusters(nClusters), bgClusters(nClusters);
			
			for (auto& pixel : fgPixels)
				fgClusters[pixel.cluster].push_back(pixel);

			for (auto& pixel : bgPixels)
				bgClusters[pixel.cluster].push_back(pixel);

			maxLikelihood = std::numeric_limits<double>::lowest();

			for (int i = 0; i < nClusters; ++i)
			{
				if (!fgClusters[i].size())
					continue;

				auto fg = getMeanAndCov(fgClusters[i]);

				if (isnan(fg.first(0)) || isnan(fg.second(0, 0)))
					continue;
				
				if (!fg.second.fullPivLu().isInvertible())
					continue;
				
				for (int j = 0; j < nClusters; ++j)
				{
					if (!bgClusters[j].size())
						continue;
					
					auto bg = getMeanAndCov(bgClusters[j]);
					if (!bg.second.fullPivLu().isInvertible())
						continue;

					if (isnan(bg.first(0)) || isnan(bg.second(0, 0)))
						continue;
					
					optimize(color, fg.first, bg.first, fg.second, bg.second);
				}
			}

			if (maxLikelihood > std::numeric_limits<double>::lowest())
			{
				int x = point.x, y = point.y;
				foreground[y][x] = optimumF;
				background[y][x] = optimumB;
				alpha[y][x] = optimumAlpha;
				mask(y, x) = 3;
				++count;
			}

			if (count % checkpoint == 0)
			{
				doc->m_pUI->mattingProgress->value(100.f * count / nUnknowns);
				Fl::check();
			}
			
		}

		++pass;
		getUnknowns();

		if (pass > passThreshold)
			size += 4;
	}

	display();
}

// Use k means instead of Orchard-Bouman clustering mentioned in the paper
void BayesianMatting::kmeans(std::vector<Pixel>& input)
{
	int numPoints[5];
	int count = 0;
	std::vector<Vector3d> clusters;
	for (int i = 0; i < nClusters; ++i)
	{
		clusters.push_back(input[frand() * (input.size() - 1)].color);
	}

	while(count++ < kmeansMaxIter)
	{

		bool flag = false;
		std::vector<Vector3d> means;
		for (int i = 0; i < nClusters; ++i) {
			means.emplace_back(0.f, 0.f, 0.f);
			numPoints[i] = 0;
		}
		
		for (auto& pixel : input)
		{
			for (int i = 0; i < nClusters; ++i)
			{
				double dist = (pixel.color - clusters[i]).norm();
				if (dist < pixel.minDist)
				{
					pixel.minDist = dist;
					pixel.cluster = i;
				}
			}
			means[pixel.cluster] += pixel.color;
		}

		for (int i = 0; i < nClusters; ++i) {
			for (int j = 0; j < 3; j++) {
				means[i][j] /= numPoints[i];
			}
		}

		for (int i = 0; i < nClusters; ++i)
		{
			double dist = (clusters[i] - means[i]).norm();
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

			if (!sum)	// background
			{
				mask(i, j) = 1;
				unsigned char* color = doc->GetOriginalPixel(j, i);
				background[i][j] = {(double)color[0], (double)color[1], (double)color[2]};
			}
			else if (sum == 255 * 3)		// foreground
			{
				mask(i, j) = 2;
				unsigned char* color = doc->GetOriginalPixel(j, i);
				foreground[i][j] = {(double)color[0], (double)color[1], (double)color[2]};
				alpha[i][j] = 1.0;
			}
		}

	/*
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			assert(alpha[i][j] <= 1 && alpha[i][j] >= 0);
	 */
}

std::pair<Eigen::Vector3d, Eigen::Matrix3d> BayesianMatting::getMeanAndCov(std::vector<Pixel>& cluster)
{
	Vector3d mean(0, 0, 0);
	Matrix3d cov = Matrix3d::Zero();

	double totalWeight = 0;

	for (auto& pixel : cluster)
	{
		totalWeight += pixel.weight;
		mean += pixel.color * pixel.weight;
	}

	for (auto& pixel : cluster)
	{
		Vector3d delta = pixel.color - mean;
		cov += pixel.weight * delta * delta.transpose();
	}
	
	return {mean / totalWeight, cov / totalWeight + Matrix3d::Identity() * 1e-5};
}

void BayesianMatting::display()
{
	unsigned char* paint = doc->m_ucPainting;
	memset(paint, 0, height * width * 3);

	/*
	Vector3d white{255.f, 255.f, 255.f};
	Vector3d grey{127.f, 127.f, 127.f};
	Vector3d black{0.f, 0.f, 0.f};
	 */
	
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			// int index = (i * width + j) * 3;
			
			/*Vector3d& color = foreground[i][j];
			paint[index] = color(0);
			paint[index + 1] = color(1);
			paint[index + 2] = color(2);*/
			
			setPaintColor(i, j, foreground[i][j]);
			
			/*double curAlpha = alpha[i][j] * 255;
			Vector3d greyScale(curAlpha, curAlpha, curAlpha);
			setPaintColor(i, j, greyScale);*/
		}

	doc->m_pUI->m_paintView->refresh();
}

void BayesianMatting::setPaintColor(int row, int col, Eigen::Vector3d& color)
{
	auto* paint = doc->m_ucPainting;
	int index = (row * width + col) * 3; 
	paint[index] = color(0);
	paint[index + 1] = color(1);
	paint[index + 2] = color(2);
}

void BayesianMatting::getGaussianFilter()
{
	gaussianFilter = MatrixXd::Zero(size, size);
    double sum = 0.0, sigma2 = sigma * sigma;
    
    for (int i = 0; i < size; i++) 
        for (int j = 0; j < size; j++) 
		{
        		int x = j - size / 2, y = i - size / 2;
            gaussianFilter(i, j) = std::exp(-(x * x + y * y) / (2 * sigma2));
            sum += gaussianFilter(i, j);
		}

	gaussianFilter /= sum;
}

void BayesianMatting::getNeighbour(Point& coords)
{
	int len = size / 2;
	int xMin = max(coords.x - len, 0);
	int xMax = min(coords.x + len + 1, width);
	int yMin = max(coords.y - len, 0);
	int yMax = min(coords.y + len + 1, height);

	fgPixels.clear();
	bgPixels.clear();
	alphaMean = 0.0;
	int count = 0;

	getGaussianFilter();

	for (int i = yMin; i < yMax; ++i)
		for (int j = xMin; j < xMax; ++j)
		{
			if (!mask(i, j))
				continue;
			++count;

			double curAlpha = alpha[i][j];

			alphaMean += curAlpha;
			Vector3d fgColor = foreground[i][j], bgColor = background[i][j];

			double gaussianFalloff = gaussianFilter(i - coords.y + size/2, j - coords.x + size/2);

			if (mask(i, j) == 2 || mask(i, j) == 3)
				fgPixels.emplace_back(fgColor, gaussianFalloff * curAlpha * curAlpha);

			if (mask(i, j) == 1 || mask(i, j) == 3)
				bgPixels.emplace_back(bgColor, gaussianFalloff * (1 - curAlpha) * (1 - curAlpha));
		}

	if (count)
		alphaMean /= count;
}
