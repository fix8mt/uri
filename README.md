<p align="center">
  <a href="https://www.fix8mt.com"><img src="https://github.com/fix8mt/uri/blob/master/assets/fix8mt_Master_Logo_Green_Trans.png" width="200"></a>
</p>

# uri

### A lightweight C++20 URI parser

------------------------------------------------------------------------
## Introduction
This is a lightweight URI parser implementation featuring zero-copy, minimal storage and high performance.

## Quick links
|**Link**|**Description**|
--|--
|[Here](https://github.com/fix8mt/uri/blob/master/include/fix8/uri.hpp)| for implementation|
|[Examples](https://github.com/fix8mt/uri/blob/master/examples)| for examples and test cases|

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
- base class is zero-copy, using `std::string_view`
- derived class moves (or copies) source string once
- extracts all components `scheme`, `authority`, `user`, `password`, `host`, `port`, `path`, `query`, `fragment`
- query components, return as `std::string_view`
- fast, very lightweight, predictive non brut force parser
- small memory footprint - base class is only 56 bytes
- built-in unit testcases with exhaustive test URI cases
- support for [**RFC 3986**](https://datatracker.ietf.org/doc/html/rfc3986)

# Examples
## 1. Parse and hold a URI
This example parses a URI string and prints out all the contained elements. Then individual components are queried and printed if present.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;

int main(int argc, char *argv[])
{
   const uri u1{"https://www.example.com:8080/pages/from?country=au"};
   std::cout << u1 << '\n'
      << u1.get_component(uri::authority) << '\n'
      << u1.get_component(uri::host) << '\n';
   if (u1.test(uri::port))
      std::cout << u1.get_component(uri::port) << '\n';
   std::cout << u1.get_component(uri::query) << '\n';
   if (u1.test(uri::fragment))
      std::cout << u1.get_component(uri::fragment) << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```bash
$ ./example1
source: https://www.example.com:8080/pages/from?country=au
scheme: https
authority:      www.example.com:8080
host:   www.example.com
port:   8080
path:   /pages/from
query:  country=au

www.example.com:8080
www.example.com
8080
country=au
$
```

</p>
</details>

## 2. Parse a URI
This example parses a URI string and prints out all the contained elements. Note the use of `basic_uri`. The lifetime of the `u1` object
is undefined after the `if` statement is executed. The source string is not stored.

<details><summary><i>source</i></summary>
<p>

```c++
#include <iostream>
#include <fix8/uri.hpp>
using namespace FIX8;

int main(int argc, char *argv[])
{
   if (const basic_uri u1{ "ws://localhost:9229/f46db715-70df-43ad-a359-7f9949f39868" }; u1.count())
      std::cout << u1 << '\n';
   return 0;
}
```

</p>
</details>

<details><summary><i>output</i></summary>
</p>

```bash
$ ./example2
source: ws://localhost:9229/f46db715-70df-43ad-a359-7f9949f39868
scheme: ws
authority:      localhost:9229
host:   localhost
port:   9229
path:   /f46db715-70df-43ad-a359-7f9949f39868
$
```

</p>
</details>

# Building
This implementation is header only. Apart from standard C++20 includes there are no external dependencies needed in your application.
[Catch2](https://github.com/catchorg/Catch2.git) is used for the built-in unit tests.

# 1. Obtaining the source, building the examples
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
# 2. Using in your application
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

# API
The base class `basic_uri` performs the bulk of the work, holding a `std::string_view` of the source uri string. If you wish to manage the scope of the source uri yourself then
this class is the most efficient way to do so.

The derived class `uri` stores the source string and then builds a `basic_uri` using that string as its reference.
The supplied string is moved or copied and stored by the object. If your application needs the uri to hold and persist the source uri, this class is suitable.

Components are named by a public enum called `component`.

## ctor
```c++
constexpr basic_uri(std::string_view src);                           (1)
constexpr basic_uri() = default;                                     (2)
constexpr uri(std::string&& src);                                    (3)
constexpr uri() = default;                                           (4)
```

1. Construct a `basic_uri` from a `std::string_view`. This base class does not store the string. The source string must not go out of scope to use this object. Thows a `std::exception` if parsing fails.
1. Construct an empty `basic_uri`. It can be populated using `assign()`.
1. Construct a `uri` from a `std::string`. The supplied string is moved or copied and stored by the object. Thows a `std::exception` if parsing fails.
1. Construct an empty `uri`. It can be populated using `replace()`.

All of `uri` is within the namespace **`FIX8`**.

## dtor
```c++
~basic_uri();
~uri();
```

Destroy the `uri` or `basic_uri`. The `uri` object will release the stored string.

## `test`
```c++
constexpr bool uri::test(uri::component what) const;
```
Return `true` if the specified component is present in the uri.

## `assign`
```c++
constexpr int assign(std::string_view src);
```
Replace the current uri reference with the given reference. No storage is allocated. Return the number of components found.

## `replace`
```c++
constexpr std::string replace(std::string&& src);
```
Replace the current uri with the given string. The storage is updated with a copy of the string. The old string is returned.

## `get_component`
```c++
constexpr std::string_view get_component(component what) const;
```
Return a `std::string_view` of the specified component or empty if componment not found. Throws a `std::exception` if not a legal component.

## `get_name`
```c++
static constexpr std::string_view get_name(component what);
```
Return a `std::string_view` of the specified component name. Throws a `std::exception` if not a legal component.

## `get_source`
```c++
constexpr std::string_view get_source() const;
```
Return a `std::string_view` of the source uri. If not set result will be empty.

## `get_named_pair`
```c++
constexpr std::pair<std::string_view, std::string_view> get_named_pair(component what) const;
```
Return a `std::pair` of `std::string_view` for the specified component. The `first` will be the component name, `second` the component value. Suitable
for populating a JSON field. Throws a `std::exception` if not a legal component.

## `count`
```c++
constexpr int count() const;
```
Return the count of components in the uri.

## `parse`
```c++
constexpr int parse() const;
```
Parse the source string into components. Return the count of components found. Will reset a uri if already parsed. Thows a `std::exception` if parsing fails.

## `operator<<`
```c++
friend std::ostream& operator<<(std::ostream& os, const basic_uri& what);
```
Print the uri object to the specified stream. The source and individual components are printed.

## `get_buffer`
```c++
constexpr const std::string& get_buffer() const;
```
Return a `const std::string&` to the stored buffer. Only available from `uri`.

