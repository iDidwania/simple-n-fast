#ifndef _SNF_HTTP_STATUS_H_
#define _SNF_HTTP_STATUS_H_

#include <string>
#include <ostream>
#include <stdexcept>

namespace snf {
namespace http {

/*
 * HTTP status codes.
 */
enum class status_code : int
{
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTH_INFO = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMP_REDIRECT = 307,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQD = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTH_REQD = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQD = 411,
	PRECOND_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	UPGRADE_REQD = 426,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

const char *reason_phrase(status_code) noexcept;

inline std::ostream &
operator<< (std::ostream &os, status_code s)
{
	os << static_cast<int>(s);
	return os;
}

/*
 * HTTP exception.
 * Encapsulates the error string and the
 * HTTP status code.
 */
class exception : public std::runtime_error
{
private:
	status_code m_status;

public:
	exception(const std::string &msg, status_code s)
		: std::runtime_error(msg)
		, m_status(s)
	{
	}

	exception(const char *msg, status_code s)
		: std::runtime_error(msg)
		, m_status(s)
	{
	}

	virtual ~exception() {}

	status_code get_status_code() const { return m_status; }
};

} // namespace http
} // namespace snf

#endif // _SNF_HTTP_STATUS_H_
