#include <iostream>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>

#define LOG std::cout << "[" << boost::this_thread::get_id() << "] "

std::atomic<int> cfoo{0}, cbar{0}, cbaz{0};

void slot_foo()
{
    ++cfoo;
    LOG << "Hello from foo" << std::endl;
}

void slot_bar(double val)
{
    ++cbar;
    LOG << "Hello from bar val: " << val << std::endl;
}

void slot_baz(int val, const std::string& str)
{
    ++cbaz;
    LOG << "Hello from baz val: " << val << " str: " << str << std::endl;
}

boost::atomic<bool> exited(false);
boost::asio::io_service bg_io;

void thread_run()
{
    LOG << "Thread start" << std::endl;
    while (!exited) {
        bg_io.run();
        if (bg_io.stopped()) bg_io.reset();
    }

    LOG << "Thread done" << std::endl;
}

std::string baz_string = "hello baz";

int main()
{
    LOG << "Hello from MainThread" << std::endl;

    boost::asio::io_service io; // in current thread
    boost::thread t(thread_run);

    LOG << "Connecting signals" << std::endl;
    boost::signals2::signal<void ()> foo_sig;
    boost::signals2::signal<void (double)> bar_sig;
    boost::signals2::signal<void (int, const std::string&)> baz_sig;

    foo_sig.connect(slot_foo);
    foo_sig.connect(io.wrap(slot_foo));
    foo_sig.connect(bg_io.wrap(slot_foo));

    bar_sig.connect(slot_bar);
    bar_sig.connect(io.wrap(slot_bar));
    bar_sig.connect(bg_io.wrap(slot_bar));

    baz_sig.connect(slot_baz);
    baz_sig.connect(io.wrap(slot_baz));
    baz_sig.connect(bg_io.wrap(slot_baz));

    LOG << "Foo fires" << std::endl;
    foo_sig();

    LOG << "Now bar" << std::endl;
    bar_sig(9.856);

    LOG << "and baz..." << std::endl;
    baz_sig(13, baz_string);

    LOG << "Flushing tasks" << std::endl;

    io.run();
    exited = true;
    t.join();

    LOG << "Bye bye"
        << " foo: " << cfoo
        << " bar: " << cbar
        << " baz: " << cbaz
        << std::endl;

    return 0;
}

