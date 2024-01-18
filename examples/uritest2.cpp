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
#include <iostream>
#include <vector>
#include <set>
#include <string_view>
#include <fix8/uri.hpp>

//-----------------------------------------------------------------------------------------
using namespace FIX8;
using namespace std::literals;

//-----------------------------------------------------------------------------------------
int main(void)
{
	//enum uri_components : unsigned { scheme, authority, userinfo, host, port, path, query, fragment, count };
	using enum uri::uri_components;
	static const std::vector<std::tuple<std::string_view, std::vector<std::tuple<uri::uri_components, std::string_view>>>> tests
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

	try
	{
		int testnum{};
		std::set<int> errors;
		for (const auto& [src,vec] : tests)
		{
			++testnum;
			const uri t1{src};
			if (t1.countof() != vec.size())
			{
				std::cerr << "test(" << testnum << "): component count error: " << t1.countof() << " != " << vec.size() << '\n';
				errors.insert(testnum);
			}
			for (const auto& [comp,str] : vec)
			{
				if (t1.get_component(comp) != str)
				{
					std::cerr << "test(" << testnum << "): component mismatch(" << t1.get_name(comp) << "): " << t1.get_component(comp) << " != " << str << '\n';
					errors.insert(testnum);
				}
			}
		}

		for (auto ii : errors)
			std::cerr << "error in test " << ii << '\n' << uri(std::get<0>(tests[ii - 1])) << '\n';
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception: " << e.what() << '\n';
	}
	return 0;
}

