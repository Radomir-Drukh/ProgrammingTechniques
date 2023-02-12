#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

string inputFile = "data.txt";
string outputFile = "output.txt";
string timeStampsFile = "timestamps.txt";

int batchNum = 7;

int dims[7] = { 100, 500, 1000, 2000, 5000, 10000, 100000 };

//average -> name -> facullty

struct Student {
    string name;
    string faculty;
    int group;
    double average;

    Student(string name, string faculty, int group, double average)
    {
        this->name = name;
        this->faculty = faculty;
        this->group = group;
        this->average = average;
    }

    friend bool operator== (const Student& a, const Student& b) {
        if (a.average == b.average && a.name == b.name && a.faculty == b.faculty)
            return true;
        return false;
    }

    friend bool operator< (const Student& a, const Student& b) {
        if (a.average < b.average || ((a.average == b.average) && (a.name < b.name)) || 
            ((a.average == b.average) && (a.name == b.name) && (a.faculty < b.faculty)))
            return true;
        return false;
    }

    friend bool operator<= (const Student& a, const Student& b) {
        if (a < b || a == b)
            return true;
        return false;
    }

    friend bool operator> (const Student& a, const Student& b) {
        if (!(a < b) && !(a == b))
            return true;
        return false;
    }

    friend bool operator>= (const Student& a, const Student& b) {
        if (!(a < b))
            return true;
        return false;
    }


    friend ostream& operator<<(ostream& os, const Student& a) {
        os << a.average << ' ' << a.name << ' ' << a.faculty << '\n';
        return os;
    }
};

string randomString(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

double randomDouble(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void makeTxtFile() {
    srand((unsigned)time(NULL));
    ofstream fout(inputFile);

    for (int i = 0; i < batchNum; ++i) {
        //Вывод числа записей
        fout << dims[i] << "\n";
        for (int j = 0; j < dims[i]; ++j) {
            //Вывод полей по порядку
            fout << randomString(20) << "\n";   //Имя
            fout << randomString(10) << "\n";   //Факультет
            fout << rand() % 10 + 1  << "\n";   //Номер группы
            fout << randomDouble(1, 5) << "\n"; //Средний балл
        }
    }
}

vector<vector<Student>> readTxtFile() {
    ifstream fin(inputFile);

    vector<vector<Student>> result;

    int dim;
    string name, faculty;
    int group;
    double average;

    for (int i = 0; i < batchNum; ++i) {
        //Ввод числа записей
        fin >> dim;
        vector<Student> v;
        for (int j = 0; j < dim; ++j) {
            //Ввод полей по порядку
            fin >> name >> faculty >> group >> average;
            Student temp(name, faculty, group, average);
            v.push_back(temp);
        }
        result.push_back(v);
    }

    return result;
}

vector<Student> selectSort(vector<Student> vec) {
    vector<Student> a = vec;

    int min = 0; // индекс минимального значения

    for (int i = 0; i < a.size(); i++)
    {
        min = i; // изначально min - номер текущей ячейки

        for (int j = i + 1; j < a.size(); j++)
            min = (a[j] < a[min]) ? j : min;

        // после нахождения минимального элемента, поменяем его местами с текущим
        
        std::swap(a[i], a[min]);
    }

    return a;
}

vector<Student> bubbleSort(vector<Student> vec) {
    vector<Student> a = vec;

    for (int i = 0; i < a.size(); i++)
    {
        for (int j = 0; j < a.size() - i; j++) {
            if (j + 1 < a.size() && a[j] > a[j + 1])
            std::swap(a[j], a[j + 1]);
        }
    }

    return a;
}

vector<Student> shakerSort(vector<Student> vec) {
    vector<Student> arr = vec;

    int control = arr.size() - 1;
    int left = 0;
    int right = arr.size() - 1;
    do {
        for (int i = left; i < right; i++) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                control = i;
            }
        }
        right = control;
        for (int i = right; i > left; i--) {
            if (arr[i] < arr[i - 1]) {
                swap(arr[i], arr[i - 1]);
                control = i;
            }
        }
        left = control;
    } while (left < right);

    return arr;
}

void writeOutput(string title, vector<vector<Student>>& res) {
    ofstream fout(outputFile, ios::app);

    fout << title << "\n";

    for (int i = 0; i < batchNum; ++i) {
        //Вывод числа записей
        fout << "Размер: " << res[i].size() << "\n";
        for (int j = 0; j < res[i].size(); ++j) {
            //Вывод объекта
            fout << res[i][j] << "\n";
        }
    }
}

void writeTime(string title, vector<std::chrono::steady_clock::time_point> time) {
    ofstream fout(timeStampsFile, ios::app);

    fout << title << "\n";

    for (int i = 0; i < time.size() - 1; ++i)
        fout << "Время для сортировки " << dims[i] << " значений: " << chrono::duration_cast<chrono::milliseconds>(time[i+1] - time[i]).count() << " [ms]\n";

    fout << "\n";
}

int main()
{
    setlocale(LC_ALL, "Russian");
    makeTxtFile();

    vector<vector<Student>> initialArray = readTxtFile();

    vector<vector<Student>> selectSortRes, bubbleSortRes, shakerSortRes;

    vector<std::chrono::steady_clock::time_point> time;

    time.push_back(std::chrono::steady_clock::now());

    for (int i = 0; i < batchNum; ++i) {
        selectSortRes.push_back(selectSort(initialArray[i]));
        time.push_back(std::chrono::steady_clock::now());
    }

    writeOutput("Сортировка выбором:\n", selectSortRes);
    writeTime("Сортировка выбором:\n", time);
    time.clear();

    time.push_back(std::chrono::steady_clock::now());

    for (int i = 0; i < batchNum; ++i) {
        bubbleSortRes.push_back(bubbleSort(initialArray[i]));
        time.push_back(std::chrono::steady_clock::now());
    }

    writeOutput("Сортировка пузырьком:\n", bubbleSortRes);
    writeTime("Сортировка пузырьком:\n", time);
    time.clear();

    time.push_back(std::chrono::steady_clock::now());

    for (int i = 0; i < batchNum; ++i) {
        shakerSortRes.push_back(shakerSort(initialArray[i]));
        time.push_back(std::chrono::steady_clock::now());
    }

    writeOutput("Сортировка шейкером:\n", shakerSortRes);
    writeTime("Сортировка шейкером:\n", time);
    time.clear();


    return 0;
}
