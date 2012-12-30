#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

int main() {
	printf("%d\n", RAND_MAX);
	vector<int> A(150000);
	for(int i=0; i < 150000; i++) A[i] = i;
	mt19937 gen(time(NULL));
	for(long long i=0; i < 3000000000LL; i++) {
		int x = gen() % 149999;
		swap(A[x], A[x+1]);
	}
}
