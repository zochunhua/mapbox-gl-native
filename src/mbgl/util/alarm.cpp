#include <mbgl/util/alarm.hpp>

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <cassert>

namespace mbgl {
namespace util {

class Alarm::Impl {
public:
    Impl(ActorRef<Alarm::Impl>, ActorRef<Alarm> parent_) : parent(parent_) {}

    void start(Duration timeout) {
        alarm.start(timeout, mbgl::Duration::zero(), [this]() {
            parent.invoke(&Alarm::fired);
        });
    }

    void stop() {
        alarm.stop();
    }

private:
    Timer alarm;
    ActorRef<Alarm> parent;
};

Alarm::Alarm()
    // FIXME: Replace with util::Scheduler::Get()
    : mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get()))
    , impl("Alarm", ActorRef<Alarm>(*this, mailbox)) {
}

Alarm::~Alarm() {
    mailbox->close();
}

void Alarm::start(Duration timeout, std::function<void()>&& cb) {
    assert(cb);
    callback = std::move(cb);

    impl.actor().invoke(&Impl::start, timeout);
}

void Alarm::stop() {
    impl.actor().invoke(&Impl::stop);
}

void Alarm::fired() {
    callback();
}

} // namespace util
} // namespace mbgl
