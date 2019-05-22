#ifndef _SNF_HTTP_HANDLER_H_
#define _SNF_HTTP_HANDLER_H_

#include "sock.h"
#include "reactor.h"

namespace snf {
namespace http {

class accept_handler : public snf::net::handler
{
protected:
	snf::net::socket    &m_sock;
	snf::net::event     m_event;
	bool                m_secured;

public:
	accept_handler(snf::net::socket &s, snf::net::event e, bool secured = false)
		: m_sock(s)
		, m_event(e)
		, m_secured(secured)
	{
	}

	bool is_secured() const { return m_secured; }

	virtual ~accept_handler() {};

	virtual bool operator()(sock_t, snf::net::event) override;
};

} // namespace http
} // namespace snf

#endif // _SNF_HTTP_HANDLER_H_
