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
using enum uri::component;

//-----------------------------------------------------------------------------------------
#include "uriexamples.hpp"

//-----------------------------------------------------------------------------------------
// run as: ctest --output-on-failure
//-----------------------------------------------------------------------------------------
TEST_CASE("uri - get component", "[uri]")
{
	const uri u1{tests[0].first};
	REQUIRE_NOTHROW(u1.get_component(host));
	REQUIRE(u1.get_component(host) == "www.blah.com");
	REQUIRE_THROWS(u1.get_component(countof));
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - get name", "[uri]")
{
	REQUIRE_NOTHROW(uri::get_name(host));
	REQUIRE(uri::get_name(host) == "host");
	REQUIRE_THROWS(uri::get_name(countof));
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - uri component validations", "[uri]")
{
	for (int ii{}; const auto& [src,vec] : tests)
	{
		INFO("uri: " << ii++);
		const uri u1{src};
		REQUIRE (u1.count() == vec.size());
		for (const auto& [comp,str] : vec)
		{
			INFO("component: " << comp);
			REQUIRE (u1.get_component(comp) == str);
		}
	}
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - get named pair", "[uri]")
{
	const uri u1{tests[0].first};
	REQUIRE_NOTHROW(u1.get_named_pair(host));
	REQUIRE_THROWS(u1.get_named_pair(countof));
	const auto [n1,n2] { u1.get_named_pair(host) };
	REQUIRE(n1 == "host");
	REQUIRE(n2 == "www.blah.com");
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - replace", "[uri]")
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
TEST_CASE("uri - storage", "[uri]")
{
	const auto& [src,vec] { tests[0] };
	const uri u1{src};
	REQUIRE(src == u1.get_source());
	REQUIRE(u1.get_buffer() == u1.get_source());
	REQUIRE(u1.get_buffer() == src);
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - invalid uri", "[uri]")
{
	REQUIRE_THROWS(uri("https://www.example.com\n"));
	REQUIRE_THROWS(uri("https://www.example.com\r"));
	REQUIRE_THROWS(uri("https://www. example.com"));
	REQUIRE_THROWS(uri("https://www.example\tcom"));
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - limits", "[uri]")
{
	char buff[UINT16_MAX+1]{};
	std::fill(buff, buff + sizeof(buff), 'x');
	REQUIRE_THROWS(uri(buff));
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - hex decode", "[uri]")
{
	std::string_view src { "https://www.netmeister.org/%62%6C%6F%67/%75%72%6C%73.%68%74%6D%6C?!@#$%25=+_)(*&^#top%3C" },
		src1 { "https://www.netmeister.org/blog/urls.html?!@#$%=+_)(*&^#top<" },
		src2 { "https://www.netmeister.org/path#top%3" };
	REQUIRE(uri::has_hex(src));
	REQUIRE(!uri::has_hex(src1));
	REQUIRE(!uri::has_hex(src2));
	auto result { uri::decode_hex(src) };
	uri u1{result};
	REQUIRE(u1.get_source() == src1);
	uri u2(std::string(src), false);
	REQUIRE(uri::has_hex(u2.get_source()));
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - query decode", "[uri]")
{
	static const std::vector<std::pair<std::string_view,std::string_view>> tbl
	{
		{ "payload1", "true" }, { "payload2", "false" }, { "test", "1" }, { "benchmark", "3" }, { "foo", "38.38.011.293" },
		{ "bar", "1234834910480" }, { "test", "19299" }, { "3992", "" }, { "key", "f5c65e1e98fe07e648249ad41e1cfdb0" },
	};
	const uri u1{tests[9].first};
	auto result{u1.decode_query()};
	REQUIRE(tbl == result);
	const uri u2{tests[8].first};
	auto result1{u2.decode_query()};
	REQUIRE(result1.empty());
}

