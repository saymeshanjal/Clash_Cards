#include <bits/stdc++.h>
using namespace std;

bool is_valid_input(istream& in) {
    int t;
    if (!(in >> t)) {
        cout << "Error: Missing or invalid number of test cases (t).\n";
        return false;
    }
    if (t < 1 || t > 100) {
        cout << "Error: Test case count 't' must be in [1, 100]. Got: " << t << "\n";
        return false;
    }

    for (int i = 0; i < t; ++i) {
        int n, e, r;
        if (!(in >> n)) {
            cout << "Error: Missing or invalid value for n in test case " << i + 1 << ".\n";
            return false;
        }
        if (!(in >> e)) {
            cout << "Error: Missing or invalid value for e in test case " << i + 1 << ".\n";
            return false;
        }
        if (!(in >> r)) {
            cout << "Error: Missing or invalid value for r in test case " << i + 1 << ".\n";
            return false;
        }

        if (n < 2 || n > 16) {
            cout << "Error: 'n' in test case " << i + 1 << " must be even and in range [2, 16]. Got: " << n << "\n";
            return false;
        }
        if (n % 2 != 0) {
            cout << "Error: 'n' in test case " << i + 1 << " must be even. Got: " << n << "\n";
            return false;
        }
        if (e < 0 || e > 500) {
            cout << "Error: 'e' in test case " << i + 1 << " must be in range [0, 500]. Got: " << e << "\n";
            return false;
        }
        if (r < 1 || r > 5) {
            cout << "Error: 'r' in test case " << i + 1 << " must be in range [1, 5]. Got: " << r << "\n";
            return false;
        }
    }
    return true;
}

int main() {
    if (is_valid_input(cin)) {
        cout << "Input is valid.\n";
    } else {
        cout << "Invalid input detected. Please fix the input above.\n";
    }
    return 0;
}
