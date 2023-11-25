// Copyright @2022 Alan Li"
// Alan Li tianyl28@uw.edu

#include <iostream>

int main() {
    int n;
    std::cout << "Which positive integer would you like me to factorize? ";
    std::cin >> n;

    if (n <= 0) {
        std::cerr << "Error: input is not a positive integer\n";
        return 1;
    }

    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            std::cout << i;
            if (i != n) {
                std::cout << " ";
            }
        }
    }
    std::cout << "\n";

    return 0;
}
