//
// kd tree for knn
//

#pragma once

#include <Eigen/Dense>
#include <vector>
#include <queue>
#include <utility>

class KdTree
{
	using Data = std::pair<Eigen::VectorXd, int>;

public:
	class Node
	{
	public:
		Node(Data data, int dimension): data(data), dimension(dimension) { }
		~Node() { delete left; delete right; }
		
		Data data;
		Node* left{nullptr}, *right{nullptr};
		int dimension;
	};

	class Pair
	{
	public:
		Node* node;
		double distance;
		friend bool operator<(const Pair& a, const Pair& b) { return a.distance > b.distance; }
	};

	KdTree(std::vector<Data>& points);

	~KdTree();
	
	Node* build(int l, int r, int dimension);

	Data& getMedian(int l, int r, int dimension);

	std::vector<Data> getKnn(Eigen::VectorXd point, int k);
	
	std::vector<Data>& points;

private:
	void traverse(Eigen::VectorXd& point, Node* cur);

	std::priority_queue<Pair> knn;

	Node* root{nullptr};
};

