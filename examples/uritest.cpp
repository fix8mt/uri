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
#include <stdexcept>
#include <memory>
#include <string_view>
#include <bitset>
#include <any>
#include <getopt.h>
#include <fix8/uri.hpp>

//-----------------------------------------------------------------------------------------
using namespace FIX8;

//-----------------------------------------------------------------------------------------
#include "uriexamples.hpp"

//-----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   static constexpr const char *optstr{"t:T:d:hlasx"};
	static constexpr const auto long_options { std::to_array<option>
   ({
      { "help",	no_argument,			nullptr, 'h' },
      { "list",	no_argument,			nullptr, 'l' },
      { "sizes",	no_argument,			nullptr, '2' },
      { "all",		no_argument,			nullptr, 'a' },
      { "dump",	required_argument,	nullptr, 'd' },
      { "test",	required_argument,	nullptr, 't' },
      { "stat",	required_argument,	nullptr, 'T' },
      {}
	})};

	int val;
	try
	{
		for (; (val = getopt_long (argc, argv, optstr, long_options.data(), 0)) != -1; )
		{
			switch (val)
			{
			case ':': case '?':
				std::cout << '\n';
				[[fallthrough]];
			case 'h':
				std::cout << "Usage: " << argv[0] << " [uri...] [-" << optstr << "]" << R"(
 -a run all tests
 -d [uri] parse uri from CLI, show debug output
 -h help
 -l list tests
 -s show sizes
 -T [num] static test to run
 -t [num] test to run)" << '\n';
				return 1;
			case 'x':
				{
					/*
					using namespace std::literals;
					constexpr auto svs { "https://www.hello.com/au/page1"sv };
					constexpr auto svss { svs.size() };
					const uri_static<svss> u1{svs};
					std::cout << u1 << '\n' << u1.max_storage() << '\n' << svss << '\n';
					for (uri::component ii{}; ii != uri::countof; ii = uri::component(ii + 1))
					{
						if (u1.test(ii))
						{
							const auto [pos,len] { u1[ii] };
							std::cout << uri::get_name(ii) << ' ' << pos << " (" << len << ")\n";
						}
					}
					*/

					//const uri_fixed<"https://www.hello.com/au/page1"> u1;
					/*
					static constexpr uri_fixed<"https://user:password@example.com/path?search=1"> u1;
					static constexpr uri_fixed<"https://hello.com/path?search=1"> u2;
					std::cout << u1 << '\n';
					std::cout << u1.max_storage() << '\n';
					std::cout << sizeof(u1) << '\n';
					std::cout << u2 << '\n';
					std::cout << u2.max_storage() << '\n';
					std::cout << sizeof(u2) << '\n';
					uri_fixed<"telnet://192.0.2.16:80/"> u3;
					std::cout << u3 << '\n';
					std::cout << "max storage: " << u3.max_storage() << '\n';
					std::cout << "total object size: " << sizeof(u3) << '\n';
					*/
					/*
					static constexpr const std::array uris
					{
						"https://www.blah.com:3000/test",
						"https://dakka@www.staylate.net:3000/",
						"https://www.buyexample.com/over/there?name=ferret&time=any#afrag",
					};
					for (const auto& pp : uris)
						std::cout << basic_uri(pp).get_component(host) << '\n';
						*/
					//static constexpr uri_fixed<"telnet://user:password@192.0.2.16:80/"> u1;
					/*
					static const std::array fixes { std::to_array<std::any>
					({
					 	{ uri_fixed<"telnet://user:password@192.0.2.16:80/">() },
						{ uri_fixed<"https://www.blah.com:3000/test">() },
						{ uri_fixed<"https://dakka@www.blah.com:3000/">() },
						{ uri_fixed<"https://example.com/over/there?name=ferret&time=any#afrag">() },
					})};
					auto ptr { std::make_unique<uri_fixed<"telnet://user:password@192.0.2.16:80/">>() };
					std::cout << sizeof(uri_fixed<"telnet://user:password@192.0.2.16:80/">) << '\n';
					std::cout << *ptr << '\n';
					*/
				}
				break;
			case 'l':
				for (int ii{}; const auto& [src,vec] : tests)
					std::cout << ii++ << '\t' << src << " (" << std::string_view(src).size() << ")\n";
				break;
			case 'T':
				if (const auto tnum {std::stoul(optarg)}; tnum >= tests.size())
					throw std::range_error("invalid test case");
				else
					std::cout << uri_static<1024>{tests[tnum].first};
				break;
			case 't':
				if (const auto tnum {std::stoul(optarg)}; tnum >= tests.size())
					throw std::range_error("invalid test case");
				else
					std::cout << uri{tests[tnum].first};
				break;
			case 'd':
				{
					const uri u1{optarg};
					if (!u1)
						std::cout << "error " << static_cast<int>(u1.get_error()) << '\n';
					std::cout << u1 << '\n' << "bitset " << std::bitset<uri::countof>(u1.get_present()) << " ("
						<< std::hex << std::showbase << u1.get_present() << std::dec << std::noshowbase << ")\n";
					for (uri::component ii{}; ii != uri::countof; ii = uri::component(ii + 1))
					{
						if (u1.test(ii))
						{
							const auto [pos,len] { u1[ii] };
							std::cout << uri::get_name(ii) << ' ' << pos << " (" << len << ")\n";
						}
					}
				}
				break;
			case 's':
				std::cout << "uri: " << sizeof(uri) << "\nbasic_uri: " << sizeof(basic_uri) << '\n';
				std::cout << "uri_static<1024>: " << sizeof(uri_static<1024>) << '\n';
				break;
			case 'a':
				for (int ii{}; const auto& [src,vec] : tests)
					std::cout << ii++ << '\n' << uri{src} << '\n';
				std::cout << tests.size() << " test cases\n";
				break;
			default:
				break;
			}
		}
		while(optind < argc)
			std::cout << uri{argv[optind++]} << '\n';
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception: " << e.what();
		if (optarg)
			std::cerr << " (-" << static_cast<char>(val) << ' ' << optarg << ')';
		std::cerr << std::endl;
		return 1;
	}
	return 0;
}

