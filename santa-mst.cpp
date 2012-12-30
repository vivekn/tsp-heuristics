// Approx MST Heuristic for the Travelling Salesman Problem
// Author: Vivek Narayanan
// Date: 28th December 2012

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <bitset>
#include <queue>
using namespace std;

struct node {
	int id;
	int x;
	int y;
	node(int a=0, int b=0, int c=0) : id(a), x(b), y(c) {}
};


vector<int> hsort, vsort, path1, path2, next1, next2, revxmap, revymap, spantree[150000], walk;
vector<node> cities;
bitset<150000> intree;
node current;

float  euclid(node, node);

struct edge {
	int id1;
	int id2;
	edge(int x, int y) : id1(x), id2(y) {}
	const bool operator<(const edge& rhs) const {
		return euclid(cities[id1], cities[id2]) > euclid(cities[rhs.id1], cities[rhs.id2]);
	}
};

bool xcmp(int a, int b) {
	return cities[a].x < cities[b].x;
}

bool ycmp(int a, int b) {
	return cities[a].y < cities[b].y;
}

float  euclid(node a, node b) {
	return sqrt((a.x-b.x) * (a.x-b.x) + (a.y - b.y) * (a.y - b.y));
}

bool distance_metric(int a, int b) {
	return euclid(cities[a], current) < euclid(cities[b], current);
}

void sortxy() {
	hsort.resize(150000, 0);
	for(int i=0; i < 150000; i++) hsort[i] = i;
	sort(hsort.begin(), hsort.end(), xcmp);
	revxmap.resize(150000, -1);
	for(int i=0; i < 150000; i++) revxmap[hsort[i]] = i;

	vsort.resize(150000, 0);
	for(int i=0; i < 150000; i++) vsort[i] = i;
	sort(vsort.begin(), vsort.end(), ycmp);
	revymap.resize(150000, -1);
	for(int i=0; i < 150000; i++) revymap[vsort[i]] = i;
}

void walk_tree(int city) {
	if (intree[city]) return;
	walk.push_back(city);
	intree[city] = 1;
	// Traverse children
	for(int i=0; i < (int) spantree[city].size(); i++) {
		walk_tree(spantree[city][i]);
	}
	walk.push_back(city);
}

void build_mst() {
	intree.reset();
	priority_queue<edge> heap;
	heap.push(edge(hsort[0], hsort[0]));
	while (heap.size() > 0) {
		edge curr = heap.top();
		heap.pop();
		if (intree[curr.id2]) continue;
		intree[curr.id2] = 1;
		if (curr.id1 != curr.id2) {
			spantree[curr.id1].push_back(curr.id2);
			spantree[curr.id2].push_back(curr.id1);
		}

		int i = revxmap[curr.id2];
		for(int j=i-1; j >= i-10 && j >= 0; j--)
			if (!intree[hsort[j]])
				heap.push(edge(curr.id2, hsort[j]));
		for(int j=i+1; j <= i+10 && j < 150000; j++)
			if (!intree[hsort[j]])
				heap.push(edge(curr.id2, hsort[j]));

		i = revymap[curr.id2];
		for(int j=i-1; j >= i-10 && j >= 0; j--)
			if (!intree[vsort[j]])
				heap.push(edge(curr.id2, vsort[j]));
		for(int j=i+1; j <= i+10 && j < 150000; j++)
			if (!intree[vsort[j]])
				heap.push(edge(curr.id2, vsort[j]));
		
		for(int j=0; j < 20; j++) {
			int r = rand() % 150000;
			if (!intree[r])
				heap.push(edge(curr.id2, r));
		}
		// Best candidates
		vector<int> candidates;
		i = revxmap[curr.id2];
		for(int j=i-1; j >= i-100 && j >= 0; j--)
			if (!intree[hsort[j]])
				candidates.push_back(hsort[j]);
		for(int j=i+1; j <= i+100 && j < 150000; j++)
			if (!intree[hsort[j]])
				candidates.push_back(hsort[j]);

		i = revymap[curr.id2];
		for(int j=i-1; j >= i-100 && j >= 0; j--)
			if (!intree[vsort[j]])
				candidates.push_back(vsort[j]);
		for(int j=i+1; j <= i+100 && j < 150000; j++)
			if (!intree[vsort[j]])
				candidates.push_back(vsort[j]);
		
		for(int j=0; j < 20; j++) {
			int r = rand() % 150000;
			if (!intree[r])
				candidates.push_back(r);
		}
		sort(candidates.begin(), candidates.end(), distance_metric);
		for(int i=0; i < min(10, (int) candidates.size()); i++)
			heap.push(edge(curr.id2, candidates[i]));

	}
	intree.reset();
	walk_tree(hsort[0]);

}

void build_path1() {
	intree.reset();
	next1.resize(150000, -1);
	next2.resize(150000, -1);
	int prev = -1;
	for(int i=0; i < (int) walk.size(); i++) {
		if (!intree[walk[i]]) {
			intree[walk[i]] = 1;
			path1.push_back(walk[i]);
			if (prev != -1) {
				next1[prev] = walk[i];
				next2[walk[i]] = prev;
			}
			prev = walk[i];
		}
	}
	assert(path1.size() == 150000);
}

void build_path2() {
	intree.reset(); 
	int prev = walk[rand() % 150000];
	path2.push_back(prev);
	intree[prev] = 1;
	while (path2.size() < 150000) {
		for(int i=0; i < (int) walk.size(); i++) {
			if (!intree[walk[i]] && next1[prev] != walk[i] && next2[prev] != walk[i]) {
				intree[walk[i]] = 1;
				path2.push_back(walk[i]);
				prev = walk[i];
			}
		}
	}
	assert(path2.size() == 150000);

}

double euc1(int a, int b) {
	if (a < 0 || b < 0 || a >= 150000 || b >= 150000) return 0.0;
	return euclid(cities[path1[a]], cities[path1[b]]);
}

double euc2(int a, int b) {
	if (a < 0 || b < 0 || a >= 150000 || b >= 150000) return 0.0;
	return euclid(cities[path2[a]], cities[path2[b]]);
}

void optimize_path1() {
	// 2-opt based optimization
	int scnt = 0;
	for(int i=0; i < 1000; i++) {
		scnt = 0;
		for(int x=0; x < 149999; x++) {
			int y = x+1;
			double subt = euc1(x, x-1) + euc1(y, y+1);
			double add = euc1(y, x-1) + euc1(x, y+1);
			if (add < subt) {
				swap(path1[x], path1[y]);
				scnt++;
			}
		}
		if (scnt == 0) break;
		printf("%d\n", scnt);	
	}
	for(int i=1; i < 150000; i++) {
		next2[path1[i]] = path1[i-1];
		next1[path1[i-1]] = path1[i];
	}
}

void optimize_path2() {
	// 2-opt based optimization
	int scnt = 0;
	for(int i=0; i < 1000; i++) {
		scnt = 0;
		for(int x=0; x < 149999; x++) {
			int y = x+1;

			bool c1 =  (x > 0) ? ((next1[path2[y]] != path2[x-1]) && next2[path2[y]] != path2[x-1]) : 1;
			bool c4 =  (y < 149999) ? ((next1[path2[y+1]] != path2[x]) && next2[path2[y+1]] != path2[x]) : 1;
			if (c1 && c4) {
				double subt = euc2(x, x-1) + euc2(y, y+1);
				double add = euc2(y, x-1) + euc2(x, y+1);
				if (add < subt) {
					swap(path2[x], path2[y]);
					scnt++;
				}
			}
		}
		if (scnt == 0) break;
		printf("%d\n", scnt);
		}	
}

int main() {
	// Read Data
	freopen("santa2.csv", "r", stdin);
	for(int i=0; i < 150000; i++) {
		int x, y, z;
		scanf("%d %d %d", &x, &y, &z);
		cities.push_back(node(x, y, z));
	}

	// Sort indices on x and y coordinates
	sortxy();

	// Approximate minimum spanning tree
	build_mst();

	// Build first path
	build_path1();
	optimize_path1();
	// Build second path
	build_path2();
	optimize_path2();

	// Print metric
	float  score1 = 0.0, score2 = 0.0;
	for(int i=1; i < 150000; i++) {
		score1 += euclid(cities[path1[i-1]], cities[path1[i]]);
		score2 += euclid(cities[path2[i-1]], cities[path2[i]]);
	}
	printf("%lf %lf\n", score1, score2);

	// Write output
	freopen("output.csv", "w", stdout);
	for(int i=0; i < 150000; i++) printf("%d,%d\n", path1[i], path2[i]);

	return 0;
}