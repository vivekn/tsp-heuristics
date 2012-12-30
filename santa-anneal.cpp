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
		for(int j=i-1; j >= i-1000  && j >= 0; j--)
			if (!sel1[hsort[j]])
				candidates.push_back(hsort[j]);
		for(int j=i+1; j <= i+1000  && j < 150000; j++)
			if (!sel1[hsort[j]])
				candidates.push_back(hsort[j]);

		i = revymap[prev];
		for(int j=i-1; j >= i-1000  && j >= 0; j--)
			if (!sel1[vsort[j]])
				candidates.push_back(vsort[j]);
		for(int j=i+1; j <= i+1000  && j < 150000; j++)
			if (!sel1[vsort[j]])
				candidates.push_back(vsort[j]);
		current = cities[prev];
		if(candidates.size() == 0) {
			int trials = 1;
			while((candidates.size() == 0) || trials < 200) {
				int r = rand () % 150000;
				while(sel1[r] == 1) {
					r = rand () % 150000;
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
		for(int j=i-1; j >= i-1000  && j >= 0; j--)
			if (!sel2[hsort[j]] && (next1[prev] != hsort[j]) && (next2[prev] != hsort[j]))
				candidates.push_back(hsort[j]);
		for(int j=i+1; j <= i+1000  && j < 150000; j++)
			if (!sel2[hsort[j]] && (next1[prev] != hsort[j]) && (next2[prev] != hsort[j]))
				candidates.push_back(hsort[j]);

		i = revymap[prev];
		for(int j=i-1; j >= i-1000  && j >= 0; j--)
			if (!sel2[vsort[j]] && (next1[prev] != vsort[j]) && (next2[prev] != vsort[j]))
				candidates.push_back(vsort[j]);
		for(int j=i+1; j <= i+1000  && j < 150000; j++)
			if (!sel2[vsort[j]] && (next1[prev] != vsort[j]) && (next2[prev] != vsort[j]))
				candidates.push_back(vsort[j]);
		current = cities[prev];
		if(candidates.size() == 0) {
			int trials = 1;
			while(candidates.size() == 0 || trials < 200) {
				int r = rand ()  % 150000;
				while(sel2[r] == 1 || (next1[prev] == r) || (next2[prev] == r)) {
					r = rand ()  % 150000;
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

float  euc1(int a, int b) {
	if (a < 0 || b < 0 || a >= 150000 || b >= 150000) return 0.0;
	return euclid(cities[path1[a]], cities[path1[b]]);
}

float  euc2(int a, int b) {
	if (a < 0 || b < 0 || a >= 150000 || b >= 150000) return 0.0;
	return euclid(cities[path2[a]], cities[path2[b]]);
}

float  find_cost(const vector<int> &ref) {
	float  cost = 0.0;
	for(int i=1; i < ref.size(); i++)
		cost += euclid(cities[ref[i]], cities[ref[i-1]]);
	return cost;
}

void optimize_path1 (vector<int> &pathx ) {
	// 2-opt based optimization
	float score = 0.0;
	for(int i=0; i < 149999; i++) score += euc1(i, i+1);
	printf("%f\n", score);
	int scnt = 0;
	for(int i=0; i < 1000; i++) {
		scnt = 0;
		for(int x=0; x < 149999; x++) {
			int y = x+1;
			float  subt = euc1(x, x-1) + euc1(y, y+1);
			float  add = euc1(y, x-1) + euc1(x, y+1);
			if (add < subt) {
				swap(pathx [x], pathx [y]);
				scnt++;
			}
		}
		if (scnt == 0) break;
	}
}

void optimize_path2 (vector<int> &pathx ) {
	// 2-opt based optimization
	float score = 0.0;
	for(int i=0; i < 149999; i++) score += euc2(i, i+1);
	printf("%f\n", score);
	int scnt = 0;
	for(int i=0; i < 1000; i++) {
		scnt = 0;
		for(int x=0; x < 149999; x++) {
			int y = x+1;
			bool c1 =  (x > 0) ? ((next1[pathx [y]] != pathx [x-1]) && next2[pathx [y]] != pathx [x-1]) : 1;
			bool c4 =  (y < 149999) ? ((next1[pathx [y+1]] != pathx [x]) && next2[pathx [y+1]] != pathx [x]) : 1;
			if (c1 && c4) {
				float  subt = euc2(x, x-1) + euc2(y, y+1);
				float  add = euc2(y, x-1) + euc2(x, y+1);
				if (add < subt) {
					swap(pathx [x], pathx [y]);
					scnt++;
				}
			}
		}
		if (scnt == 0) break;
	}
}

void anneal_path1(int max_iter=100) {
	// Simulated Annealing
	float  temp = 2500000, alpha = 1;
	vector<int> best = path1;
	float  minc = find_cost(path1);
	printf("%lf\n", minc);
	for(int i=0; i < 10; i++) {
		vector<int> curr(best.begin(), best.end());
		for(int j=0; j < max_iter; j++) {
			// temp = temp * alpha;
			int x = rand ()  % 149999;
			int y = x+1;
			// int y = rand ()  % 150000;
			float  subt = euc1(x, x-1) + euc1(y, y+1);
			float  add = euc1(y, x-1) + euc1(x, y+1);
			if (add <= subt) {
				swap(curr[x], curr[y]);
			}
			else if (j < 2000000) {
				float  r = (float ) rand ()  / RAND_MAX;
				if (exp((subt-add) / temp) > r) {
					swap(curr[x], curr[y]);
				}
			}
		}
		optimize_path1(curr);
		if (find_cost(curr) < minc) {
			best = curr;
			minc = find_cost(curr);
			printf("%lf\n", minc);
		}
	}
	path1 = best;
	for(int i=1; i < 150000; i++) {
		next2[path1[i]] = path1[i-1];
		next1[path1[i-1]] = path1[i];
	}
	printf("%lf\n\n", find_cost(path1));
}

void anneal_path2(int max_iter=200) {
	float  temp = 2500000, alpha = 1;
	vector<int> best = path2;
	float  minc = find_cost(path2);
	printf("%lf\n", minc);
	for(int i=0; i < 10; i++) {
		vector<int> curr(best.begin(), best.end());
		for(long long j=0; j < max_iter; j++) {
			int x = rand ()  % 149999;
			int y = x+1;
			bool c1 =  (x > 0) ? ((next1[curr[y]] != curr[x-1]) && next2[curr[y]] != curr[x-1]) : 1;
			bool c4 =  (y < 149999) ? ((next1[curr[y+1]] != curr[x]) && next2[curr[y+1]] != curr[x]) : 1;
			if (c1 && c4) {
				float  subt = euc2(x, x-1) + euc2(y, y+1);
				float  add = euc2(y, x-1) + euc2(x, y+1);
				if (add <= subt) {
					swap(curr[x], curr[y]);
				}
				else if (j < 2000000) {
					float  r = (float ) rand ()  / RAND_MAX;
					if (exp((subt-add) / temp) > r) {
						swap(curr[x], curr[y]);
					}
				}
			}
		}
		optimize_path2(curr);
		if (find_cost(curr) < minc) {
			best = curr;
			minc = find_cost(curr);
			printf("%lf\n", minc);
		}
	}
	path2 = best;
	printf("%lf\n\n", find_cost(path2));
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
	anneal_path1();
	// Build second path
	build_path2();
	anneal_path2();

	// Print metric
	float  score1 = 0.0, score2 = 0.0;
	for(int i=1; i < 150000; i++) {
		score1 += euclid(cities[path1[i-1]], cities[path1[i]]);
		score2 += euclid(cities[path2[i-1]], cities[path2[i]]);
	}
	printf("%lf %lf\n", score1, score2);

	// Write output
	freopen("output-anneal.csv", "w", stdout);
	for(int i=0; i < 150000; i++) printf("%d,%d\n", path1[i], path2[i]);

	return 0;
}