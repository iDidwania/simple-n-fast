#ifndef _SNF_FLOGGER_H_
#define _SNF_FLOGGER_H_

#include <mutex>
#include <file.h>
#include "fattr.h"
#include "logger.h"

namespace snf {
namespace log {

class rotation
{
public:
	enum class scheme : int
	{
		none = 0x00,
		daily = 0x01,
		by_size = 0x02
	};

private:
	scheme  m_scheme;
	int64_t m_size;

public:
	rotation(scheme s = scheme::none) : m_scheme(s), m_size(104857600) {}

	bool daily() const;
	bool by_size() const;
	int64_t size() const { return m_size; }
	void size(int64_t size) { m_size = size; }
	
};

inline rotation::scheme
operator| (const rotation::scheme &s1, const rotation::scheme &s2)
{
	int v1 = static_cast<int>(s1);
	int v2 = static_cast<int>(s2);
	return static_cast<rotation::scheme>(v1 | v2);
}

inline rotation::scheme
operator& (const rotation::scheme &s1, const rotation::scheme &s2)
{
	int v1 = static_cast<int>(s1);
	int v2 = static_cast<int>(s2);
	return static_cast<rotation::scheme>(v1 & v2);
}

inline bool
rotation::daily() const
{
	return (m_scheme & scheme::daily) == scheme::daily;
}

inline bool
rotation::by_size() const
{
	return (m_scheme & scheme::by_size) == scheme::by_size;
}

class retention
{
public:
	enum class scheme : int
	{
		all,
		last_n_days,
		last_n_files
	};

private:
	scheme  m_scheme;
	int     m_args;

public:
	retention(scheme s = scheme::all, int args = -1) : m_scheme(s), m_args(args) {}

	bool retain_by_days() const { return (m_scheme == scheme::last_n_days); }
	bool retain_by_file_count() const { return (m_scheme == scheme::last_n_files); }
	int get_argument() const { return m_args; }
};

class file_logger : public logger
{
private:
	std::string m_path;
	bool        m_make_path = false;
	std::string m_name_fmt;
	std::string m_name;
	int64_t     m_size = 0;
	int         m_seqno = 0;
	int         m_last_day = 0;
	rotation    *m_rotation = nullptr;
	retention   *m_retention = nullptr;
	snf::file   *m_file = nullptr;
	std::mutex  m_file_lock;

	void init_name_format()
	{
		if (m_name_fmt.empty())
			m_name_fmt = "%D_%N.log";
	}

	std::string regex_pattern(bool capture = false,
		int *date_index = nullptr, int *seq_index = nullptr);
	void parse_name(const std::string &, std::string &, std::string &);
	std::string name(const snf::local_time &);
	std::string name(const snf::file_attr &, const snf::local_time &);
	int open();
	void close();
	void open(const snf::local_time &);
	void rotate(const snf::local_time &);

	bool rotate_daily()
	{
		return (m_rotation && m_rotation->daily());
	}

	bool rotate_by_size(int64_t fsize)
	{
		return (m_rotation && m_rotation->by_size() && (fsize > m_rotation->size()));
	}

public:
	file_logger(
		const std::string &path,
		severity sev,
		const std::string &fmt = "%D %T %p.%t [%s] [%C] [%F:%c.%f.%l] %m")
		: logger(sev, fmt)
		, m_path(path)
	{
	}

	virtual ~file_logger()
	{
		std::lock_guard<std::mutex> guard(m_file_lock);
		delete m_file;
		delete m_retention;
		delete m_rotation;
	}

	type get_type() const { return logger::type::file; }

	const std::string & get_path() const { return m_path; }

	bool make_path() const { return m_make_path; }
	void make_path(bool mp) { m_make_path = mp; }

	const std::string & get_name_format() const { return m_name_fmt; }
	void set_name_format(const std::string &fmt) { m_name_fmt = fmt; }

	const rotation *get_rotation() const { return m_rotation; }
	void set_rotation(rotation *r) { delete m_rotation; m_rotation = r; }

	const retention *get_retention() const { return m_retention; }
	void set_retention(retention *r) { delete m_retention; m_retention = r; }

	void log(const record &);
};

} // namespace log
} // namespace snf

#endif // _SNF_FLOGGER_H_
