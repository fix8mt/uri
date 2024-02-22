<p align="center">
  <a href="https://www.fix8mt.com"><img src="https://github.com/fix8mt/uri/blob/master/assets/fix8mt_Master_Logo_Green_Trans.png" width="200"></a>
</p>

# uri

### A lightweight C++20 URI parser

# Quick links
|**Link**|**Description**|
--|--
|[Here](https://github.com/fix8mt/uri/blob/master/include/fix8/uri.hpp)| For implementation|
|[Examples](#2-examples)| For examples|
|[Building](#3-building)| How to build or include|
|[API](#4-api)| API details |
|[Testing](#testing)| Testing apps|
|[Benchmark](#benchmarks)| Benchmark info|
|[Discussion](#6-discussion)| Discussion |

------------------------------------------------------------------------
# 1. Introduction
This is a lightweight URI parser implementation featuring zero-copy, minimal storage and high performance.

## Motivation
- header-only
- zero-copy where possible (base class uses references only)
- no external dependencies
- simplicity, lightweight
- make use of C++20 features
- entirely `constexpr`
- high performance
- non-validating

## Features
- single _header-only_
- fast, very lightweight, predictive non brute force parser: avg 53ns to decode a URI with `basic_uri`
- base class is zero-copy, using `std::string_view`
- derived class moves (or copies) source string once
- all methods `constexpr`; no virtual methods
- extracts all components `scheme`, `authority`, `userinfo`, `user`, `password`, `host`, `port`, `path`, `query`, `fragment`
- query components returned as `std::string_view`
- query decode and search - no copying, results point to uri source
- small memory footprint - base class object is only 64 bytes
- support for dynamic or static uri storage
- built-in unit test cases with exhaustive test URI cases; simple test case addition
- support for [**RFC 3986**](https://datatracker.ietf.org/doc/html/rfc3986)
- cmake integration with FetchContent

# 2. Examples
## i. Use `basic_uri` as a view
This example parses a list of URI strings and prints out `host` component. `basic_uri` creates a no-copy view of the source.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <array>
#include <fix8/uri.hpp>
using namespace FIX8;

int main(int argc, char *argv[])
{
   static constexpr const std::array uris
   {
      "https://www.blah.com:3000/test",
      "https://dakka@www.staylate.net:3000/",
      "https://www.buyexample.com/over/there?name=ferret&time=any#afrag",
   };
   for (const auto& pp : uris)
      std::cout << basic_uri(pp).get_component(uri::host) << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```CSV
$ ./example1
www.blah.com
www.staylate.net
www.buyexample.com
$
```

</p>
</details>

## ii. Parse and hold a URI
This example parses a URI string and prints out all the contained elements. Then individual components are queried and printed if present.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;

int main(int argc, char *argv[])
{
   const uri u1 {"http://nodejs.org:89/docs/latest/api/foo/bar/qua/13949281/0f28b/5d49/b3020/url.html"
      "?payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293"
      "&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0#test"};

   std::cout << u1 << '\n';

   std::cout << u1.get_component(uri::authority) << '\n'
      << u1.get_component(uri::host) << '\n'
      << u1.get_component(uri::port) << '\n'
      << u1.get_component(uri::query) << '\n'
      << u1.get_component(uri::fragment) << '\n';
   if (u1.test(uri::user)) // should be no user
      std::cout << u1.get_component(uri::user) << '\n';
   auto result{u1.decode_query(true)}; // sort result
   std::cout << "key = " << uri::find_query("key", result) << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```CSV
$ ./example2
uri         http://nodejs.org:89/docs/latest/api/foo/bar/qua/13949281/0f28b/5d49/b3020/url.html?payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0#test
scheme      http
authority   nodejs.org:89
host        nodejs.org
port        89
path        /docs/latest/api/foo/bar/qua/13949281/0f28b/5d49/b3020/url.html
query       payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0
   payload1    true
   payload2    false
   test        1
   benchmark   3
   foo         38.38.011.293
   bar         1234834910480
   test        19299
   3992        (empty)
   key         f5c65e1e98fe07e648249ad41e1cfdb0
fragment    test

nodejs.org:89
nodejs.org
89
payload1=true&payload2=false&test=1&benchmark=3&foo=38.38.011.293&bar=1234834910480&test=19299&3992&key=f5c65e1e98fe07e648249ad41e1cfdb0
test
key = f5c65e1e98fe07e648249ad41e1cfdb0
$
```

</p>
</details>

## iii. Create a static uri
Create a static URI with a maximum storage of 256 bytes, from the supplied string. Print out the result and max storage.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;

int main(int argc, char *argv[])
{
   uri_static<256> u1{"mailto:John.Smith@example.com"};
   std::cout << u1 << '\n';
   std::cout << "max storage: " << uri_static<256>::max_storage() << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```CSV
$ ./example3
uri         mailto:John.Smith@example.com
scheme      mailto
path        John.Smith@example.com
max storage: 256
$
```

</p>
</details>

## iv. Use the factory
Create a URI from an initializer list. Print out the result.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;
using enum uri::component;

int main(int argc, char *argv[])
{
   const auto u1 { uri::factory({{scheme, "https"}, {user, "dakka"},
      {host, "www.blah.com"}, {port, "3000"}, {path, "/"}}) };
   std::cout << u1 << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```CSV
$ ./example4
uri         https://dakka@www.blah.com:3000/
scheme      https
authority   dakka@www.blah.com:3000
userinfo    dakka
user        dakka
host        www.blah.com
port        3000
path        /
$
```

</p>
</details>

## v. Edit a URI
Create a URI and then edit it.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;
using enum uri::component;

int main(int argc, char *argv[])
{
   uri u1 { "https://dakka@www.blah.com:3000" };
   std::cout << u1 << '\n';
   u1.edit({{port, "80"}, {user, ""}, {path, "/newpath"}});
   std::cout << '\n' << u1 << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```CSV
$ ./example5
uri         https://dakka@www.blah.com:3000
scheme      https
authority   dakka@www.blah.com:3000
userinfo    dakka
user        dakka
host        www.blah.com
port        3000
path        (empty)

uri         https://www.blah.com:80/newpath
scheme      https
authority   www.blah.com:80
host        www.blah.com
port        80
path        /newpath
$
```

</p>
</details>

# 3. Building
This implementation is header only. Apart from standard C++20 includes there are no external dependencies needed in your application.
[Catch2](https://github.com/catchorg/Catch2.git) is used for the built-in unit tests. [Criterion](https://github.com/p-ranav/criterion) is used for benchmarking.

## i. Obtaining the source, building the examples
To clone and default build all the examples, including the unit tests.
```bash
git clone git@github.com:fix8mt/uri.git
cd uri
mkdir build
cd build
cmake ..
make -j4
make test (or ctest)
```
## ii. Using in your application with cmake
In `CMakeLists.txt` set your include path to:
```cmake
include_directories([uri directory]/include)
# e.g.
set(uridir /home/dd/prog/uri)
include_directories(${uridir}/include)
```
and just include:
```c++
#include <fix8/uri.hpp>
```
in your application. Everything in this class is within the namespace `FIX8`, so you can add:
```c++
using namespace FIX8;
```

This is C++20, so you can also add in your local code:
```c++
using enum uri::component;
```

## iii. Integrating uri in your project with cmake FetchContent
You can use cmake [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) to integrate uri with your project.
If your project was called `myproj` with the sourcefile `myproj.cpp` then...
```cmake
project(myproj)
add_executable (myproj myproj.cpp)
set_target_properties(myproj PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED true)
message(STATUS "Downloading uri...")
include(FetchContent)
FetchContent_Declare(uri GIT_REPOSITORY https://github.com/fix8mt/uri.git)
FetchContent_MakeAvailable(uri)
target_include_directories(myproj PRIVATE ${uri_SOURCE_DIR}/include)
```

# 4. API
## i. Class hierarchy
### `basic_uri`
The base class `basic_uri` performs the bulk of the work, holding a `std::string_view` of the source uri string. If you wish to manage the scope of the source uri yourself then
this class is the most efficient way to use uri functionality.

```c++
basic_uri u1{"https://www.example.com:8080/path1"};
```

***
### `uri`
The derived class `uri` stores the source string and then builds a `basic_uri` using that string as its reference. `uri` derives from `basic_uri` and a private **dynamic** storage class
`uri_storage`. The supplied string is moved or copied and stored by the object. If your application needs the uri to hold and persist the source uri, this class is suitable.
The storage class used is a specialisation of `uri_storage` which specifies `0` as the non-type parameter `sz`, selecting dynamic storage.

```c++
std::string myuri;
.
.
.
uri u1{myuri};
```

![class diagram](https://github.com/fix8mt/uri/blob/master/assets/classdynamic.png)

***
### `uri_static`
The derived class `uri_static` stores the source string and then builds a `basic_uri` using that string as its reference. `uri_static` derives from `basic_uri` and a private **static** storage class
`uri_storage`. The supplied string is moved or copied and stored by the object. The class is templated by the non-type parameter `sz` which sets the static size and maximum storage capacity
for the uri. `sz` defaults to `1024`. Storage is allocated once with the object in a `std::array`. No dynamic memory is used.
If your application needs the uri to hold and persist the source uri statically (for example in another container), this class is suitable.

```c++
std::string myuri;
.
.
.
uri_static<256> u1{myuri};
```

![class diagram (static)](https://github.com/fix8mt/uri/blob/master/assets/classstatic.png)

## ii. Types
### component
```c++
enum component { scheme, authority, userinfo, user, password, host, port, path, query, fragment, countof };
```
Components are named by a public enum called `component`.  Note that the component `user` and `password` are populated if present and
`userinfo` will also be populated.

### other types
| Type | Typedef of |Description |
| --- | --- | --- |
| `uri_len_t`  | `std::uint16_t` | the integral type used to store offsets and lengths|
| `value_pair`  | `std::pair<std::string_view,std::string_view>` |used to return tag value pairs|
| `query_result`  | `std::vector<value_pair>` |used to return a collection of query pairs|
| `range_pair`  | `std::pair<uri_len_t,uri_len_t>` |used to store offset and length |
| `comp_pair` | `std::pair<component, std::string_view>`|used by `factory` to pass individual `component` pairs|
| `comp_list` | `std::vector<std::string_view>`|used by `factory`,`edit` and `make_source` to pass individual `component` values; each position in the vector corresponds to the component index|
| `error` | `enum class error : uri_len_t { no_error, too_long, illegal_chars, empty_src, countof };`|error types|

### consts
| Const | Description |
| ------------- | ------------- |
| `uri_max_len`  | the maximum length of a supplied uri|

## iii. Construction and destruction
### ctor
```c++
class basic_uri;
constexpr basic_uri(std::string_view src);                           // (1)
constexpr basic_uri(int bits);                                       // (2)
constexpr basic_uri();                                               // (3)

class uri;
constexpr uri(std::string src);                                      // (4)
constexpr uri(std::string_view src);                                 // (5)
constexpr uri(const char *src);                                      // (6)
constexpr uri();                                                     // (7)

template<size_t sz>
class uri_static;
constexpr uri_static(std::string src);                               // (8)
constexpr uri_static(std::string_view src);                          // (9)
constexpr uri_static(const char *src);                               // (10)
constexpr uri_static();                                              // (11)
```

1. Construct a `basic_uri` from a `std::string_view`. This base class does not store the string. Calls `parse()`. The source string must not go out of scope to use this object. If parsing
fails, you can check for error using `operator bool` or `count()` and then `get_error()` for more info. Since this method takes a `std::string_view` you can declare objects `constexpr`. Note
that `std::string` contains a convert to `std::string_view` operator.
1. Construct a `basic_uri` that has the corresponding bitset passed in `bits`. No components are present. Permits object to be used as a component bitset.
1. Construct an empty `basic_uri`. It can be populated using `assign()`.
1. Construct a `uri` from a `std::string`. The source string is percent decoded before parsing. Calls `parse()`.
The supplied string is moved or copied and stored by the object. You can check for error using `operator bool` or `count()` and then `get_error()` for more info.
1. Construct a `uri` from a `std::string_view`. Creates a `std::string` from src and delegates to (4).
1. Construct a `uri` from a `null` terminated `const char *`. Creates a `std::string` from src and delegates to (4).
1. Construct an empty `uri`. It can be populated using `replace()`.
1. Construct a `uri_static` from a `std::string`. The class is templated by the non-type parameter `sz` which sets the static size and maximum storage capacity
for the uri. The source string is percent decoded before parsing. Calls `parse()`.
1. Construct a `uri_static` from a `std::string_view`. Creates a `std::string` from src and delegates to (8).
1. Construct a `uri_static` from a `null` terminated `const char *`. Creates a `std::string` from src and delegates to (8).
1. Construct an empty `uri_static` from a `std::string`. The class is templated by the non-type parameter `sz` which sets the static size and maximum storage capacity for the uri.

All of `uri` is within the namespace **`FIX8`**.

### dtor
```c++
~basic_uri();
~uri();
```

Destroy the `uri` or `basic_uri`. The `uri` object will release the stored string.

## iv. Accessors
### `test`
```c++
constexpr bool uri::test(uri::component what=countof) const;
```
Return `true` if the specified component is present in the uri. With no parameter (default) returns `true` if any component is present.

### `get_component`
```c++
constexpr std::string_view get_component(component what) const;
```
Return a `std::string_view` of the specified component or empty if component not found. Returns an empty `std::string_view` if not found or not a legal component.

### `get_present`
```c++
constexpr uri_len_t get_present() const;
```
Return the present bitset as `uri_len_t` which has bits set corresponding to the component's enum position.

### `get`
```c++
constexpr std::string_view get(component what) const;
```
Return a `std::string_view` of the specified component.
> [!WARNING]
> This is _not_ range checked.

### `operator bool`
```c++
constexpr operator bool() const;
```
Returns true if parsing was successful, false on fail.

### `get_error`
```c++
constexpr error get_error() const;
```
Return the last `uri::error` error enum. If no error returns `error::no_error`. Use it to obtain the reason a uri failed to parse.

### `const operator[component]`
```c++
constexpr const range_pair& operator[](component idx) const;
```
Return a `const range_pair&` which is a `std::pair<uri_len_t, uri_len_t>&` to the specified component at the index given in the ranges table. This provides read-only
access to the offset and length of the specified component and is used to create a `std::string_view`. No copying, results point to uri source.
> [!WARNING]
> This is _not_ range checked.

### `decode_query`
```c++
template<char separator='&',char tagequ='='>
constexpr query_result decode_query(bool sort=false) const;
```
Returns a `std::vector` of pairs of `std::string_view` of the query component if present.  You can optionally override the value pair separator character using
the first non-type template parameter - some queries use `;`. You can also optionally override the value equality separator character using the second non-type
template parameter (some queries use `:`). Pass `true` to optionally sort the `query_result` lexicographically by the key. No copying, results point to uri source.
Returns an empty vector if no query was found. The query is assumed to be in the form:
```
&tag=value[&tag=value...]
```
Or if you override, say
```
;tag:value[;tag:value...]
```
If no value is present, just the tag will be populated with an empty value.

### `find_query`
```c++
static constexpr std::string_view find_query (std::string_view what, const query_result& from);
```
Find the specified query key and return its value from the given `query_result`. `query_result` must be sorted by key, as returned by
passing `true` to `decode_query`. If not sorted, use `sort_query` first. If key not found return empty `std::string_view`. No copying, results point to uri source.

### `decode_hex`
```c++
static constexpr std::string decode_hex(std::string_view src);
```
Decode any hex values present in the supplied string. Hex values are only recognised if
they are in the form `%XX` where X is a hex digit (octet) `[0-9a-fA-F]`. Return in a new string.

### `has_hex`
```c++
static constexpr bool has_hex(std::string_view src);
```
Return true if any hex values are present in the supplied string. Hex values are only recognised if
they are in the form `%XX` where X is a hex digit (octet) `[0-9a-fA-F]`.

### `find_hex`
```c++
static constexpr std::string_view::size_type find_hex(std::string_view src);
```
Return the position of the first hex value (if any) in the supplied string. Hex values are only recognised if
they are in the form `%XX` where X is a hex digit (octet) `[0-9a-fA-F]`. If not found returns `std::string_view::npos`.

### `get_name`
```c++
static constexpr std::string_view get_name(component what);
```
Return a `std::string_view` of the specified component name. Returns an empty `std::string_view` if not found or not a legal component.

### `get_uri`
```c++
constexpr std::string_view get_uri() const;
```
Return a `std::string_view` of the source uri. If not set return value will be empty.

### `get_named_pair`
```c++
constexpr value_pair get_named_pair(component what) const;
```
Return a `std::pair` of `std::string_view` for the specified component. The `first` will be the component name, `second` the component value. Suitable
for populating a JSON field. No copying, results point to uri source. Returns an empty `std::string_view` pair if not found or not a legal component.

### `count`
```c++
constexpr int count() const;
```
Return the count of components in the uri.

### `operator<<`
```c++
friend std::ostream& operator<<(std::ostream& os, const basic_uri& what);
```
Print the uri object to the specified stream. The source and individual components are printed. If a query is present, each tag value pair is also printed.

### `get_buffer`
```c++
constexpr const std::string& get_buffer() const;
```
Return a `const std::string&` to the stored buffer. Only available from `uri`.

### `any_authority`
```c++
constexpr bool any_authority() const;
```
Returns true if any authority components are present. This means any one of `host`, `password`, `port`, `user` or `userinfo`.


## v. Mutators
### `set`
```c++
constexpr void uri::set(uri::component what=countof);
```
Set the specified component bit as present in the uri. By default sets all bits. Use carefully.

### `clear`
```c++
constexpr void uri::clear(uri::component what=countof);
```
Clear the specified component bit in the uri. By default clears all bits. Use carefully.

### `assign`
```c++
constexpr int assign(std::string_view src);
```
Replace the current uri reference with the given reference. No storage is allocated. Return the number of components found.

### `replace`
```c++
constexpr std::string replace(std::string&& src);
```
Replace the current uri with the given string. The storage is updated with a move (or copy) of the string. The old string is returned.

### `set_error`
```c++
constexpr void set_error(error what);
```
Set the last `uri::error` error to the error given. Setting an error is destructive and renders the uri unusable.

## `operator[component]`
```c++
constexpr range_pair& operator[](component idx);
```
Return a `range_pair&` which is a `std::pair<uri_len_t, uri_len_t>&` to the specified component at the index given in the ranges table. This provides direct
access to the offset and length of the specified component and is used to create a `std::string_view`.
> [!WARNING]
> This is _not_ range checked. Allows for modification of the `string_view` range. Use carefully.

## `parse`
```c++
constexpr int parse();
```
Parse the source string into components. Return the count of components found. Will reset a uri if already parsed. You can check for error using `get_error()` for more info.

## `sort_query`
```c++
static constexpr void sort_query(query_result& query);
```
Sort the supplied query alphanumerically based on the tag in the query value pair.

## vi. Generation and editing
### `factory`
```c++
static constexpr uri::factory(std::initializer_list<comp_pair> from);
template<size_t sz>
static constexpr uri_static<sz>::factory(std::initializer_list<comp_pair> from);
```
Create a `uri` from the supplied components. The `initializer_list` contains a 1..n `comp_pair` objects. The following constraints apply:
1. If `authority` is supplied and any of the following components are present `host`, `password`, `port`, `user` or `userinfo` then `authority` is ignored;
1. If `userinfo` is supplied and any of the following components are present `user` or `password` then `userinfo` is ignored;

### `edit`
```c++
constexpr int edit(std::initializer_list<comp_pair> from);
```
Modify an existing `uri` by replacing existing components with the supplied components. Components not specified are left unchanged. The `initializer_list` contains a 1..n `comp_pair` objects. The same constraints as `factory` apply.

### `make_uri`
```c++
static constexpr std::string make_uri(std::initializer_list<comp_pair> from);
```
Construct a `std::string` representation of a `uri` from the supplied components. The `initializer_list` contains a 1..n `comp_pair` objects. The same constraints as `factory` apply.

### `make_edit`
```c++
static constexpr std::string make_edit(const auto& what, std::initializer_list<comp_pair> from);
```
Construct a `std::string` representation of a `uri` from the supplied `uri` (`what`) and components. The returned string is based on the existing `uri` with replacements of the supplied components.
Components not specified are left unchanged. The `initializer_list` contains a 1..n `comp_pair` objects. The same constraints as `factory` apply.

# 5. Testing
## Test cases
The header file `uriexamples.hpp` contains a data structure holding test cases used by the [Catch2](https://github.com/catchorg/Catch2.git) unit test app `unittests` and by the CLI test app `uritest`.
You can add your own test cases to `uriexamples.hpp` - the structure is easy enough to follow.

<details><summary><i>sample</i></summary>
<p>

```c++
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
.
.
.
```

</p>
</details>

## `unittests`
This application is run by default if you run `make test` or `ctest`. When running using `ctest` use the following command:

```bash
$ ctest --output-on-failure
```

## `uritest`
This is a simple CLI test app which allows you to run individual or all tests from `uriexamples.hpp`, or test a uri passed from the command line.

```bash
$ ./uritest -h
Usage: ./uritest [uri...] [-t:T:d:hlas]
 -a run all tests
 -d [uri] parse uri from CLI, show debug output
 -h help
 -l list tests
 -s show sizes
 -T [num] static test to run
 -t [num] test to run
$
```

You can run adhoc tests from the CLI as follows:
<details><summary><i>output</i></summary>
</p>

```CSV
$ ./uritest -d "https://user:password@example.com:3000/path?search=1&key=val&when=now#frag"
uri         https://user:password@example.com:3000/path?search=1&key=val&when=now#frag
scheme      https
authority   user:password@example.com:3000
userinfo    user:password
user        user
password    password
host        example.com
port        3000
path        /path
query       search=1&key=val&when=now
   search      1
   key         val
   when        now
fragment    frag

bitset 1111111111 (0x3ff)
scheme 0 (5)
authority 8 (30)
userinfo 8 (13)
user 8 (4)
password 13 (8)
host 22 (11)
port 34 (4)
path 38 (5)
query 44 (25)
fragment 70 (5)
$
```

</p>
</details>

## `benchmarks`
We use the [Criterion](https://github.com/p-ranav/criterion) benchmarking library. The benchmark app is built by default.
The file `basiclist.hpp` contains 1000 generic URIs. The benchmark **creates 1000** `basic_uri`, `uri` and
`uri_static` objects and measures the total time taken. We can calculate the average time to decode each URI[^1].

![Benchmarks](https://github.com/fix8mt/uri/blob/master/assets/benchmarks.png)

From the above results we can see the following average performance per URI:

| Class | Decode(ns) |
| --- | --- |
| `basic_uri`  | **53** _ns_ |
| `uri`  | **94** _ns_ |
| `uri_static`  | **96** _ns_ |

[^1]: Ubuntu 23.10, 12 core 4.7GHz Intel i7 Cometlake Processors, 15.3GB RAM; gcc-13.2.0

# 6. Discussion
## i. Non-validating
This class is non-validating. The source URI is expected to be normalised or at least parsable. Validation is out of scope for this implementation.
We decided against validating for a few reasons:
1. Performance - validating is expensive; most URIs are generally parsable
1. Complex - validation rules are complicated; for most use cases, simple rejection for gross rule violation is sufficient.
See [URL Standard](https://url.spec.whatwg.org/) for complete validation rules.

## ii. Low level access
There are two methods that provide unchecked direct access to the `range` table and `component`. You must ensure that you don't pass an invalid component
index when using these. Making changes to the range object with `operator[]` can have serious consequences. Use carefully.
1. `constexpr std::string_view get(component what) const;`
1. `constexpr range_pair& operator[](component idx)`;

## iii. Sanity checking
This class will perform basic sanity checks on the source URI and refuses to continue parsing. You can test for failure using the `operator bool`. These are:
1. Length - source must not exceed `uri_max_len` (`UINT16_MAX`)
1. Illegal chars - source must not contain any whitespace characters

## iv. Performance
This class performs well, with minimal latency. Since there is no copying of strings or sub-strings, the decoding functionality in `basic_uri` uses minimal cycles
\- especially for applications that can manage the storage of the source string themselves. The memory footprint of `basic_uri` is 64 bytes and will fit in a cache-line.

- If storage of the source is needed, `uri` performs a single string copy (or move), and aside from that will have the same performance as `basic_uri`.

- If you need to store the source URI but wish to avoid using dynamic memory, use `uri_static` (for example for including in another container).
This ensures a single allocation for the entire object. For most purposes
(and excluding edits) a statically stored URI is the most efficient storage option. Be aware that the template parameter `sz`
must be large enough for any URI you wish to store and of course objects created with different templated sizes will be different types.

- The `factory` and `edit` have more copying although even these still use `std::string_view` where possible with actual copying of strings or sub-strings occurring
once at most.

- With all methods `constexpr` and `noexcept`, no `virtual` methods and header only your compiler should be able to optimise your code most efficiently.

- If you want to reduce the size of `basic_uri` further, you can change:
```c++
using uri_len_t = std::uint16_t;
```
to:
```c++
using uri_len_t = std::uint8_t;
```
This will limit the maximum length of a URI to 256 bytes, but reduce the overall storage needed for `basic_uri` from `64` to `40` bytes.
