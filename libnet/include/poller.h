#ifndef _SNF_POLLER_H_
#define _SNF_POLLER_H_

#include "netplat.h"
#include "sock.h"
#include <array>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>

namespace snf {
namespace net {
namespace poller {

#if defined(_WIN32)
constexpr short POLLRD = POLLRDNORM;
constexpr short POLLWR = POLLWRNORM;
#else
constexpr short POLLRD = POLLIN;
constexpr short POLLWR = POLLOUT;
#endif

/*
 * Following events can be registered with the reactor.
 */
enum class event : short
{
	read = POLLRD,
	write = POLLWR,
	// Can only be returned to handler; cannot be requested
	error = POLLERR,
	hup = POLLHUP,         
	invalid = POLLNVAL
};

class reactor
{
private:
	struct ev_info
	{
		event                               e;      // events
		std::function<void(sock_t, event)>  h;      // handler
		bool                                o;      // run only once

		ev_info(event ee, std::function<void(sock_t, event)> &&hh, bool oo)
			: e(ee), h(std::move(hh)), o(oo) {}
		ev_info(const ev_info &ei)
			: e(ei.e) , h(ei.h) , o(ei.o) {}
		ev_info(ev_info &&ei)
			: e(ei.e), h(std::move(ei.h)), o(ei.o) {}

		const ev_info &operator=(const ev_info &ei)
		{
			if (this != &ei) {
				e = ei.e;
				h = ei.h;
				o = ei.o;
			}
			return *this;
		}

		ev_info &operator=(ev_info &&ei)
		{
			if (this != &ei) {
				e = ei.e;
				h = std::move(ei.h);
				o = ei.o;
			}
			return *this;
		}
	};

	using ev_info_type    = std::vector<ev_info>;
	using ev_handler_type = std::map<sock_t, ev_info_type>;

	int                               m_timeout;
	std::atomic<bool>                 m_stopped { false };
	std::array<snf::net::socket, 2>   m_sockpair;
	std::future<void>                 m_future;
	std::mutex                        m_lock;
	ev_handler_type                   m_handlers;

	void set_poll_vector(std::vector<pollfd> &);
	void process_poll_vector(std::vector<pollfd> &, int);
	void start();
	void stop();

public:
	reactor(int to = 5000);
	reactor(const reactor &) = delete;
	reactor(reactor &&) = delete;
	const reactor &operator=(const reactor &) = delete;
	reactor &operator=(reactor &&) = delete;
	~reactor() { stop(); }

	void add_handler(sock_t, event, std::function<void(sock_t, event)>, bool one_shot = false);
	void remove_handler(sock_t);
	void remove_handler(sock_t, event);
};

} // namespace poller
} // namespace net
} // namespace snf

#endif // _SNF_POLLER_H_
