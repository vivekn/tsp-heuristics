#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <cstring>
#include <cassert>
#include <queue>
#include <set>
#include <numeric>
#include <stack>
using namespace std;
#define mp make_pair
#define INF (int)1e9
#define X first
#define Y second
#define REP(i, n) for(int i=0; i < n; i++)
#define FOR(i, a, b) for(int i=a; i < b; i++)
#define fill(a, x) memset(a, x, sizeof(a))
#define all(c) c.begin(), c.end()
#define sz(x)    ((int) x.size())
typedef vector<int> vi;
typedef pair<int, int> ii;
typedef long long ll;

struct node {
	int id, x, y;
	node(int a=0, int b=0, int c=0) : id(a), x(b), y(c) {}
};

bool vcmp(node a, node b) {
	if (a.x != b.x) return a.x < b.x;
	return a.y < b.y;
}

bool hcmp(node a, node b) {
	if (a.y != b.y) return a.y < b.y;
	return a.x < b.x;
}

node hcities[150000], vcities[150000], cities[150000];
set<ii> seen;
vi closest[150000];

double euclid(node a, node b) {
	return sqrt((a.x-b.x) * (a.x-b.x) + (a.y - b.y) * (a.y - b.y));
}
node point;

bool dist_from(int a, int b) {
	return euclid(cities[a], point) < euclid(cities[b], point);
}

void build_closest() {
	for(int i=0; i < 150000; i++) closest[i].clear();
	for(int i=0; i < 150000; i++) {
		// closest on x coord
		for(int j = i+1; j < i+75 && j < 150000; j++)
			if (j < 150000 && j >= 0)
			closest[hcities[i].id].push_back(hcities[j].id);
		for(int j = i-1; j >= i-35 && j >= 0; j--)
			if (j < 150000 && j >= 0)
			closest[hcities[i].id].push_back(hcities[j].id);
	}
	for(int i=0; i < 150000; i++) {
		// closest on y coord
		for(int j = i+1; j <= i+75 && j < 150000; j++)
			if (j < 150000 && j >= 0)
			closest[vcities[i].id].push_back(vcities[j].id);
		for(int j = i-1; j >= i-35 && j >= 0; j--)
			if (j < 150000 && j >= 0)
			closest[vcities[i].id].push_back(vcities[j].id);
	}
	for(int i=0; i < 150000; i++) {
		point = cities[i];
		sort(all(closest[i]), dist_from);
	}

}

int main() {
	freopen("santa2.csv", "r", stdin);
	for(int i=0; i < 150000; i++) {
		int z, x, y;
		scanf("%d", &z);
		scanf("%d", &x);
		scanf("%d", &y);
		// if (z > 150000) assert(0);
		cities[i] = vcities[i] = hcities[i] = node(i, x, y);
	}
	sort(hcities, hcities+150000, hcmp);
	vi path1, path2;

	
	sort(vcities, vcities+150000, vcmp);
	
	build_closest();

	bool selected[150000] = {false};

	// Build path 1
	int hid = hcities[0].id;
	for(int i=0; i < 150000; i++) {
		selected[hid] = 1;
		for(int j=0; j < sz(closest[hid]); j++) {
			if (!selected[closest[hid][j]]) {
				if (seen.find(ii(hid, closest[hid][j])) == seen.end()) {
					seen.insert(ii(hid, closest[hid][j]));
					seen.insert(ii(closest[hid][j], hid));
					path1.push_back(hid);
					hid = closest[hid][j];
					break;
				}
			}
		}
	}

		selected[hid] = 1;
		path1.push_back(hid);
		seen.insert(ii(path1[sz(path1)-2], path1[sz(path1) - 1]));
		seen.insert(ii(path1[sz(path1)-1], path1[sz(path1) - 2]));

	vi to_add;
	for(int i=0; i < 150000; i++)
		if (!selected[i])
			to_add.push_back(i);
	sort(all(to_add), hcmp);
	for(int i=0; i < sz(to_add); i++) {
		path1.push_back(to_add[i]);
		seen.insert(ii(path1[sz(path1)-2], path1[sz(path1) - 1]));
		seen.insert(ii(path1[sz(path1)-1], path1[sz(path1) - 2]));
	}
	fill(selected, false);

	// Build Path 2
	hid = hcities[0].id;
	for(int i=0; i < 150000; i++) {
		selected[hid] = 1;
		for(int j=0; j < sz(closest[hid]); j++) {
			if (!selected[closest[hid][j]]) {
				if (seen.find(ii(hid, closest[hid][j])) == seen.end()) {
					seen.insert(ii(hid, closest[hid][j]));
					seen.insert(ii(closest[hid][j], hid));
					path2.push_back(hid);
					hid = closest[hid][j];
					break;
				}
			}
		}
	}
		selected[hid] = 1;
		path2.push_back(hid);
		seen.insert(ii(path1[sz(path1)-2], path1[sz(path1) - 1]));
		seen.insert(ii(path1[sz(path1)-1], path1[sz(path1) - 2]));
	
	to_add.clear();
	for(int i=0; i < 150000; i++)
		if (!selected[i])
			to_add.push_back(i);
	sort(all(to_add), vcmp);
	for(int i=0; i < sz(to_add); i++) {
		while(seen.find(ii(path2[sz(path2)-1], to_add[i])) != seen.end()) {
			int x = rand() % (sz(to_add) - i) + i;
			swap(to_add[x], to_add[i]);
		}	
		path2.push_back(to_add[i]);
	}
	
	printf("%d %d\n", sz(path1), sz(path2));

	double d1 = 0.0, d2 = 0.0;

	for(int i=0; i < 149999; i++) {
		d1 += euclid(cities[path1[i]], cities[path1[i+1]]);
		d2 += euclid(cities[path2[i]], cities[path2[i+1]]);
	}
	printf("%lf %lf\n", d1, d2);



	freopen("output.csv", "w", stdout);
	printf("path1,path2\n");
	for(int i=0; i < 150000; i++)
		printf("%d,%d\n", path1[i], path2[i]);
	return 0;
}