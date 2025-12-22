#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

int* createArray(int n) {
    int* arr = new int[n];
    return arr;
}

int main() {
    int n;
    cout << "Enter size: ";
    cin >> n;

    int* data = createArray(n);

    for (int i = 0; i <= n; i++) { // ошибка: <= вместо <
        data[i] = i * 2;
    }

    cout << "Last element: " << data[n] << endl; // выход за границы

    if (data != nullptr) {
        delete data; // ошибка: delete вместо delete[]
    }

    return 0;
}
