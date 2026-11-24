#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Базовый уровень: шаблонная функция и анализ сложности.
namespace basic {
    template <typename T>
    T min(T a, T b) {
        return b < a ? b : a;
    }
}

void check(bool condition, const char* name) {
    std::cout << name << ": " << (condition ? "PASS" : "FAIL") << '\n';
    if (!condition) throw std::runtime_error(name);
}

template <typename F>
bool rejects(F action) {
    try { action(); }
    catch (const std::out_of_range&) { return true; }
    return false;
}

void basicDemo() {
    std::cout << "\n=== BASIC LEVEL: TEMPLATES AND BIG O ===\n";
    check(basic::min(7, -3) == -3, "min<int>");
    check(basic::min(2.5, 4.1) == 2.5, "min<double>");
    check(basic::min(std::string("Pear"), std::string("Apple")) == "Apple",
          "min<string>");
    check(basic::min(5, 5) == 5, "Equal values");
    std::cout << "min: O(1) comparisons\n"
              << "Examples from the manual:\n"
              << "A: O(n), one full pass\n"
              << "B: O(n^2), two nested loops\n"
              << "C: O(log n), doubling the index\n"
              << "D: O(n log n), linear and logarithmic loops\n"
              << "E: O(sqrt(n)), condition i*i < n\n";
}

// Средний уровень: три значения могут иметь разные типы.
template <typename T1, typename T2, typename T3>
class Triple {
    T1 first;
    T2 second;
    T3 third;
public:
    // Конструктор: O(1) при постоянной стоимости копирования полей.
    Triple(const T1& a, const T2& b, const T3& c)
        : first(a), second(b), third(c) {}
    // Геттеры возвращают ссылки за O(1).
    const T1& getFirst() const { return first; }
    const T2& getSecond() const { return second; }
    const T3& getThird() const { return third; }
    // Сеттеры: O(1) при постоянной стоимости присваивания.
    void setFirst(const T1& a) { first = a; }
    void setSecond(const T2& b) { second = b; }
    void setThird(const T3& c) { third = c; }
};

template <typename T>
std::ptrdiff_t linearSearch(const std::vector<T>& data, const T& key) {
    for (std::size_t i = 0; i < data.size(); ++i)
        if (data[i] == key) return static_cast<std::ptrdiff_t>(i);
    return -1;
}

template <typename T>
std::ptrdiff_t binarySearch(const std::vector<T>& data, const T& key) {
    std::size_t left = 0, right = data.size();
    while (left < right) {
        const auto mid = left + (right - left) / 2;
        if (data[mid] == key) return static_cast<std::ptrdiff_t>(mid);
        if (data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return -1;
}

using Clock = std::chrono::steady_clock;
using Microseconds = std::chrono::duration<double, std::micro>;

// Измеряем время без вывода в консоль внутри измеряемого участка.
template <typename F>
double measure(F action) {
    const auto start = Clock::now();
    action();
    return Microseconds(Clock::now() - start).count();
}

double median(std::array<double, 3> values) {
    std::sort(values.begin(), values.end());
    return values[1];
}

void mediumDemo() {
    std::cout << "\n=== MEDIUM LEVEL: TRIPLE ===\n";
    Triple<int, double, std::string> t(25, 3.14, "Hello");
    check(t.getFirst() == 25 && t.getSecond() == 3.14 &&
          t.getThird() == "Hello", "Constructor and getters");
    t.setFirst(42); t.setSecond(2.5); t.setThird("World");
    check(t.getFirst() == 42 && t.getSecond() == 2.5 &&
          t.getThird() == "World", "All setters");
    Triple<std::string, int, bool> other("C++", 17, true);
    check(other.getFirst() == "C++" && other.getSecond() == 17 &&
          other.getThird(), "Different template types");
    std::cout << "Triple: " << t.getFirst() << ", " << t.getSecond()
              << ", " << t.getThird() << '\n'
              << "Constructor/setters: O(1) for fixed-cost types\n"
              << "Getters: O(1)\n";
    check(linearSearch(std::vector<int>{}, 1) == -1 &&
          binarySearch(std::vector<int>{}, 1) == -1, "Empty search");
    std::cout << "\nSearch benchmark: 1000 identical queries, median of 3 (us)\n"
              << std::setw(10) << "N" << std::setw(16) << "Linear"
              << std::setw(16) << "Binary" << '\n';
    std::mt19937 rng(42);
    for (int n : {1000, 10000, 100000, 1000000}) {
        std::vector<int> data(n), keys(1000);
        for (int i = 0; i < n; ++i) data[i] = i;
        std::uniform_int_distribution<int> dist(0, n);
        for (int& key : keys) key = dist(rng);
        std::array<double, 3> a{}, b{};
        std::vector<std::ptrdiff_t> ra(keys.size()), rb(keys.size());
        for (int r = 0; r < 3; ++r) {
            auto linear = [&] { a[r] = measure([&] {
                for (std::size_t i = 0; i < keys.size(); ++i)
                    ra[i] = linearSearch(data, keys[i]);
            }); };
            auto binary = [&] { b[r] = measure([&] {
                for (std::size_t i = 0; i < keys.size(); ++i)
                    rb[i] = binarySearch(data, keys[i]);
            }); };
            if (r % 2 == 0) { linear(); binary(); }
            else { binary(); linear(); }
            if (ra != rb) throw std::runtime_error("Search results differ");
        }
        std::cout << std::setw(10) << n << std::setw(16) << median(a)
                  << std::setw(16) << median(b) << '\n';
    }
    check(true, "Search results match");
}

// Продвинутый уровень: собственный двусвязный список.
template <typename T>
class List {
    struct Node {
        T value;
        Node* prev = nullptr;
        Node* next = nullptr;
        explicit Node(const T& v) : value(v) {}
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    std::size_t count = 0;

    // Поиск узла с ближайшего конца: O(n) в худшем случае.
    Node* nodeAt(std::size_t index) const {
        if (index >= count) throw std::out_of_range("Invalid index");
        if (index < count / 2) {
            Node* p = head;
            for (std::size_t i = 0; i < index; ++i) p = p->next;
            return p;
        }
        Node* p = tail;
        for (std::size_t i = count - 1; i > index; --i) p = p->prev;
        return p;
    }

    // Удаление известного узла: O(1).
    void unlink(Node* p) {
        if (p->prev) p->prev->next = p->next;
        else head = p->next;
        if (p->next) p->next->prev = p->prev;
        else tail = p->prev;
        delete p;
        --count;
    }

    void swap(List& other) noexcept {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(count, other.count);
    }
public:
    List() = default;
    ~List() { clear(); }

    // Глубокое копирование: O(n). При исключении освобождаем узлы.
    List(const List& other) {
        try {
            for (Node* p = other.head; p; p = p->next) push_back(p->value);
        } catch (...) { clear(); throw; }
    }
    List& operator=(const List& other) {
        if (this != &other) { List copy(other); swap(copy); }
        return *this;
    }
    // Перемещение конструктора: O(1).
    List(List&& other) noexcept { swap(other); }
    List& operator=(List&& other) noexcept {
        if (this != &other) { clear(); swap(other); }
        return *this;
    }

    // Добавление в начало и конец: O(1).
    void push_front(const T& value) {
        Node* p = new Node(value);
        p->next = head;
        if (head) head->prev = p;
        else tail = p;
        head = p;
        ++count;
    }
    void push_back(const T& value) {
        Node* p = new Node(value);
        p->prev = tail;
        if (tail) tail->next = p;
        else head = p;
        tail = p;
        ++count;
    }
    // Удаление с концов: O(1).
    void pop_front() {
        if (empty()) throw std::out_of_range("List is empty");
        unlink(head);
    }
    void pop_back() {
        if (empty()) throw std::out_of_range("List is empty");
        unlink(tail);
    }
    std::size_t size() const noexcept { return count; }
    bool empty() const noexcept { return count == 0; }
    const T& front() const { return nodeAt(0)->value; }
    const T& back() const {
        if (empty()) throw std::out_of_range("List is empty");
        return tail->value;
    }
    // Вставка по индексу: O(n) в худшем случае, O(1) на концах.
    void insert(std::size_t index, const T& value) {
        if (index > count) throw std::out_of_range("Invalid insert index");
        if (index == 0) { push_front(value); return; }
        if (index == count) { push_back(value); return; }
        Node* next = nodeAt(index);
        Node* p = new Node(value);
        p->prev = next->prev;
        p->next = next;
        next->prev->next = p;
        next->prev = p;
        ++count;
    }
    // Удаление по индексу: O(n) в худшем случае, O(1) на концах.
    void erase(std::size_t index) { unlink(nodeAt(index)); }
    // Очистка и преобразование для проверки: O(n).
    void clear() noexcept { while (head) unlink(head); }
    std::vector<T> values() const {
        std::vector<T> result;
        result.reserve(count);
        for (Node* p = head; p; p = p->next) result.push_back(p->value);
        return result;
    }
};

void listDemo() {
    std::cout << "\n=== ADVANCED LEVEL: LIST<T> ===\n";
    List<int> l;
    check(l.empty() && l.size() == 0, "Initially empty");
    l.push_back(20); l.push_front(10); l.push_back(40);
    l.insert(2, 30);
    check(l.values() == std::vector<int>{10, 20, 30, 40}, "Push and insert");
    l.erase(1); l.pop_front(); l.pop_back();
    check(l.values() == std::vector<int>{30} && l.front() == l.back(),
          "Erase and pop");
    l.insert(0, 5); l.insert(l.size(), 60);
    check(l.values() == std::vector<int>{5, 30, 60} && l.size() == 3,
          "Boundary insert and size");
    List<int> copy(l), assigned;
    assigned = l;
    copy.erase(0);
    check(l.size() == 3 && copy.size() == 2 && assigned.values() == l.values(),
          "Deep copy and assignment");
    List<int> moved(std::move(copy));
    assigned = std::move(moved);
    check(copy.empty() && moved.empty() && assigned.size() == 2,
          "Move constructor and assignment");
    check(rejects([&] { l.insert(4, 0); }) && rejects([&] { l.erase(3); }),
          "Invalid indices");
    l.clear(); l.push_back(1); l.pop_back();
    check(l.empty() && rejects([&] { l.pop_back(); }) &&
          rejects([&] { l.pop_front(); }) && rejects([&] { l.erase(0); }) &&
          rejects([&] { l.front(); }) && rejects([&] { l.back(); }), "Empty list errors");
    List<std::string> words;
    words.push_back("world"); words.push_front("hello");
    check(words.values() == std::vector<std::string>{"hello", "world"},
          "List<string>");
    List<double> decimals;
    decimals.push_back(1.5); decimals.insert(0, 2.5);
    check(decimals.values() == std::vector<double>{2.5, 1.5}, "List<double>");
    std::cout << "push_front/push_back/pop_front/pop_back/size: O(1)\n"
              << "insert/erase by index: O(n) worst case, O(1) at ends\n"
              << "Memory: O(n)\n";
}

// Адаптер STL использует такой же поиск позиции, как собственный список.
class StdList {
    std::list<int> data;
    auto at(std::size_t index) {
        if (index < data.size() / 2)
            return std::next(data.begin(), static_cast<std::ptrdiff_t>(index));
        return std::prev(data.end(), static_cast<std::ptrdiff_t>(data.size() - index));
    }
public:
    void push_back(int x) { data.push_back(x); }
    void push_front(int x) { data.push_front(x); }
    void pop_back() { data.pop_back(); }
    void pop_front() { data.pop_front(); }
    void insert(std::size_t i, int x) { data.insert(at(i), x); }
    void erase(std::size_t i) { data.erase(at(i)); }
    std::size_t size() const { return data.size(); }
    int front() const { return data.front(); }
    int back() const { return data.back(); }
};

struct Times { double ends; double middle; };

template <typename L>
Times listTrial(int n) {
    L l;
    // N добавлений с каждого конца и N удалений с каждого конца.
    double ends = measure([&] {
        for (int i = 0; i < n; ++i) l.push_back(i);
        for (int i = 0; i < n; ++i) l.push_front(-i - 1);
    });
    if (l.size() != static_cast<std::size_t>(2 * n) ||
        l.front() != -n || l.back() != n - 1)
        throw std::runtime_error("Wrong benchmark list state");
    // Десять пар вставка/удаление в середине, включая поиск позиции.
    double middle = measure([&] {
        for (int i = 0; i < 10; ++i) {
            l.insert(n, i);
            l.erase(n);
        }
    });
    ends += measure([&] {
        for (int i = 0; i < n; ++i) l.pop_front();
        for (int i = 0; i < n; ++i) l.pop_back();
    });
    if (l.size() != 0) throw std::runtime_error("Benchmark list not empty");
    return {ends, middle};
}

void listBenchmark() {
    std::cout << "\n=== LIST<T> VS STD::LIST<int> ===\n"
              << "Median of 3 trials, time in microseconds (us)\n"
              << "Ends: N pushes and pops at each end (4*N operations)\n"
              << "Middle: 10 insert/erase pairs in a list of 2*N elements\n"
              << std::setw(10) << "N" << std::setw(15) << "List ends"
              << std::setw(15) << "STL ends" << std::setw(15) << "List middle"
              << std::setw(15) << "STL middle" << '\n';
    for (int n : {1000, 10000, 100000, 1000000}) {
        std::array<double, 3> ae{}, be{}, am{}, bm{};
        for (int r = 0; r < 3; ++r) {
            Times a{}, b{};
            if (r % 2 == 0) { a = listTrial<List<int>>(n); b = listTrial<StdList>(n); }
            else { b = listTrial<StdList>(n); a = listTrial<List<int>>(n); }
            ae[r] = a.ends; am[r] = a.middle; be[r] = b.ends; bm[r] = b.middle;
        }
        std::cout << std::setw(10) << n << std::setw(15) << median(ae)
                  << std::setw(15) << median(be) << std::setw(15) << median(am)
                  << std::setw(15) << median(bm) << '\n';
    }
}

int main() {
    try {
        std::cout << std::fixed << std::setprecision(2)
                  << "LAB 2 - YEAR 2 - VARIANT 2\n";
        basicDemo();
        mediumDemo();
        listDemo();
        listBenchmark();
        std::cout << "\nProgram completed successfully.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
