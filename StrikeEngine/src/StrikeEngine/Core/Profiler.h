#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <iomanip>



#ifdef ENABLE_PROFILER

class Profiler {
private:
    std::unordered_map<std::string, double> profiles;

    static inline Profiler& instance() {
        static Profiler prof;
        return prof;
    }

public:
    class Timer {
    private:
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
        
    public:
        Timer(const std::string& n) : name(n) {
            start = std::chrono::high_resolution_clock::now();
        }
        
        ~Timer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            double timeMs = duration / 1000.0;
            
            // Just save it, don't print
            Profiler::instance().addEntry(name, timeMs);
        }
    };
    
    inline void addEntry(const std::string& name, double timeMs) {
        profiles[name] = timeMs;
    }
    
    static inline void printResults() {
        auto& prof = instance();
        std::cout << "\n=== Profiling Results ===\n";
        std::cout << std::left << std::setw(30) << "Function" 
                  << std::right << std::setw(15) << "Time (ms)" << "\n";
        std::cout << std::string(45, '-') << "\n";
        
        for (const auto& p : prof.profiles) {
            std::cout << std::left << std::setw(30) << p.first
                      << std::right << std::setw(15) << std::fixed 
                      << std::setprecision(3) << p.second << "\n";
        }
    }
    
    static inline void reset() {
        instance().profiles.clear();
    }
};


#define PROFILE_SCOPE(name) Profiler::Timer _profiler_timer_##__LINE__(name)
#define PROFILE_FUNCTION()  Profiler::Timer _profiler_timer_##__LINE__(__FUNCTION__)

#else

#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()

class Profiler {
public:
    static inline void printResults() {}
    static inline void reset() {}
};

#endif