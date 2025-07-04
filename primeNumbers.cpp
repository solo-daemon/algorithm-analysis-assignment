#include <iostream>
#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>
#include <sys/resource.h>  // for getrusage on Unix (Linux/macOS)
#include <mutex>
#include <thread>
#include <future>

size_t getMemoryUsageKB() {
#if defined(__unix__) || defined(__APPLE__)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    // On macOS, ru_maxrss is in bytes; on Linux it's in kilobytes
    #if defined(__APPLE__)
        return usage.ru_maxrss / 1024; // Convert bytes to KB
    #else
        return usage.ru_maxrss;        // Already in KB
    #endif
#else
    // Unsupported OS fallback
    return 0;
#endif
}


constexpr long double EPS = 1e-15;

// Floating-point comparator for map
struct FloatCompare {
    bool operator()(long double a, long double b) const {
        return a < b - EPS;
    }
};

// Our number representation
struct Number {
    long long int two_e; // exponent of 2
    long long int three_e; // exponent of 3
    long long int five_e; // exponent of 5

    Number(long long int a = 0, long long int b = 0, long long int c = 0) : two_e(a), three_e(b), five_e(c) {}

    long double logValue() const {
        static const long double LOG2 = std::logl(2.0L);
        static const long double LOG3 = std::logl(3.0L);
        static const long double LOG5 = std::logl(5.0L);
        return two_e * LOG2 + three_e * LOG3 + five_e * LOG5;
    }

    long long actualValue() const {
        long long result = 1;
        for (int i = 0; i < two_e; ++i) result *= 2;
        for (int i = 0; i < three_e; ++i) result *= 3;
        for (int i = 0; i < five_e; ++i) result *= 5;
        return result;
    }
    
    bool operator<(const Number& other) const {
        return logValue() + EPS < other.logValue();
    }
};

struct Compare {
    bool operator()(const Number& a, const Number& b) const {
        return a.logValue() > b.logValue();  // min-heap
        }
    };

void printPriorityQueue(std::priority_queue<Number, std::vector<Number>, Compare> pq) {
    std::cout << "Peeking into the priority queue:\n";
    while (!pq.empty()) {
        std::cout << pq.top().actualValue() << " ";
        pq.pop();
    }
    std::cout << "\n";
}


void printMap(std::map<long double, Number, FloatCompare> &result){
    std::cout << "Peeking into the map:\n";
    for (const auto& [logval, num] : result) {
        std::cout << num.actualValue() << " ";
    }
    std::cout << "\n";
}



void solution(int n, bool verbose = false){

   // Initialization
   std::map<long double, Number, FloatCompare> result;
   std::priority_queue<Number, std::vector<Number>, Compare> q;

    Number start(0, 0, 0);  // Represents the number 1
    q.push(start);
    result.insert({start.logValue(), start});

    int count = 0;
    Number current;

    while (count < n) {
        current = q.top();
        q.pop();
        
        count++;

        Number next1 = Number(current.two_e + 1, current.three_e, current.five_e);
        Number next2 = Number(current.two_e, current.three_e + 1, current.five_e);
        Number next3 = Number(current.two_e, current.three_e, current.five_e + 1);

        for (auto& next : {next1, next2, next3}) {
            if (result.find(next.logValue()) == result.end()) {
                q.push(next);
                result.insert({next.logValue(), next});
            }
        }
    }
    if(verbose){
        printMap(result);
        // printPriorityQueue(q);
    }

}

void test() {
    std::ofstream csv("performance_data.csv");
    csv << "N,TimeConsumed(ms),MemoryUsed(KB)\n";

    for (int n = 1; n <= 10000000; ++n) {
        auto start = std::chrono::high_resolution_clock::now();

        solution(n);  // Call the algorithm

        auto end = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        size_t memory_kb = getMemoryUsageKB();

        csv << n << "," << duration_ms << "," << memory_kb << "\n";

        if (n % 1000 == 0) {
            std::cout << "Completed N = " << n << "\n";
        }
    }

    csv.close();
}

int main() {
    int n;
    // std::cout << "Enter n: ";
    // std::cin >> n;
    // for 1500
    // solution(2, true);
    // for 100000
    // solution(100000, true);
    test();
    return 0;
}


