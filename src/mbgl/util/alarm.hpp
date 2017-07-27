#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class Mailbox;

namespace util {

// Alarm works similarly to the util::Timer class, with the
// difference it does not require the owning thread to sleep
// in a select(). It is useful for platforms like Android
// where you cannot set a timer on the main thread because the
// NDK does not have a concept of timeout callback in the ALooper.
class Alarm : private util::noncopyable {
public:
    Alarm();
    ~Alarm();

    void start(Duration timeout, std::function<void()>&&);
    void stop();

private:
    class Impl;

    void fired();

    std::shared_ptr<Mailbox> mailbox;
    util::Thread<Impl> impl;
    std::function<void()> callback;
};

} // namespace util
} // namespace mbgl
