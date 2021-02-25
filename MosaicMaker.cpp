#include "MosaicMaker.h"

#include "KdTree.h"
#include "ImpressionistUI.h"
#include "ImpressionistDoc.h"
#include "Bitmap.h"
#include <Windows.h>
#include <utility>

using namespace std;
using namespace Eigen;
using Data = pair<VectorXd, int>;

MosaicMaker::MosaicMaker(ImpressionistDoc* doc): doc(doc) { }

bool MosaicMaker::loadThumbnails(string dir)
{
	thumbnails.clear();
	auto filenames = getFilenames(dir);
	for (auto& filename : filenames)
	{
		int h, w;
		unsigned char* pixels;
		if ( (pixels = readBMP((dir + "/" + filename).c_str(), w, h)) != nullptr )
		{
			if (h != tileSize || w != tileSize) continue;
			thumbnails.push_back(pixels);
		}
	}

	if (thumbnails.size() < 100)		// too few thumbnails
		return false;

	vector<Data> vectors;
	for (int i = 0; i < thumbnails.size(); ++i)
	{
		vectors.emplace_back(retrieveMeanColor(thumbnails[i], 0, 0, tileSize, tileSize), i);
	}

	delete kdt;
	kdt = new KdTree(vectors, 3);
	return true;
}

bool MosaicMaker::generate(double alpha)
{
	if (thumbnails.size() < 100)
		return false;
	height = doc->m_nPaintHeight;
	width = doc->m_nPaintWidth;
	for (int i = 0; i < height; i += 10)
		for (int j = 0; j < width; j += 10)
		{
			auto mean = retrieveMeanColor(doc->m_ucBitmap, i, j, height, width);
			auto knn = kdt->getKnn(mean, 1);
			overwrite(i, j, thumbnails[knn[0].second], alpha);
		}
	return true;
}

// Source: https://stackoverflow.com/a/20847429
std::vector<std::string> MosaicMaker::getFilenames(std::string dir)
{
	vector<string> filenames;
	string searchPath = dir + "/*.bmp";
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(searchPath.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				filenames.push_back(fd.cFileName);
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	} 
    return filenames;
}

Eigen::Vector3d MosaicMaker::retrieveMeanColor(unsigned char* target, int stRow, int stCol, int height, int width)
{
	Eigen::Vector3d mean(0, 0, 0);
	int endRow = stRow + tileSize < height ? stRow + tileSize : height;
	int endCol = stCol + tileSize < width ? stCol + tileSize : width;
	for (int i = stRow; i < endRow; ++i)
		for (int j = stCol; j < endCol; ++j)
		{
			auto* pixel = target + (i * width + j) * 3;
			for (int k = 0; k < 3; ++k)
				mean(k) += pixel[k];
		}
	return mean / (endCol - stCol) / (endRow - stRow);
}

void MosaicMaker::overwrite(int stRow, int stCol, unsigned char* tile, double alpha)
{
	auto* paint = doc->m_ucPainting;
	auto* bitmap = doc->m_ucBitmap;
	for (int y = stRow, i = 0; y < stRow + tileSize && y < height; ++y, ++i)
		for (int x = stCol, j = 0; x < stCol + tileSize && x < width; ++x, ++j)
		{
			auto* dst = paint + (y * width + x) * 3;
			auto* src = tile + (i * tileSize + j) * 3;
			auto* orig = bitmap + (y * width + x) * 3;

			for (int k = 0; k < 3; ++k)
				dst[k] = alpha * src[k] + (1.0 - alpha) * orig[k];
		}
}


