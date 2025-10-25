#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <string>
// ������Windowsƽ̨������ͷ�ļ����������ÿ���̨����
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Complex {
private:
    double real;  // ʵ��
    double imag;  // �鲿

public:
    // ���캯��
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // ��ȡʵ��
    double getReal() const { return real; }
    // ��ȡ�鲿
    double getImag() const { return imag; }
    // ����ʵ��
    void setReal(double r) { real = r; }
    // �����鲿
    void setImag(double i) { imag = i; }

    // ���㸴����ģ
    double modulus() const { return sqrt(real * real + imag * imag); }

    // ��������������ʵ�����鲿����ͬ���ж���ȣ�
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // ���ز��������
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    // ���������������Զ��帴����ӡ��ʽ
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << fixed << setprecision(2);  // ����2λС��
        if (c.imag >= 0) {
            os << c.real << " + " << c.imag << "i";
        } else {
            os << c.real << " - " << -c.imag << "i";
        }
        return os;
    }
};

// ���ɵ������������ʵ�����鲿��[minVal, maxVal]��Χ�ڣ�
Complex generateRandomComplex(double minVal, double maxVal) {
    double real = minVal + (rand() % 1000) / 1000.0 * (maxVal - minVal);
    double imag = minVal + (rand() % 1000) / 1000.0 * (maxVal - minVal);
    return Complex(real, imag);
}

// ���������������
vector<Complex> generateRandomComplexVector(int size, double minVal, double maxVal) {
    vector<Complex> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(generateRandomComplex(minVal, maxVal));
    }
    return vec;
}

// �������ң�Fisher-Yates ϴ���㷨��
void shuffleVector(vector<Complex>& vec) {
    for (int i = vec.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);  // ����[0, i]���������
        swap(vec[i], vec[j]);      // ����Ԫ��
    }
}

// ����Ԫ�أ�����Ŀ��Ԫ�ص�������δ�ҵ�����-1��
int findElement(const vector<Complex>& vec, const Complex& target) {
    for (size_t i = 0; i < vec.size(); ++i) {  // size_tƥ��vector.size()���޷�������
        if (vec[i] == target) {
            return static_cast<int>(i);  // ��ʽת��Ϊint����
        }
    }
    return -1;
}

// ����Ԫ�أ���ָ��λ��pos����Ԫ��elem��
void insertElement(vector<Complex>& vec, size_t pos, const Complex& elem) {
    if (pos <= vec.size()) {  // �޷����������ж�>=0��pos���Ϊ�������ȣ����뵽ĩβ��
        vec.insert(vec.begin() + pos, elem);
    }
}

// ɾ��Ԫ�أ�ɾ��ָ��λ��pos��Ԫ�أ��ɹ�����true��ʧ�ܷ���false��
bool deleteElement(vector<Complex>& vec, size_t pos) {
    if (pos < vec.size()) {
        vec.erase(vec.begin() + pos);
        return true;
    }
    return false;
}

// ����Ψһ����ȥ���ظ�Ԫ�أ�
void uniqueVector(vector<Complex>& vec) {
    vector<Complex> temp;
    for (size_t i = 0; i < vec.size(); ++i) {
        const Complex& elem = vec[i];
        bool isDuplicate = false;
        // ��鵱ǰԪ���Ƿ�����temp��
        for (size_t j = 0; j < temp.size(); ++j) {
            if (elem == temp[j]) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) {
            temp.push_back(elem);
        }
    }
    vec = temp;  // ��ȥ�غ��temp�滻ԭ����
}

// �������򣨰�ģ����ģ��ͬ��ʵ������
void bubbleSort(vector<Complex>& vec) {
    size_t n = vec.size();
    for (size_t i = 0; i < n - 1; ++i) {
        // ÿ��ð�ݺ����Ԫ�س���ĩβ����һ��������
        for (size_t j = 0; j < n - i - 1; ++j) {
            double modJ = vec[j].modulus();
            double modJ1 = vec[j + 1].modulus();
            // �ȽϹ����ȱ�ģ��ģ��ͬ��ʵ��
            if (modJ > modJ1 || (modJ == modJ1 && vec[j].getReal() > vec[j + 1].getReal())) {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
}

// �鲢�������������ϲ������������������飩
void merge(vector<Complex>& vec, int left, int mid, int right) {
    int n1 = mid - left + 1;  // �������鳤��
    int n2 = right - mid;     // �������鳤��

    // ��ʱ�洢����������
    vector<Complex> leftVec(n1), rightVec(n2);
    for (int i = 0; i < n1; ++i) {
        leftVec[i] = vec[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        rightVec[j] = vec[mid + 1 + j];
    }

    // �ϲ���ʱ���鵽ԭ����
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        double modL = leftVec[i].modulus();
        double modR = rightVec[j].modulus();
        if (modL < modR || (modL == modR && leftVec[i].getReal() <= rightVec[j].getReal())) {
            vec[k] = leftVec[i];
            i++;
        } else {
            vec[k] = rightVec[j];
            j++;
        }
        k++;
    }

    // ����ʣ��Ԫ��
    while (i < n1) {
        vec[k] = leftVec[i];
        i++;
        k++;
    }
    while (j < n2) {
        vec[k] = rightVec[j];
        j++;
        k++;
    }
}

// �鲢���򣨵ݹ�ʵ�֣���ģ����ģ��ͬ��ʵ������
void mergeSort(vector<Complex>& vec, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;  // ������������(left+right)/2��
        mergeSort(vec, left, mid);            // ������������
        mergeSort(vec, mid + 1, right);       // ������������
        merge(vec, left, mid, right);         // �ϲ���������������
    }
}

// ������ң�����ģ����[m1, m2)��Ԫ�أ�������������
vector<Complex> rangeSearch(const vector<Complex>& vec, double m1, double m2) {
    vector<Complex> result;
    for (size_t i = 0; i < vec.size(); ++i) {
        const Complex& elem = vec[i];
        double mod = elem.modulus();
        if (mod >= m1 && mod < m2) {
            result.push_back(elem);
        }
    }
    return result;
}

// ��ӡ���������Զ�����ʾ��Ϣ��
void printVector(const vector<Complex>& vec, const string& msg = "") {
    if (!msg.empty()) {
        cout << msg << endl;
    }
    // ����������ӡÿ��Ԫ�ؼ�ģ
    for (size_t i = 0; i < vec.size(); ++i) {
        const Complex& elem = vec[i];
        cout << elem << " (ģ: " << fixed << setprecision(2) << elem.modulus() << ")\t";
        // ÿ5��Ԫ�ػ��У�����һ�й�����
        if ((i + 1) % 5 == 0) {
            cout << endl;
        }
    }
    cout << endl << endl;  // ĩβ�����У����ֲ�ͬ��������
}

int main() {
    // ����������Windows����̨����ΪGBK�������������
    #ifdef _WIN32
    SetConsoleOutputCP(936);
    #endif

    srand(time(0));  // ��ʼ����������ӣ�ȷ��ÿ��������������ͬ��

    // ---------------------- ����1�������������������ҡ����ҡ����롢ɾ����Ψһ���� ----------------------
    int basicSize = 10;
    // ���ɳ�ʼ�����������
    vector<Complex> complexVec = generateRandomComplexVector(basicSize, -10, 10);
    printVector(complexVec, "1. ��ʼ�������������10��Ԫ�أ�:");

    // ��������
    shuffleVector(complexVec);
    printVector(complexVec, "2. ���Һ������:");

    // �������Ԫ��
    if (!complexVec.empty()) {
        size_t randPos = rand() % complexVec.size();
        Complex target = complexVec[randPos];
        int findRes = findElement(complexVec, target);
        if (findRes != -1) {
            cout << "3. ���ҽ����Ԫ�� " << target << " ������λ��Ϊ " << findRes << endl << endl;
        } else {
            cout << "3. ���ҽ����δ�ҵ�Ԫ�� " << target << endl << endl;
        }
    }

    // ����Ԫ�أ�������2λ�ò���(100, 200)��
    Complex insertElem(100, 200);
    insertElement(complexVec, 2, insertElem);
    printVector(complexVec, "4. ������2����Ԫ�� " + to_string(insertElem.getReal()) + " + " + to_string(insertElem.getImag()) + "i �������:");

    // ɾ��Ԫ�أ�ɾ������2λ�õ�Ԫ�أ�
    if (deleteElement(complexVec, 2)) {
        printVector(complexVec, "5. ɾ������2��Ԫ�غ������:");
    } else {
        cout << "5. ɾ��ʧ�ܣ�����2����������Χ" << endl << endl;
    }

    // ����Ψһ��
    uniqueVector(complexVec);
    printVector(complexVec, "6. Ψһ�����������ȥ���ظ�Ԫ�أ�:");

    // ---------------------- ����2������Ч�ʶԱȣ��������� vs �鲢���� ----------------------
    int sortTestSize = 1000;  // ������Ե�������С���ɵ�����Խ�����Խ���ԣ�
    vector<Complex> randomVec = generateRandomComplexVector(sortTestSize, -100, 100);  // �������
    vector<Complex> sortedVec = randomVec;
    mergeSort(sortedVec, 0, static_cast<int>(sortedVec.size()) - 1);  // �����������ù鲢������ǰ�źã�
    vector<Complex> reversedVec = sortedVec;
    reverse(reversedVec.begin(), reversedVec.end());  // ��������

    clock_t start, end;
    double duration;
    cout << "---------------------- ����Ч�ʲ��ԣ�������С��" << sortTestSize << "�� ----------------------" << endl;

    // 1. ���˳���µ�����Ч��
    // �������������
    vector<Complex> bubbleRandom = randomVec;
    start = clock();
    bubbleSort(bubbleRandom);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "1. ���˳��" << endl;
    cout << "   ���������ʱ��" << fixed << setprecision(6) << duration << " ��" << endl;

    // �鲢���������
    vector<Complex> mergeRandom = randomVec;
    start = clock();
    mergeSort(mergeRandom, 0, static_cast<int>(mergeRandom.size()) - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "   �鲢�����ʱ��" << fixed << setprecision(6) << duration << " ��" << endl << endl;

    // 2. ����˳���µ�����Ч��
    // ������������
    vector<Complex> bubbleSorted = sortedVec;
    start = clock();
    bubbleSort(bubbleSorted);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "2. ����˳��" << endl;
    cout << "   ���������ʱ��" << fixed << setprecision(6) << duration << " ��" << endl;

    // �鲢��������
    vector<Complex> mergeSorted = sortedVec;
    start = clock();
    mergeSort(mergeSorted, 0, static_cast<int>(mergeSorted.size()) - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "   �鲢�����ʱ��" << fixed << setprecision(6) << duration << " ��" << endl << endl;

    // 3. ����˳���µ�����Ч��
    // ������������
    vector<Complex> bubbleReversed = reversedVec;
    start = clock();
    bubbleSort(bubbleReversed);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "3. ����˳��" << endl;
    cout << "   ���������ʱ��" << fixed << setprecision(6) << duration << " ��" << endl;

    // �鲢��������
    vector<Complex> mergeReversed = reversedVec;
    start = clock();
    mergeSort(mergeReversed, 0, static_cast<int>(mergeReversed.size()) - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "   �鲢�����ʱ��" << fixed << setprecision(6) << duration << " ��" << endl << endl;

    // ---------------------- ����3��������� ----------------------
    double m1 = 5.0, m2 = 10.0;  // ����ģ����[5.0, 10.0)��Ԫ��
    vector<Complex> rangeRes = rangeSearch(sortedVec, m1, m2);
    if (rangeRes.empty()) {
        cout << "---------------------- ������ҽ�� ----------------------" << endl;
        cout << "ģ����[" << m1 << ", " << m2 << ")��Ԫ�ز�����" << endl << endl;
    } else {
        printVector(rangeRes, "---------------------- ������ҽ�� ----------------------\nģ����[" + to_string(m1) + ", " + to_string(m2) + ")��Ԫ�أ���" + to_string(rangeRes.size()) + "����:");
    }

    // ��������ͣ���ڣ�����ִ����ֱ�ӹرգ����ֶ���������˳���
    system("pause");

    return 0;
}