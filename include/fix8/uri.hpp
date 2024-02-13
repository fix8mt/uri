//-----------------------------------------------------------------------------------------
//
// uri (header only)
// Copyright (C) 2024 Fix8 Market Technologies Pty Ltd
//   by David L. Dight
// see https://github.com/fix8mt/uri
//
// Lightweight header-only C++20 URI parser
//
// Distributed under the Boost Software License, Version 1.0 August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------------------
#ifndef FIX8_URI_HPP_
#define FIX8_URI_HPP_

//----------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <array>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <cstdint>
#include <vector>
#include <list>
#include <algorithm>
#include <bit>

//-----------------------------------------------------------------------------------------
namespace FIX8 {

//-----------------------------------------------------------------------------------------
class basic_uri
{
public:
	using value_pair = std::pair<std::string_view, std::string_view>;
	using query_result = std::vector<value_pair>;
	using uri_len_t = std::uint16_t;
	using range_pair = std::pair<uri_len_t, uri_len_t>; // offset, len
	enum component { scheme, authority, userinfo, user, password, host, port, path, query, fragment, countof };
	enum class error : uri_len_t { no_error, too_long, illegal_chars, empty_src, countof };
	static constexpr const auto uri_max_len {UINT16_MAX};
	using comp_pair = std::pair<component, std::string_view>;
	using comp_list = std::vector<std::string_view>;
private:
	std::string_view _source;
	std::array<range_pair, component::countof> _ranges{};
	uri_len_t _present{};
	static constexpr const std::array _component_names { "scheme", "authority", "userinfo", "user", "password", "host", "port", "path", "query", "fragment", };
	static constexpr bool query_comp(const value_pair& pl, const value_pair& pr) noexcept { return pl.first < pr.first; }
public:
	constexpr basic_uri(std::string_view src) noexcept : _source(src) { parse(); }
	constexpr basic_uri(int bits) noexcept : _present(bits) {}
	constexpr basic_uri() = default;
	~basic_uri() = default;

	constexpr int assign(std::string_view src)
	{
		_source = src;
		clear();
		return parse();
	}
	constexpr std::string_view get_uri() const noexcept { return _source; }
	constexpr std::string_view get(component what) const noexcept { return _source.substr(_ranges[what].first, _ranges[what].second); }
	constexpr std::string_view get_component(component what) const noexcept { return what < countof ? get(what) : std::string_view(); }

	/*! Provides const direct access to the offset and length of the specifed component and is used to create a `std::string_view`.
	  	\param idx index into table
		\return a `const range_pair&` which is a `std::pair<uri_len_t, uri_len_t>&` to the specified component at the index given in the ranges table. */
	constexpr const range_pair& operator[](component idx) const noexcept { return _ranges[idx]; }

	/*! Provides direct access to the offset and length of the specifed component and is used to create a `std::string_view`. USE CAREFULLY.
	  	\param idx index into table
		\return a `range_pair&` which is a `std::pair<uri_len_t, uri_len_t>&` to the specified component at the index given in the ranges table. */
	constexpr range_pair& operator[](component idx) noexcept { return _ranges[idx]; }

	constexpr value_pair get_named_pair(component what) const noexcept
	{
		return what < countof ? value_pair(_component_names[what], get(what)) : value_pair();
	}
	constexpr int count() const noexcept { return std::popcount(_present); } // upgrade to std::bitset when constexpr in c++23
	constexpr uri_len_t get_present() const noexcept { return _present; }
	constexpr void set(component what=countof) noexcept { what == countof ? _present = (1 << countof) - 1 : _present |= (1 << what); }
	constexpr void clear(component what=countof) noexcept { what == countof ? _present = 0 : _present &= ~(1 << what); }
	constexpr bool any_authority() const noexcept { return _present & (1 << host | 1 << password | 1 << port | 1 << user | 1 << userinfo); };
	constexpr bool test(component what=countof) const noexcept { return what == countof ? _present : _present & (1 << what); }
	constexpr operator bool() const noexcept { return count(); }
	constexpr error get_error() const noexcept { return test() ? error::no_error : static_cast<error>(_ranges[0].first); }
	constexpr void set_error(error what) noexcept
	{
		if (!test())
			_ranges[0].first = static_cast<uri_len_t>(what);
	}
	constexpr int parse() noexcept
	{
		using namespace std::literals;
		while(true)
		{
			if (_source.empty())
				set_error(error::empty_src);
			else if (_source.size() > uri_max_len)
				set_error(error::too_long);
			else if (_source.find_first_of(" \t\n\f\r\v"sv) != std::string_view::npos)
			{
				auto qur { _source.find_first_of('?') }, sps { _source.find_first_of(' ') };
				if (qur != std::string_view::npos && sps != std::string_view::npos && qur < sps)
					break;
				set_error(error::illegal_chars);
			}
			else
				break;
			return 0;	// refuse to parse
		}
		std::string_view::size_type pos{}, hst{}, pth{std::string_view::npos};
		bool scq{};
		if (const auto sch {_source.find_first_of(':')}; sch != std::string_view::npos)
		{
			_ranges[scheme] = {0, sch};
			set(scheme);
			pos = sch + 1;
		}
		if (_source[pos] == '?')	// short circuit query eg. magnet
			scq = true;
		else if (auto auth {_source.find("//"sv, pos)}; auth != std::string_view::npos)
		{
			auth += 2;
			if ((pth = _source.find_first_of('/', auth)) == std::string_view::npos) // unterminated path
				pth = _source.size();
			_ranges[authority] = {auth, pth - auth};
			set(authority);
			if (const auto usr {_source.find_first_of('@', auth)}; usr != std::string_view::npos && usr < pth)
			{
				if (const auto pw {_source.find_first_of(':', auth)}; pw != std::string_view::npos && pw < usr) // no nested ':' before '@'
				{
					_ranges[user] = {auth, pw - auth};
					if (usr - pw - 1 > 0)
					{
						_ranges[password] = {pw + 1, usr - pw - 1};
						set(password);
					}
				}
				else
					_ranges[user] = {auth, usr - auth};
				set(user);
				_ranges[userinfo] = {auth, usr - auth};
				set(userinfo);
				hst = pos = usr + 1;
			}
			else
				hst = pos = auth;

			if (auto prt { _source.find_first_of(':', pos) }; prt != std::string_view::npos)
			{
				if (auto autstr {get(authority)}; autstr.front() != '[' && autstr.back() != ']')
				{
					++prt;
					if (_source.size() - prt > 0)
					{
						_ranges[port] = {prt, _source.size() - prt};
						set(port);
					}
				}
			}
		}
		if (pth != std::string_view::npos)
		{
			if (test(port))
			{
				if (pth - _ranges[port].first == 0) // remove empty port
					clear(port);
				else
					_ranges[port].second = pth - _ranges[port].first;
				_ranges[host] = {hst, _ranges[port].first - 1 - hst};
			}
			else
				_ranges[host] = {hst, pth - hst};
			if (_ranges[host].second)
				set(host);
			_ranges[path] = {pth, _source.size() - pth};
			set(path);
		}
		if (pth == std::string_view::npos && !scq)
		{
			set(path);
			if ((pth = _source.find_first_of('/', pos)) != std::string_view::npos)
				_ranges[path] = {pth, _source.size() - pth};
			else if (test(scheme))
				_ranges[path] = {pos, _source.size() - pos};
			else
				clear(path);
		}
		if (const auto qur {_source.find_first_of('?', pos)}; qur != std::string_view::npos)
		{
			if (test(path))
				_ranges[path].second = qur - _ranges[path].first;
			_ranges[query] = {qur + 1, _source.size() - qur};
			set(query);
		}
		if (const auto fra {_source.find_first_of('#', pos)}; fra != std::string_view::npos)
		{
			if (test(query))
				_ranges[query].second = fra - _ranges[query].first;
			_ranges[fragment] = {fra + 1, _source.size() - fra};
			set(fragment);
		}
		return count();
	}

	template<char valuepair='&',char valueequ='='>
	constexpr query_result decode_query(bool sort=false) const
	{
		constexpr auto decpair([](std::string_view src) noexcept ->value_pair
		{
			if (auto fnd { src.find_first_of(valueequ) }; fnd != std::string_view::npos)
				return {src.substr(0, fnd), src.substr(fnd + 1)};
			else if (src.size())
				return {src, ""};
			return {};
		});
		query_result result;
		if (test(query))
		{
			std::string_view src{get(query)};
			for (std::string_view::size_type pos{};;)
			{
				if (auto fnd { src.find_first_of(valuepair, pos) }; fnd != std::string_view::npos)
				{
					result.emplace_back(decpair(src.substr(pos, fnd - pos)));
					pos = fnd + 1;
					continue;
				}
				if (pos < src.size())
					result.emplace_back(decpair(src.substr(pos, src.size() - pos)));
				break;
			}
		}
		if (sort)
			sort_query(result);
		return result;
	}

	static constexpr void sort_query(query_result& query) noexcept { std::sort(query.begin(), query.end(), query_comp); }
	static constexpr std::string_view find_query(std::string_view what, const query_result& from) noexcept
	{
		auto result { std::equal_range(from.cbegin(), from.cend(), value_pair(what, std::string_view()), query_comp) };
		return result.first == result.second ? std::string_view() : result.first->second;

	}
	static constexpr std::string_view::size_type find_hex(std::string_view src) noexcept
	{
		for (std::string_view::size_type fnd{}; ((fnd = src.find_first_of('%', fnd))) != std::string_view::npos; ++fnd)
		{
			if (fnd + 2 < src.size())
			{
				if (std::isxdigit(static_cast<unsigned char>(src[fnd + 1]))
				 && std::isxdigit(static_cast<unsigned char>(src[fnd + 2])))
					return fnd;
			}
			else
				break;
		}
		return std::string_view::npos;
	}
	static constexpr bool has_hex(std::string_view src) noexcept { return find_hex(src) != std::string_view::npos; }
	static constexpr std::string decode_hex(std::string_view src)
	{
		std::string result{src};
		for (std::string_view::size_type fnd; ((fnd = find_hex(result))) != std::string_view::npos; )
			result.replace(fnd, 3, 1, ((result[fnd + 1] & 0xF) + (result[fnd + 1] >> 6) * 9) << 4
				| ((result[fnd + 2] & 0xF) + (result[fnd + 2] >> 6) * 9));
		return result;
	}
	static constexpr std::string_view get_name(component what) noexcept
	{
		return what < countof ? _component_names[what] : std::string_view();
	}

	static constexpr std::string make_edit(const auto& what, std::initializer_list<comp_pair> from)
	{
		basic_uri ibase;
		comp_list ilist{countof};
		for (component ii{}; ii != countof; ii = component(ii + 1))
		{
			if (what.test(ii))
			{
				ibase.set(ii);
				ilist[ii] = what.get_component(ii);
			}
		}
		for (const auto& [comp,str] : from)
		{
			if (comp < countof)
			{
				ibase.set(comp);
				ilist[comp] = str;
			}
		}
		if (!ibase.test())
			return 0;
		if (ibase.any_authority())
			ibase.clear(authority);
		if (ibase.test(userinfo) && (ibase.test(user) || ibase.test(password)))
			ibase.clear(userinfo);
		return make_uri(ibase, std::move(ilist));
	}

	static constexpr std::string make_uri(std::initializer_list<comp_pair> from) noexcept
	{
		basic_uri ibase;
		comp_list ilist{countof};
		for (const auto& [comp,str] : from)
		{
			if (comp < countof)
			{
				ibase.set(comp);
				ilist[comp] = str;
			}
		}
		return make_uri(ibase, std::move(ilist));
	}

	static constexpr std::string make_uri(basic_uri ibase, comp_list ilist) noexcept
	{
		if (!ibase.test())
			return {};
		using namespace std::literals;
		basic_uri done;
		std::string result;
		for (component ii{}; ii != countof; ii = component(ii + 1))
		{
			if (!ibase.test(ii) || done.test(ii))
				continue;
			switch(const auto& str{ilist[ii]}; ii)
			{
			case scheme:
				result += str;
				result += ':';
				if (ibase.any_authority())
					result += "//"sv;
				break;
			case authority:
				if (!ibase.any_authority())
					result += "//"sv;
				result += str;
				break;
			case userinfo:
				if (ibase.test(authority) || ibase.test(user) || ibase.test(password))
					continue;
				result += str;
				break;
			case user:
				if (str.empty() && (ibase.test(authority) || ibase.test(userinfo)))
					continue;
				result += str;
				break;
			case password:
				if (ibase.test(authority) || ibase.test(userinfo))
					continue;
				if (!str.empty())
				{
					result += ':';
					result += str;
				}
				break;
			case host:
				if (ibase.test(authority))
					continue;
				if ((!ilist[user].empty() || !ilist[password].empty()) && (done.test(user) || done.test(password)))
					result += '@';
				result += str;
				break;
			case port:
				if (ibase.test(authority))
					continue;
				if (!str.empty())
				{
					result += ':';
					result += str;
				}
				break;
			case path:
				result += str;
				break;
			case query:
				if (!str.empty())
				{
					result += '?';
					result += str;
				}
				break;
			case fragment:
				if (!str.empty())
				{
					result += '#';
					result += str;
				}
				break;
			default:
				continue;
			}
			done.set(ii);
		}
		return result;
	}

	friend std::ostream& operator<<(std::ostream& os, const basic_uri& what)
	{
		if (!what)
			os << "error: " << static_cast<int>(what.get_error()) << '\n';
		os << std::setw(12) << std::left << "uri" << what._source << '\n';
		for (component ii{}; ii != countof; ii = component(ii + 1))
		{
			if (what.test(ii))
			{
				os << std::setw(12) << std::left << what.get_name(ii)
					<< (what.get(ii).size() ? what.get(ii) : "(empty)") << '\n';
				if (ii == query)
					for (auto qresult { what.decode_query() }; const auto [tag,value] : qresult)
						os << "   " << std::setw(12) << std::left << tag << (value.size() ? value : "(empty)") << '\n';
			}
		}
		return os;
	}
};

//-----------------------------------------------------------------------------------------
/// static storage
template<size_t sz>
class uri_storage
{
	std::array<char, sz> _buffer;
	size_t _sz{};
protected:
	constexpr uri_storage(std::string src) noexcept : _sz(src.size() > sz ? 0 : src.size())
		{ std::copy_n(src.cbegin(), _sz, _buffer.begin()); }
	constexpr uri_storage() = default;
	~uri_storage() = default;
	constexpr std::string swap(std::string src) noexcept
	{
		if (src.size() > sz)
			return {};
		std::string old(_buffer.cbegin(), _sz);
		std::copy_n(src.cbegin(), _sz = src.size(), _buffer.begin());
		return old;
	}
public:
	constexpr std::string_view buffer() const noexcept { return {_buffer.cbegin(), _sz}; }
	static constexpr auto max_storage() noexcept { return sz; }
};

//-----------------------------------------------------------------------------------------
/// specialisation: dynamic storage
template<>
class uri_storage<0>
{
	std::string _buffer;
protected:
	constexpr uri_storage(std::string src) noexcept : _buffer(std::move(src)) {}
	constexpr uri_storage() = default;
	~uri_storage() = default;
	constexpr std::string swap(std::string src) noexcept { return std::exchange(_buffer, std::move(src)); }
public:
	constexpr std::string_view buffer() const noexcept { return _buffer; }
	static constexpr auto max_storage() noexcept { return basic_uri::uri_max_len; }
};

//-----------------------------------------------------------------------------------------
class uri : public uri_storage<0>, public basic_uri
{
public:
	constexpr uri(std::string src, bool decode=true) noexcept
		: uri_storage(decode && uri::has_hex(src) ? uri::decode_hex(src) : std::move(src)), basic_uri(buffer()) {}
	constexpr uri() = default;
	~uri() = default;

	constexpr std::string replace(std::string src)
	{
		auto rbuf { swap(std::move(src)) };
		assign(buffer());
		return rbuf;
	}
	constexpr int edit(std::initializer_list<comp_pair> from)
	{
		replace(make_edit(*this, std::move(from)));
		return count();
	}
	static constexpr auto factory(std::initializer_list<comp_pair> from) noexcept
	{
		return uri(make_uri(std::move(from)));
	}
};

//-----------------------------------------------------------------------------------------
template<size_t sz=1024>
class uri_static : public uri_storage<sz>, public basic_uri
{
public:
	constexpr uri_static(std::string src, bool decode=true) noexcept
		: uri_storage<sz>(decode && uri::has_hex(src) ? uri::decode_hex(src) : std::move(src)), basic_uri(this->buffer()) {}
	constexpr uri_static() = default;
	~uri_static() = default;

	constexpr std::string replace(std::string src)
	{
		auto rbuf { this->swap(std::move(src)) };
		assign(this->buffer());
		return rbuf;
	}
	constexpr int edit(std::initializer_list<comp_pair> from)
	{
		replace(make_edit(*this, std::move(from)));
		return count();
	}
	static constexpr auto factory(std::initializer_list<comp_pair> from) noexcept
	{
		return uri_static(make_uri(std::move(from)));
	}
};

} // FIX8
#endif // FIX8_URI_HPP_
