#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include <string>

namespace asciinema {

class FPSCounter {
public:
    void tick() {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(mutex_);
        timestamps_.push_back(now);
        
        auto cutoff = now - std::chrono::seconds(1);
        while (!timestamps_.empty() && timestamps_.front() < cutoff)
            timestamps_.pop_front();
    }

    double fps() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<double>(timestamps_.size());
    }

private:
    mutable std::mutex mutex_;
    std::deque<std::chrono::steady_clock::time_point> timestamps_;
};

class LatencyTracker {
public:
    explicit LatencyTracker(size_t window = 100) : window_(window) {}

    void record(double ms) {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back(ms);
        if (samples_.size() > window_)
            samples_.pop_front();
    }

    double p50() const { return percentile(50); }
    double p95() const { return percentile(95); }
    double avg() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (samples_.empty()) return 0;
        double sum = 0;
        for (double s : samples_) sum += s;
        return sum / samples_.size();
    }

private:
    double percentile(int p) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (samples_.empty()) return 0;
        
        std::vector<double> sorted(samples_.begin(), samples_.end());
        std::sort(sorted.begin(), sorted.end());
        
        size_t idx = (p * sorted.size()) / 100;
        if (idx >= sorted.size()) idx = sorted.size() - 1;
        return sorted[idx];
    }

    mutable std::mutex mutex_;
    std::deque<double> samples_;
    size_t window_;
};

struct Metrics {
    FPSCounter decode_fps;
    FPSCounter process_fps;
    FPSCounter render_fps;
    
    LatencyTracker latency{100};
    
    std::atomic<uint64_t> frames_decoded{0};
    std::atomic<uint64_t> frames_processed{0};
    std::atomic<uint64_t> frames_rendered{0};
    std::atomic<uint64_t> frames_dropped{0};

    std::string format() const {
        char buf[256];
        snprintf(buf, sizeof(buf),
            "FPS D:%.0f P:%.0f R:%.0f | Lat %.1f/%.1fms | Drop %llu | Frames %llu",
            decode_fps.fps(),
            process_fps.fps(),
            render_fps.fps(),
            latency.p50(),
            latency.p95(),
            frames_dropped.load(),
            frames_rendered.load()
        );
        return buf;
    }
};

}
