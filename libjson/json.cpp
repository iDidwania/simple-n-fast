#include "json.h"
#include "misc.h"
#include <sstream>
#include <stdexcept>

namespace snf {
namespace json {

object::object(std::initializer_list<member_t> kvpairs)
{
	for (auto &i : kvpairs)
		m_members.insert(i);
}

const object &
object::operator=(const object &o)
{
	if (this != &o) {
		m_members.clear();
		m_members = o.m_members;
	}
	return *this;
}

object &
object::operator=(object &&o)
{
	if (this != &o) {
		m_members.clear();
		m_members = std::move(o.m_members);
	}
	return *this;
}

value &
object::operator[] (const std::string &key)
{
	std::string &&k = string_unescape(key);
	return m_members[k];
}

object &
object::add(const std::string &key, const value &val)
{
	std::string &&k = string_unescape(key);
	m_members.insert(std::make_pair(k, val));
	return *this;
}

object &
object::add(const member_t &kvpair)
{
	m_members.insert(kvpair);
	return *this;
}

bool
object::contains(const std::string &key) const
{
	std::string &&k = string_unescape(key);
	return (m_members.end() != m_members.find(k));
}

const value &
object::get(const std::string &key) const
{
	std::string &&k = string_unescape(key);
	auto i = m_members.find(k);
	if (i == m_members.end()) {
		std::ostringstream oss;
		oss << "key " << key << " not found in JSON object!";
		throw std::runtime_error(oss.str());
	} else {
		return i->second;
	}
}

const value &
object::get(const std::string &key, const value &default_value) const
{
	std::string &&k = string_unescape(key);
	auto i = m_members.find(k);
	if (i == m_members.end()) {
		return default_value;
	} else {
		return i->second;
	}
}

std::string
object::str(const member_t &kvpair, bool pretty, int indent) const
{
	std::ostringstream oss;

	if (pretty)
		oss << std::string(indent, ' ');

	oss	<< "\""
		<< string_escape(kvpair.first)
		<< "\" : "
		<< kvpair.second.str(pretty, indent);

	return oss.str();
}

std::string
object::str(bool pretty, int indent) const
{
	std::ostringstream oss;

	oss << "{";

	for (auto i = m_members.begin(); i != m_members.end(); ++i) {
		if (i != m_members.begin()) {
			oss << ",";
		}

		if (pretty)
			oss << std::endl;
		else
			oss << " ";

		oss << str(*i, pretty, indent + 2);
	}

	if (pretty)
		oss << std::endl << std::string(indent, ' ');
	else
		oss << " ";

	oss << "}";

	return oss.str();
}

array::array(std::initializer_list<value> elements)
{
	for (auto &i : elements)
		m_elements.emplace_back(i);
}

const array &
array::operator=(const array &a)
{
	if (this != &a) {
		m_elements.clear();
		m_elements = a.m_elements;
	}
	return *this;
}

array &
array::operator=(array &&a)
{
	if (this != &a) {
		m_elements.clear();
		m_elements = std::move(a.m_elements);
	}
	return *this;
}

value &
array::operator[] (size_t index)
{
	if (!valid(index))
		if (index >= size())
			m_elements.resize(index + 1);
	return m_elements[index];
}

array &
array::add(const value &elem)
{
	m_elements.emplace_back(elem);
	return *this;
}

const value &
array::get(size_t index) const
{
	if (valid(index)) {
		return m_elements[index];
	} else {
		std::ostringstream oss;
		oss << "index " << index << " is out of range in JSON array!";
		throw std::range_error(oss.str().c_str());
	}
}

const value &
array::get(size_t index, const value &default_value) const
{
	if (valid(index)) {
		return m_elements[index];
	} else {
		return default_value;
	}
}

std::string
array::str(bool pretty, int indent) const
{
	std::ostringstream oss;

	oss << "[";

	for (auto i = m_elements.begin(); i != m_elements.end(); ++i) {
		if (i != m_elements.begin()) {
			oss << ",";
		}

		if (pretty)
			oss << std::endl;
		else
			oss << " ";

		oss << i->str(pretty, indent + 2);
	}

	if (pretty)
		oss << std::endl << std::string(indent, ' ');
	else
		oss << " ";

	oss << "]";

	return oss.str();
}

value::V::V() : i_val(0) {}
value::V::V(bool b) : b_val(b) {}
value::V::V(int64_t i) : i_val(i) {}
value::V::V(double d) : d_val(d) {}

value::V::V(const char *s)
{
	std::string &&str = string_unescape(s);
	s_val = new std::string(str);
}

value::V::V(const std::string &s)
{
	std::string &&str = string_unescape(s);
	s_val = new std::string(str);
}

value::V::V(std::string &&s)
{
	std::string &&str = string_unescape(s);
	s_val = new std::string(std::move(str));
}

value::V::V(const object &o) : o_val(new object(o)) {}
value::V::V(object &&o) : o_val(new object(std::move(o))) {}
value::V::V(const array &a) : a_val(new array(a)) {}
value::V::V(array &&a) : a_val(new array(std::move(a))) {}

void
value::clean(value &v)
{
	switch (v.m_type) {
	case T::T_STRING:
		if (v.m_val.s_val)
			delete v.m_val.s_val;
		break;

	case T::T_OBJECT:
		if (v.m_val.o_val)
			delete v.m_val.o_val;
		break;

	case T::T_ARRAY:
		if (v.m_val.a_val)
			delete v.m_val.a_val;
		break;

	default:
		break;
	}
	v.m_type = T::T_NULL;
	v.m_val.i_val = 0L;
}

void
value::copy(const value &v)
{
	switch (v.m_type) {
	case T::T_BOOLEAN:
		m_val.b_val = v.m_val.b_val;
		break;

	case T::T_INTEGER:
		m_val.i_val = v.m_val.i_val;
		break;

	case T::T_REAL:
		m_val.d_val = v.m_val.d_val;
		break;

	case T::T_STRING:
		m_val.s_val = new std::string(*v.m_val.s_val);
		break;

	case T::T_OBJECT:
		m_val.o_val = new object(*v.m_val.o_val);
		break;

	case T::T_ARRAY:
		m_val.a_val = new array(*v.m_val.a_val);
		break;

	default:
		m_val.i_val = 0;
		break;
	}
	m_type = v.m_type;
}

void
value::move(value &&v)
{
	switch (v.m_type) {
	case T::T_BOOLEAN:
		m_val.b_val = v.m_val.b_val;
		break;

	case T::T_INTEGER:
		m_val.i_val = v.m_val.i_val;
		break;

	case T::T_REAL:
		m_val.d_val = v.m_val.d_val;
		break;

	case T::T_STRING:
		m_val.s_val = new std::string(std::move(*v.m_val.s_val));
		break;

	case T::T_OBJECT:
		m_val.o_val = new object(std::move(*v.m_val.o_val));
		break;

	case T::T_ARRAY:
		m_val.a_val = new array(std::move(*v.m_val.a_val));
		break;

	default:
		m_val.i_val = 0;
		break;
	}
	m_type = v.m_type;
	clean(v);
}

const value &
value::operator= (bool b)
{
	clean(*this);
	m_type = T::T_BOOLEAN;
	m_val.b_val = b;
	return *this;
}

const value &
value::operator= (int64_t i)
{
	clean(*this);
	m_type = T::T_INTEGER;
	m_val.i_val = i;
	return *this;
}

const value &
value::operator= (double d)
{
	clean(*this);
	m_type = T::T_REAL;
	m_val.d_val = d;
	return *this;
}

const value &
value::operator= (const std::string &s)
{
	clean(*this);
	std::string &&str = string_unescape(s);
	m_type = T::T_STRING;
	m_val.s_val = new std::string(str);
	return *this;
}

const value &
value::operator= (std::string &&s)
{
	clean(*this);
	std::string &&str = string_unescape(s);
	m_type = T::T_STRING;
	m_val.s_val = new std::string(std::move(str));
	return *this;
}

const value &
value::operator= (const object &o)
{
	clean(*this);
	m_type = T::T_OBJECT;
	m_val.o_val = new object(o);
	return *this;
}

const value &
value::operator= (object &&o)
{
	clean(*this);
	m_type = T::T_OBJECT;
	m_val.o_val = new object(std::move(o));
	return *this;
}

const value &
value::operator= (const array &a)
{
	clean(*this);
	m_type = T::T_ARRAY;
	m_val.a_val = new array(a);
	return *this;
}

const value &
value::operator= (array &&a)
{
	clean(*this);
	m_type = T::T_ARRAY;
	m_val.a_val = new array(std::move(a));
	return *this;
}

const value &
value::operator= (const value &v)
{
	if (this != &v)
		copy(v);
	return *this;
}

const value &
value::operator= (value &&v)
{
	if (this != &v)
		move(std::move(v));
	return *this;
}

value &
value::operator[] (const std::string &key)
{
	if (!is_object()) {
		clean(*this);
		m_type = T::T_OBJECT;
		m_val.o_val = new object();
	}
	return m_val.o_val->operator[](key);
}

value &
value::operator[] (size_t index)
{
	if (!is_array()) {
		clean(*this);
		m_type = T::T_ARRAY;
		m_val.a_val = new array();
	}
	return m_val.a_val->operator[](index);
}

bool
value::get_boolean() const
{
	if (is_boolean()) {
		return m_val.b_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

int64_t
value::get_integer() const
{
	if (is_integer()) {
		return m_val.i_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

double
value::get_real() const
{
	if (is_real()) {
		return m_val.d_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

const std::string &
value::get_string() const
{
	if (is_string()) {
		return *m_val.s_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

const object &
value::get_object() const
{
	if (is_object()) {
		return *m_val.o_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

const array &
value::get_array() const
{
	if (is_array()) {
		return *m_val.a_val;
	} else {
		throw std::logic_error("invalid JSON value type!");
	}
}

std::string
value::str(bool pretty, int indent) const
{
	std::ostringstream oss;

	switch (m_type) {
	case T::T_NULL:
		oss << "null";
		break;

	case T::T_BOOLEAN:
		if (m_val.b_val)
			oss << "true";
		else
			oss << "false";
		break;

	case T::T_INTEGER:
		oss << m_val.i_val;
		break;

	case T::T_REAL:
		oss << m_val.d_val;
		break;

	case T::T_STRING:
		oss << "\"" << string_escape(*m_val.s_val) << "\"";
		break;

	case T::T_OBJECT:
		oss << m_val.o_val->str(pretty, indent);
		break;

	case T::T_ARRAY:
		oss << m_val.a_val->str(pretty, indent);
		break;
	}

	return oss.str();
}

} // json
} // snf
