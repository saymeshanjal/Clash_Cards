#include <bits/stdc++.h>
using namespace std;

int main() {
    srand(time(0));
    int t = 100; // Number of testcases
    cout << t << "\n";
    for (int i = 0; i < t; ++i) {
        int n = 2 * (rand() % 8 + 1);     // Even n in [2, 16]
        int E = rand() % 501;              // Energy in [0, 50]
        int R = rand() % 5 + 1;           // Rounds in [1, 5]
        cout << n << " " << E << " " << R << "\n";
    }
    return 0;
}
