#include <cassert>
#include <iostream>

// Заготовка задачи
int sum(const int* data, int n)
{
    int s = 0;
    for (int i = 0; i < n; ++i) {
        s += data[i];
    }
    return s;
}

int main()
{
    int a[] = {1, 2, 3, 4};
    assert(sum(a, 4) == 10);
    assert(sum(a, 0) == 0);
    std::cout << "OK\n";
}
