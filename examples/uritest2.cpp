//-----------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string_view>
#include <fix8/uri.hpp>

//-----------------------------------------------------------------------------------------
using namespace FIX8;
using namespace std::literals;

//-----------------------------------------------------------------------------------------
#include "uriexamples.hpp"

//-----------------------------------------------------------------------------------------
// run as: ctest --output-on-failure
//-----------------------------------------------------------------------------------------
TEST_CASE("get component")
{
	const uri u1{tests[0].first};
	REQUIRE_NOTHROW(u1.get_component(host));
	REQUIRE(u1.get_component(host) == "www.blah.com");
	REQUIRE(u1.get(host) == "www.blah.com");
	REQUIRE(u1.get(fragment) == "");
	REQUIRE(u1.get_component(countof) == "");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("operators")
{
	uri u1{tests[0].first};
	REQUIRE(u1.test());
	const auto [tag,value] { u1[host] };
	REQUIRE(tag == 8);
	REQUIRE(value == 12);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("bitset")
{
	uri u1{tests[0].first};
	REQUIRE(u1.get_present() == 0b0010100011);
	u1.clear();
	REQUIRE(u1.get_present() == 0);
	u1.set(uri::countof);
	REQUIRE(u1.get_present() == 0b1111111111);
	basic_uri b1{0b1111111111};
	REQUIRE (b1.get_component(scheme) == "");
	REQUIRE (b1.get_component(host) == "");
	b1.clear(scheme);
	REQUIRE(b1.get_present() == 0b1111111110);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("get name")
{
	REQUIRE_NOTHROW(uri::get_name(host));
	REQUIRE(uri::get_name(host) == "host");
	REQUIRE(uri::get_name(countof) == "");
}

//-----------------------------------------------------------------------------------------
void run_test_comp(int id, const auto& ui)
{
	const auto& vec { tests[id].second };
	INFO("uri: " << id); // << ' ' << uri{u1});
	REQUIRE (ui.count() == vec.size());
	for (const auto& [comp,str] : vec)
	{
		INFO("component: " << comp);
		REQUIRE (ui.get_component(comp) == str);
	}
}

TEST_CASE("uri component validations")
{
	for (int ii{}; ii < tests.size(); ++ii)
		run_test_comp(ii, uri{tests[ii].first});
}

TEST_CASE("uri component validations (static)")
{
	using stat_uri = uri_static<>;
	for (int ii{}; ii < tests.size(); ++ii)
	{
		REQUIRE(std::string_view(tests[ii].first).size() < stat_uri::max_storage());
		run_test_comp(ii, stat_uri{tests[ii].first});
	}
}

//-----------------------------------------------------------------------------------------
TEST_CASE("get named pair")
{
	const uri u1{tests[0].first};
	REQUIRE_NOTHROW(u1.get_named_pair(host));
	REQUIRE(u1.get_named_pair(countof).second == "");
	const auto [tag,value] { u1.get_named_pair(host) };
	REQUIRE(tag == "host");
	REQUIRE(value == "www.blah.com");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("replace")
{
	const auto& [src,vec] { tests[0] };
	const auto& [src1,vec1] { tests[4] };
	uri u1{src};
	REQUIRE(u1.get_component(host) == "www.blah.com");
	uri u2{u1.replace(src1)};
	REQUIRE(u1.get_component(host) == "example.com");
	REQUIRE(u2.get_component(host) == "www.blah.com");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("replace (static)")
{
	const auto& [src,vec] { tests[0] };
	const auto& [src1,vec1] { tests[4] };
	uri_static<> u1{src};
	REQUIRE(u1.get_component(host) == "www.blah.com");
	uri_static<> u2{u1.replace(src1)};
	REQUIRE(u1.get_component(host) == "example.com");
	REQUIRE(u2.get_component(host) == "www.blah.com");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("storage")
{
	const auto& [src,vec] { tests[0] };
	const uri u1{src};
	REQUIRE(src == u1.get_uri());
	REQUIRE(u1.buffer() == u1.get_uri());
	REQUIRE(u1.buffer() == src);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("invalid uri")
{
	static constexpr const auto baduris { std::to_array<basic_uri>
	({
		"https://www.example.com\n"sv,
		"https://www.example.com\r"sv,
		"https://www. example.com"sv,
		"https://www.example.\tcom"sv,
		"https://www.example.\vcom"sv,
		"https://www.example.\fcom"sv,
	})};
	for (auto pp : baduris)
	{
		REQUIRE(!pp);
		REQUIRE(pp.get_error() == uri::error::illegal_chars);
	}
}

//-----------------------------------------------------------------------------------------
TEST_CASE("limits")
{
	char buff[uri::uri_max_len+1]{};
	std::fill(buff, buff + sizeof(buff), 'x');
	uri u1{buff};
	REQUIRE(!u1);
	REQUIRE(u1.get_error() == uri::error::too_long);
	uri_static<> u2{buff}; // too long
	REQUIRE(u2.get_uri() == "");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("empty")
{
	uri u1{""};
	REQUIRE(!u1);
	REQUIRE(u1.get_error() == uri::error::empty_src);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("hex decode")
{
	std::string_view src { "https://www.netmeister.org/%62%6C%6F%67/%75%72%6C%73.%68%74%6D%6C?!@#$%25=+_)(*&^#top%3C" },
		src1 { "https://www.netmeister.org/blog/urls.html?!@#$%=+_)(*&^#top<" },
		src2 { "https://www.netmeister.org/path#top%3" };
	REQUIRE(uri::has_hex(src));
	REQUIRE(!uri::has_hex(src1));
	REQUIRE(!uri::has_hex(src2));
	auto result { uri::decode_hex(src) };
	REQUIRE(!uri::has_hex(result));
	uri u1{result};
	REQUIRE(u1.get_uri() == src1);
	uri u2(std::string(src), false);
	REQUIRE(uri::has_hex(u2.get_uri()));

	std::string_view src3 { "https://www.netmeister.org/%%62" };
	REQUIRE(uri::has_hex(src3));
}

//-----------------------------------------------------------------------------------------
template<typename T>
void do_decode()
{
	static const typename T::query_result tbl
	{
		{ "payload1", "true" }, { "payload2", "false" }, { "test", "1" }, { "benchmark", "3" }, { "foo", "38.38.011.293" },
		{ "bar", "1234834910480" }, { "test", "19299" }, { "3992", "" }, { "key", "f5c65e1e98fe07e648249ad41e1cfdb0" },
	};
	const T u1{tests[9].first};
	auto result{u1.decode_query()};
	REQUIRE(tbl == result);
	const T u2{tests[8].first};
	auto result1{u2.decode_query()};
	REQUIRE(result1.empty());

	const T u3{ "http://host.com/?payload1:true;payload2:false;test:1;benchmark:3;foo:38.38.011.293"
		";bar:1234834910480;test:19299;3992;key:f5c65e1e98fe07e648249ad41e1cfdb0#test"};
	auto result2{u3.template decode_query<';',':'>()};
	REQUIRE(tbl == result2);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("query decode")
{
	do_decode<uri>();
}

TEST_CASE("query decode (static)")
{
	do_decode<uri_static<>>();
}

//-----------------------------------------------------------------------------------------
TEST_CASE("query search")
{
	static const uri::query_result tbl { { "first", "1st" }, { "second", "2nd" }, { "third", "3rd" } };
	const uri u1{tests[34].first};
	auto result{u1.decode_query(true)}; // auto sort - must be sorted to use find
	auto result1{u1.decode_query()}; // not sorted, sort later
	uri::sort_query(result1);
	REQUIRE(tbl == result);
	REQUIRE(uri::find_query("first", result) == "1st");
	REQUIRE(uri::find_query("second", result) == "2nd");
	REQUIRE(uri::find_query("third", result) == "3rd");
	REQUIRE(uri::find_query("fourth", result) == "");
	REQUIRE(result == result1);
}

//-----------------------------------------------------------------------------------------
template<typename T>
void do_factory()
{
	const auto u1 { T::factory({{scheme, "https"}, {user, "dakka"}, {host, "www.blah.com"}, {port, "3000"}, {path, "/"}}) };
	run_test_comp(3, u1);
	const auto u2 { T::factory({{scheme, "file"}, {authority, ""}, {path, "/foo/bar/test/node.js"}}) };
	run_test_comp(8, u2);
	const auto u3 { T::factory({{scheme, "mailto"}, {path, "John.Smith@example.com"}}) };
	run_test_comp(15, u3);
}

TEST_CASE("factory")
{
	do_factory<uri>();
}

TEST_CASE("factory (static)")
{
	do_factory<uri_static<>>();
}

//-----------------------------------------------------------------------------------------
template<typename T>
void do_edit()
{
	T u1 { "https://dakka@www.blah.com:3000/" };
	u1.edit({{port, "80"}, {user, ""}, {path, "/newpath"}});
	REQUIRE(u1.get_uri() == "https://www.blah.com:80/newpath");

	T u2 { "file:///foo/bar/test/node.js" };
	u2.edit({{scheme, "mms"}, {fragment, "bookmark1"}});
	REQUIRE(u2.get_uri() == "mms:///foo/bar/test/node.js#bookmark1");

	T u3 { "https://user:password@example.com/?search=1" };
	u3.edit({{port, "80"}, {user, "dakka"}, {password, ""}, {path, "/newpath"}});
	REQUIRE(u3.get_uri() == "https://dakka@example.com:80/newpath?search=1");
}

TEST_CASE("edit")
{
	do_edit<uri>();
}

TEST_CASE("edit (static)")
{
	do_edit<uri_static<>>();
}

