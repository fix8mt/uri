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
const std::vector<std::pair<const char *, std::vector<std::pair<uri::component, const char *>>>> tests
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
			{ user, "dakka" },
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
	{ "https://user:password@example.com/path?search=1",
		{
			{ scheme, "https" },
			{ authority, "user:password@example.com" },
			{ host, "example.com" },
			{ user, "user" },
			{ password, "password" },
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
			{ authority, "你" },
			{ host, "你" },
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
	{ "tel:+1-816-555-1212",
		{
			{ scheme, "tel" },
			{ path, "+1-816-555-1212" },
		}
	},
	{ "telnet://192.0.2.16:80/",
		{
			{ scheme, "telnet" },
			{ authority, "192.0.2.16:80" },
			{ host, "192.0.2.16" },
			{ port, "80" },
			{ path, "/" },
		}
	},
	{ "http://-.~_!$&'()*+,;=:%40:80%2f::::::@example.com",
		{
			{ scheme, "http" },
			{ authority, "-.~_!$&'()*+,;=:@:80" },
			{ user, "-.~_!$&'()*+,;=:" },
			{ port, "80" },
			{ path, "/::::::@example.com" },
		}
	},
	{ "http://foo.com/blah_blah_(wikipedia)_(again)",
		{
			{ scheme, "http" },
			{ authority, "foo.com" },
			{ host, "foo.com" },
			{ path, "/blah_blah_(wikipedia)_(again)" },
		}
	},
	{ "http://उदाहरण.परीक्षा",
		{
			{ scheme, "http" },
			{ authority, "उदाहरण.परीक्षा" },
			{ host, "उदाहरण.परीक्षा" },
			{ path, "" }, // empty path
		}
	},
	{ "http://foo.com/(something)?after=parens",
		{
			{ scheme, "http" },
			{ authority, "foo.com" },
			{ host, "foo.com" },
			{ path, "/(something)" },
			{ query, "after=parens" },
		}
	},
	{ "http://foo.com/unicode_(✪)_in_parens",
		{
			{ scheme, "http" },
			{ authority, "foo.com" },
			{ host, "foo.com" },
			{ path, "/unicode_(✪)_in_parens" },
		}
	},
	{ "http://➡.ws/䨹",
		{
			{ scheme, "http" },
			{ authority, "➡.ws" },
			{ host, "➡.ws" },
			{ path, "/䨹" },
		}
	},
	{ "epgm://127.0.0.1;224.0.0.0:11042",
		{
			{ scheme, "epgm" },
			{ authority, "127.0.0.1;224.0.0.0:11042" },
			{ host, "127.0.0.1;224.0.0.0" },
			{ port, "11042" },
			{ path, "" }, // empty path
		}
	},
	{ "https://!$%25:)(*&^@www.netmeister.org/blog/urls.html",
		{
			{ scheme, "https" },
			{ authority, "!$%:)(*&^@www.netmeister.org" },
			{ host, "www.netmeister.org" },
			{ user, "!$%" },
			{ password, ")(*&^" },
			{ path, "/blog/urls.html" },
		}
	},
	{ "https://www.netmeister.org/t/h/e/s/e/../../../../../d/i/r/e/c/t/o/"
		"r/i/e/s/../../../../../../../../../../../d/o/../../n/o/t/../../../e/x/i/s/t/../../../../../blog/urls.html",
		{
			{ scheme, "https" },
			{ authority, "www.netmeister.org" },
			{ host, "www.netmeister.org" },
			{ path, "/t/h/e/s/e/../../../../../d/i/r/e/c/t/o/r/i/e/s/../../../../../../../../../../../d/o/../../n/o/t/"
				"../../../e/x/i/s/t/../../../../../blog/urls.html" },
		}
	},
	{ "https://www.blah.com:/test",
		{
			{ scheme, "https" },
			{ authority, "www.blah.com:" },
			{ host, "www.blah.com" },
			{ path, "/test" },
		}
	},
	{ "https://www.netmeister.org/%62%6C%6F%67/%75%72%6C%73.%68%74%6D%6C?!@#$%25=+_)(*&^#top%3C",
		{
			{ scheme, "https" },
			{ authority, "www.netmeister.org" },
			{ host, "www.netmeister.org" },
			{ path, "/blog/urls.html" },
			{ query, "!@" },
			{ fragment, "$%=+_)(*&^#top<" },
		}
	},
	{ "https://en.wikipedia.org/wiki/C%2B%2B20",
		{
			{ scheme, "https" },
			{ authority, "en.wikipedia.org" },
			{ host, "en.wikipedia.org" },
			{ path, "/wiki/C++20" },
		}
	},
};

