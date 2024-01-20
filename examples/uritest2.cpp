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
#include <set>
#include <string_view>
#include <fix8/uri.hpp>

//-----------------------------------------------------------------------------------------
using namespace FIX8;
using namespace std::literals;
using enum uri::component;

//-----------------------------------------------------------------------------------------
//enum component : unsigned { scheme, authority, userinfo, host, port, path, query, fragment, countof };
const std::vector<std::tuple<std::string_view, std::vector<std::tuple<uri::component, std::string_view>>>> tests
{
	{ "https://www.blah.com/",
		{
			{ scheme, "https" },
			{ authority, "www.blah.com" },
			{ host, "www.blah.com" },
			{ path, "/" },
		}
	},
	{ "https://www.blah.com",
		{
			{ scheme, "https" },
			{ authority, "www.blah.com" },
			{ host, "www.blah.com" },
			{ path, "" }, // empty path
		}
	},
	{ "https://www.blah.com:3000/test",
		{
			{ scheme, "https" },
			{ authority, "www.blah.com:3000" },
			{ host, "www.blah.com" },
			{ port, "3000" },
			{ path, "/test" },
		}
	},
	{ "https://dakka@www.blah.com:3000/",
		{
			{ scheme, "https" },
			{ authority, "dakka@www.blah.com:3000" },
			{ userinfo, "dakka" },
			{ host, "www.blah.com" },
			{ port, "3000" },
			{ path, "/" },
		}
	},
	{ "https://example.com/over/there?name=ferret&time=any#afrag",
		{
			{ scheme, "https" },
			{ authority, "example.com" },
			{ host, "example.com" },
			{ path, "/over/there" },
			{ query, "name=ferret&time=any" },
			{ fragment, "afrag" },
		}
	},
	{ "https://example.org/./a/../b/./c",
		{
			{ scheme, "https" },
			{ authority, "example.org" },
			{ host, "example.org" },
			{ path, "/./a/../b/./c" },
		}
	},
	{ "ws://localhost:9229/f46db715-70df-43ad-a359-7f9949f39868",
		{
			{ scheme, "ws" },
			{ authority, "localhost:9229" },
			{ host, "localhost" },
			{ port, "9229" },
			{ path, "/f46db715-70df-43ad-a359-7f9949f39868" },
		}
	},
	{ "ldap://[2001:db8::7]/c=GB?objectClass?one",
		{
			{ scheme, "ldap" },
			{ authority, "[2001:db8::7]" },
			{ host, "[2001:db8::7]" },
			{ path, "/c=GB" },
			{ query, "objectClass?one" },
		}
	},
	{ "file:///foo/bar/test/node.js",
		{
			{ scheme, "file" },
			{ authority, "" }, // empty authority
			{ path, "/foo/bar/test/node.js" },
		}
	},
	{	"http://nodejs.org:89/docs/latest/api/foo/bar/qua/13949281/0f28b/5d49/b3020/url.html"
		"?payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293"
		"&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0#test",
		{
			{ scheme, "http" },
			{ authority, "nodejs.org:89" },
			{ host, "nodejs.org" },
			{ port, "89" },
			{ path, "/docs/latest/api/foo/bar/qua/13949281/0f28b/5d49/b3020/url.html" },
			{ query, "payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293"
						"&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0" },
			{ fragment, "test" },
		}
	},
	{ "https://user:pass@example.com/path?search=1",
		{
			{ scheme, "https" },
			{ authority, "user:pass@example.com" },
			{ host, "example.com" },
			{ userinfo, "user:pass" },
			{ path, "/path" },
			{ query, "search=1" },
		}
	},
	{ "javascript:alert(\"nodeisawesome\");",
		{
			{ scheme, "javascript" },
			{ path, "alert(\"nodeisawesome\");" },
		}
	},
	{ "https://%E4%BD%A0/foo",
		{
			{ scheme, "https" },
			{ authority, "%E4%BD%A0" },
			{ host, "%E4%BD%A0" },
			{ path, "/foo" },
		}
	},
	{ "http://你好你好.在",
		{
			{ scheme, "http" },
			{ authority, "你好你好.在" },
			{ host, "你好你好.在" },
			{ path, "" }, // empty path
		}
	},
	{ "urn:oasis:names:specification:docbook:dtd:xml",
		{
			{ scheme, "urn" },
			{ path, "oasis:names:specification:docbook:dtd:xml" },
		}
	},
	{ "mailto:John.Smith@example.com",
		{
			{ scheme, "mailto" },
			{ path, "John.Smith@example.com" },
		}
	},
	{ "news:comp.infosystems.www.servers.unix",
		{
			{ scheme, "news" },
			{ path, "comp.infosystems.www.servers.unix" },
		}
	},
};

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - get component", "[uri]")
{
	const uri u1{std::get<0>(tests[0])};
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
TEST_CASE("uri - various permutations", "[uri]")
{
	for (const auto& [src,vec] : tests)
	{
		const uri u1{src};
		REQUIRE (u1.count() == vec.size());
		for (const auto& [comp,str] : vec)
			REQUIRE (u1.get_component(comp) == str);
	}
}

//-----------------------------------------------------------------------------------------
TEST_CASE("uri - get named pair", "[uri]")
{
	const uri u1{std::get<0>(tests[0])};
	REQUIRE_NOTHROW(u1.get_named_pair(host));
	REQUIRE_THROWS(u1.get_named_pair(countof));
	const auto [n1,n2] { u1.get_named_pair(host) };
	REQUIRE(n1 == "host");
	REQUIRE(n2 == "www.blah.com");
}

