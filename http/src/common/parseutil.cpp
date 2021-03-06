#include "common.h"
#include "parseutil.h"
#include "charset.h"
#include "status.h"
#include <ostream>
#include <sstream>
#include <iostream>

namespace snf {
namespace http {

void
skip_spaces(const std::string &istr, size_t &i, size_t len)
{
	while ((i < len) && is_whitespace(istr[i]))
		i++;
}

void
skip_comments(const std::string &istr, size_t &i, size_t len)
{
	skip_spaces(istr, i, len);

	if (i >= len)
		return;

	if (istr[i] != '(')
		return;

	++i;

	while (i < len) {
		if (is_commented(istr[i]))
			i++;
		else
			break;
	}

	if ((i >= len) || (istr[i] != ')')) {
		std::ostringstream oss;
		oss << "comment not terminated properly (" << istr << ")";
		throw bad_message(oss.str());
	} else {
		++i;
	}
}

std::string
parse_token(const std::string &istr, size_t &i, size_t len)
{
	std::string token;

	skip_spaces(istr, i, len);

	while (i < len) {
		if (is_tchar(istr[i]))
			token.push_back(std::tolower(istr[i++]));
		else
			break;
	}

	return token;
}

param_vec_t
parse_parameter(const std::string &istr, size_t &i, size_t len)
{
	std::string name;
	std::string value;
	param_vec_t parameters;
	std::ostringstream oss;

	while (i < len) {
		skip_spaces(istr, i, len);

		if (i >= len)
			break;

		if (istr[i] == ';')
			i++;

		skip_spaces(istr, i, len);

		if (i >= len)
			break;

		bool processing_name = true;
		bool quoted = false;

		for (; i < len; ++i) {
			bool error = false;

			if (!quoted) {
				if (istr[i] == ';')
					break;

				if (is_whitespace(istr[i]))
					break;
			}

			if (istr[i] == '=') {
				processing_name = false;
			} else if ((istr[i] == '"') && !processing_name) {
				quoted = !quoted;
			} else {
				if (processing_name) {
					if (is_tchar(istr[i]))
						name.push_back(std::tolower(istr[i]));
					else
						error = true;
				} else {
					if (quoted) {
						if (is_quoted(istr[i]))
							value.push_back(istr[i]);
						else
							error = true;
					} else {
						if (is_tchar(istr[i]))
							value.push_back(istr[i]);
						else
							error = true;
					}
				}

				if (error) {
					oss << "invalid character in parameter "
						<< (processing_name ? "name" : "value");
					throw bad_message(oss.str());
				}
			}
		}

		if (quoted) {
			oss << "parameter value (" << value << ") does not terminate with \" for " << name;
			throw bad_message(oss.str());
		}

		if (!name.empty())
			parameters.push_back(std::make_pair(name, value));
		else
			throw bad_message("parameter name is empty");

		name.clear();
		value.clear();
	}

	return parameters;
}

std::vector<std::string>
parse_list(const std::string &istr)
{
	std::vector<std::string> values;
	std::stringstream ss(istr);

	while (ss.good()) {
		std::string val, tval;
		std::getline(ss, val, ',');

		if (!val.empty())
			tval = std::move(snf::trim(val));

		if (!tval.empty())
			values.push_back(tval);
	}

	return values;
}

std::string
parse_generic(const std::string &istr, size_t &i, size_t len)
{
	std::string value;
	int dquoted = 0;
	int commented = 0;
	bool escaped = false;

	while ((i < len) && is_whitespace(istr[i]))
		i++;

	for (; i < len; ++i) {
		if (escaped) {
			if (is_escaped(istr[i]))
				escaped = false;
			else
				break;
		} else if (dquoted) {
			if (istr[i] == '"')
				dquoted--;
			else if (!is_quoted(istr[i]))
				break;
		} else if (commented) {
			if (istr[i] == ')')
				commented--;
			else if (!is_commented(istr[i]))
				break;
		} else if (istr[i] == '\\') {
			if (dquoted || commented)
				escaped = true;
			else
				break;
		} else if (istr[i] == '"') {
			dquoted++;
		} else if (istr[i] == '(') {
			commented++;
		} else if (!is_vchar(istr[i]) && !is_whitespace(istr[i])) {
			break;
		}

		if (!escaped)
			value.push_back(istr[i]);
	}

	if (dquoted)
		throw bad_message("mismatch quote");

	if (commented)
		throw bad_message("mismatch comment");

	if ((value.front() == '"') && (value.back() == '"')) {
		value = value.substr(1, value.length() - 2);
	}

	return value;
}

} // namespace http
} // namespace snf
