#include "numbers.dat"
#include <cstdio>
#include <cstdlib>

const int N = 100000+1;//max integer from range

using namespace std;

//type = -1 for the first of the same numbers in array. 1 - last
int binary_search (const int* s, int n, int elem, int type) {	
	int l = 0; int r = n-1; int m;
	
	while (l <= r) {
		m = (l+r)>>1;
		if (s[m] == elem) {
			while (s[m] == elem && m < n && m >= 0)
				m+=type;
			return m-type;
		}
		else if (s[m] > elem) {
			r = m-1;
		}
		else
			l = m+1;
	}
	return -1;
}

void find_prime (bool* S, int n) {
	S[1] = 0; S[2] = 1;
	for (int k = 3; k < N; ++k)
		S[k] = k&1;
	
	for (int k = 3; k*k <= N; k+=2) {
		if (S[k])
			for (int l = k*k; l <= N; l+=k)
				S[l] = 0;
	}
}

int main (int argc, char* argv[]) {
	if (!(argc&1) || argc == 1)
		return -1;
	
	bool *S = new bool [N];
	find_prime(S, N);
	
	for (int i = 1; i < argc; i += 2) {
		int a1 = binary_search(Data, sizeof(Data)/sizeof(int), atoi(argv[i]), -1);
		int b1 = binary_search(Data, sizeof(Data)/sizeof(int), atoi(argv[i+1]), 1);
		//~ printf("(%d, %d)\n", a1, b1);
		if (a1 == -1 || b1 == -1 || a1 > b1)
			printf ("0\n");
		else {
			int count = 0;
			for (int j = a1; j <= b1; ++j)
				count+=S[Data[j]];
			printf("%d\n", count);
		}
   }
   delete[]S;
   return 0;
}
