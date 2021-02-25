#pragma once
#include <Eigen/Dense>
#include "KdTree.h"
#include <vector>
#include <string>

class ImpressionistDoc;

class MosaicMaker
{
public:
	MosaicMaker(ImpressionistDoc* doc);

	bool loadThumbnails(std::string dir);
	
	bool generate(double alpha = 1.0);

private:
	std::vector<std::string> getFilenames(std::string dir);
	
	Eigen::Vector3d retrieveMeanColor(unsigned char* target, int stRow, int stCol, int height, int width);

	void overwrite(int stRow, int stCol, unsigned char* tile, double alpha);

	std::vector<unsigned char*> thumbnails;
	ImpressionistDoc* doc;
	KdTree* kdt{nullptr};
	int tileSize{10};
	int height, width;
};

