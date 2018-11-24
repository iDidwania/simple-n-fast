#ifndef _SNF_SESSION_H_
#define _SNF_SESSION_H_

#include "sslfcn.h"

namespace snf {
namespace net {
namespace ssl {

class session
{
private:
	SSL_SESSION *m_session = nullptr;

public:
	session(const uint8_t *, size_t);
	session(const std::string &);
	session(const session &);
	session(session &&);
	session(SSL_SESSION *);
	~session();

	const session &operator=(const session &);
	session &operator=(session &&);

	operator SSL_SESSION* () { return m_session; }

	uint8_t *to_bytes(size_t *);
	void to_file(const std::string &);

	uint8_t *get_id(size_t *);
	std::string get_id();
	std::string get_context();
	int get_protocol_version();
	time_t start_time();
	time_t timeout();
	void timeout(time_t);
	bool has_ticket();
};

} // namespace ssl
} // namespace net
} // namespace snf

#endif // _SNF_SESSION_H_
