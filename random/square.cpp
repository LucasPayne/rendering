#include <iostream>
#include <vector>

// using namespace std;

inline std::vector<double> square(const std::vector<double> &vec)
{
    std::vector<double> squared(vec.size());
    for (int i = 0; i < vec.size(); i++) {
        squared[i] = vec[i] * vec[i];
    }
    return squared;
}

inline double square(double x)
{
    return x * x;
}

double get(const std::vector<double> &vec, int index)
{
    return vec[index];
}

int main()
{
    double x = square(9);
    std::cout << "The number is " << x << "\n";

    std::vector<double> nums(22);
    int c = 0;
    for (double &x : nums) {
        x = c;
        c++;
    }

    for (const double &x : nums) {
        std::cout << x << "\n";
    }

    for (int i = 0; i < 22; i++) {
        std::cout << ":: " << get(nums, i) << "\n";
    }
    std::vector<double> squared = square(nums);
    for (const double &x : squared) {
        std::cout << x << "\n";
    }
}
