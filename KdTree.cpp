#include "KdTree.h"
#include <algorithm>
#include <cmath>

using namespace Eigen;
using namespace std;

using Data = pair<VectorXd, int>;

// Use sort for now, switch to quick selection if performance issue occurs
Data& KdTree::getMedian(int l, int r, int dimension)
{
	sort(points.begin() + l, points.begin() + r, 
		[dimension] (const Data& a, const Data& b) { return a.first(dimension) < b.first(dimension); });
	return points[(l + r) / 2];
}

vector<Data> KdTree::getKnn(VectorXd point, int k)
{
	knn = priority_queue<Pair>{less<Pair>(), vector<Pair>{(unsigned)k, {nullptr, numeric_limits<double>::lowest()}}};
	traverse(point, root);
	vector<Data> result;
	while (!knn.empty())
	{
		auto cur = knn.top();
		knn.pop();
		if (cur.node != nullptr) 
			result.push_back(cur.node->data);
	}
	return result;
}

void KdTree::traverse(Eigen::VectorXd& point, Node* cur)
{
	if (cur == nullptr)	return;
	double dist = point(cur->dimension) - cur->data.first(cur->dimension);

	if (dist < 0) traverse(point, cur->left);
	else traverse(point, cur->right);
	
	double cur_dist = (point - cur->data.first).norm();

	knn.push({cur, -cur_dist});
	knn.pop();

	if (-knn.top().distance > std::abs(dist))
	{
		if (dist < 0) traverse(point, cur->right);
		else traverse(point, cur->left);
	}
}

KdTree::KdTree(std::vector<Data>& points, int dimension): points(points), totDim(dimension)
{
	root = build(0, points.size(), 0);
}

KdTree::~KdTree()
{
	delete root;
}

KdTree::Node* KdTree::build(int l, int r, int dimension)
{
	if (l >= r)	return nullptr;
	Node* cur = new Node(getMedian(l, r, dimension), dimension);
	cur->left = build(l, (l + r) / 2, (dimension + 1) % totDim);
	cur->right = build((l + r) / 2 + 1, r, (dimension + 1) % totDim);
	return cur;
}
