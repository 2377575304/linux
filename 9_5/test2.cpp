#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>
#include <type_traits>
#include <optional>
#include <variant>

// 1. 模板与类型萃取
template <typename T>
constexpr bool is_numeric = std::is_arithmetic_v<T>;

template <typename T>
std::enable_if_t<is_numeric<T>, void> print_sum(const std::vector<T>& vec) {
    T sum = std::accumulate(vec.begin(), vec.end(), T{});
    std::cout << "Sum: " << sum << "\n";
}

// 2. 智能指针与资源管理
class Resource {
private:
    int id;
    static std::mutex mtx;
public:
    Resource(int i) : id(i) { 
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Resource " << id << " created\n";
    }
    ~Resource() { 
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Resource " << id << " destroyed\n";
    }
    void use() const { std::cout << "Using resource " << id << "\n"; }
};
std::mutex Resource::mtx;

// 3. 多态与抽象类
class DataProcessor {
public:
    virtual ~DataProcessor() = default;
    virtual std::optional<double> process(const std::vector<double>& data) = 0;
};

class AverageProcessor : public DataProcessor {
public:
    std::optional<double> process(const std::vector<double>& data) override {
        if (data.empty()) return std::nullopt;
        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        return avg;
    }
};

// 4. 并发编程
void parallel_task(int id, std::shared_ptr<Resource> res) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    res->use();
}

// 5. Lambda与函数式编程
auto create_filter(double threshold) {
    return [threshold](double val) { return val > threshold; };
}

// 6. 编译期计算
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

int main() {
    // 编译期计算演示
    constexpr int fact5 = factorial(5);
    std::cout << "5! = " << fact5 << " (computed at compile time)\n";

    // 智能指针演示
    auto res = std::make_shared<Resource>(1);
    {
        std::unique_ptr<Resource> unique_res = std::make_unique<Resource>(2);
    } // unique_res自动释放

    // 多态演示
    std::unique_ptr<DataProcessor> processor = std::make_unique<AverageProcessor>();
    std::vector<double> data = {1.2, 3.4, 5.6, 7.8};
    if (auto result = processor->process(data)) {
        std::cout << "Average: " << *result << "\n";
    }

    // Lambda与STL算法
    auto filter = create_filter(4.0);
    std::vector<double> filtered;
    std::copy_if(data.begin(), data.end(), std::back_inserter(filtered), filter);
    std::cout << "Filtered values: ";
    for (auto v : filtered) std::cout << v << " ";
    std::cout << "\n";

    // 并发编程
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 3; ++i) {
        futures.emplace_back(std::async(std::launch::async, 
            parallel_task, i, res));
    }
    // 等待所有任务完成
    for (auto& f : futures) f.get();

    // 类型萃取演示
    print_sum(std::vector<int>{1, 2, 3, 4});
    // print_sum(std::vector<std::string>{"a", "b"}) 编译错误，符合预期

    return 0;
}

