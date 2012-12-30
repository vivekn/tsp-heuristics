// Heuristics for the Travelling Salesman Problem
// Author: Vivek Narayanan
// Date: 28th December 2012

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <bitset>
using namespace std;

struct node {
	int id;
	int x;
	int y;
	node(int a=0, int b=0, int c=0) : id(a), x(b), y(c) {}
};

vector<int> hsort, vsort, path1, path2, next1, next2, revxmap, revymap;
vector<node> cities;
bitset<150000> sel1, sel2;
node current;

bool xcmp(int a, int b) {
	return cities[a].x < cities[b].x;
}

bool ycmp(int a, int b) {
	return cities[a].y < cities[b].y;
}

double euclid(node a, node b) {
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

void build_path1() {
	next1.resize(150000, -1);
	next2.resize(150000, -1);
	sel1.reset();
	path1.push_back(hsort[0]);
	sel1[hsort[0]] = 1;
	int prev = hsort[0];
	for(int t=1; t < 150000; t++) {
		vector<int> candidates;
		int i = revxmap[prev];
		for(int j=i-1; j >= i-10000 && j >= 0; j--)
			if (!sel1[hsort[j]])
				candidates.push_back(hsort[j]);
		for(int j=i+1; j <= i+10000 && j < 150000; j++)
			if (!sel1[hsort[j]])
				candidates.push_back(hsort[j]);

		i = revymap[prev];
		for(int j=i-1; j >= i-10000 && j >= 0; j--)
			if (!sel1[vsort[j]])
				candidates.push_back(vsort[j]);
		for(int j=i+1; j <= i+10000 && j < 150000; j++)
			if (!sel1[vsort[j]])
				candidates.push_back(vsort[j]);
		current = cities[prev];
		if(candidates.size() == 0) {
			int trials = 1;
			while((candidates.size() == 0) || trials < 2000) {
				int r = rand() % 150000;
				while(sel1[r] == 1) {
					r = rand() % 150000;
					++trials;
				}
				candidates.push_back(r);
			}
		}
		int mincity = *min_element(candidates.begin(), candidates.end(), distance_metric);
		path1.push_back(mincity);
		sel1[mincity] = 1;
		next1[prev] = mincity;
		next2[mincity] = prev;
		prev = mincity;
	}
	assert(path1.size() == 150000);
}

void build_path2() {
	sel2.reset();
	path2.push_back(hsort[0]);
	sel2[hsort[0]] = 1;
	int prev = hsort[0];
	for(int t=1; t < 150000; t++) {
		vector<int> candidates;
		int i = revxmap[prev];
		for(int j=i-1; j >= i-10000 && j >= 0; j--)
			if (!sel2[hsort[j]] && (next1[prev] != hsort[j]) && (next2[prev] != hsort[j]))
				candidates.push_back(hsort[j]);
		for(int j=i+1; j <= i+10000 && j < 150000; j++)
			if (!sel2[hsort[j]] && (next1[prev] != hsort[j]) && (next2[prev] != hsort[j]))
				candidates.push_back(hsort[j]);

		i = revymap[prev];
		for(int j=i-1; j >= i-10000 && j >= 0; j--)
			if (!sel2[vsort[j]] && (next1[prev] != vsort[j]) && (next2[prev] != vsort[j]))
				candidates.push_back(vsort[j]);
		for(int j=i+1; j <= i+10000 && j < 150000; j++)
			if (!sel2[vsort[j]] && (next1[prev] != vsort[j]) && (next2[prev] != vsort[j]))
				candidates.push_back(vsort[j]);
		current = cities[prev];
		if(candidates.size() == 0) {
			int trials = 1;
			while(candidates.size() == 0 || trials < 2000) {
				int r = rand() % 150000;
				while(sel2[r] == 1 || (next1[prev] == r) || (next2[prev] == r)) {
					r = rand() % 150000;
					++trials;
				}
				candidates.push_back(r);
			}			
		}		
		int mincity = *min_element(candidates.begin(), candidates.end(), distance_metric);
		path2.push_back(mincity);
		sel2[mincity] = 1;
		prev = mincity;
	}
	assert(path2.size() == 150000);
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
	// Build first path
	build_path1();
	// Build second path
	build_path2();

	// Print metric
	double score1 = 0.0, score2 = 0.0;
	for(int i=1; i < 150000; i++) {
		score1 += euclid(cities[path1[i-1]], cities[path1[i]]);
		score2 += euclid(cities[path2[i-1]], cities[path2[i]]);
	}
	printf("%lf %lf\n", score1, score2);
	// printf("%d %d\n", randcnt, randcnt2);

	// Write output
	freopen("output.csv", "w", stdout);
	for(int i=0; i < 150000; i++) printf("%d,%d\n", path1[i], path2[i]);

	return 0;
}