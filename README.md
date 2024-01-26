<p align="center">
  <a href="https://www.fix8mt.com"><img src="https://github.com/fix8mt/uri/assets/fix8mt_Master_Logo_Green_Trans.png" width="200"></a>
</p>

# uri

### A lightweight C++20 URI parser

------------------------------------------------------------------------
## Introduction
This is a lightweight URI parser implementation featuring zero-copy, minimal storage and high performance.

## Quick links
|**Link**|**Description**|
--|--
|[Here](https://github.com/fix8mt/uri/blob/main/include/fix8/uri.hpp)| for implementation|
|[Examples](https://github.com/fix8mt/uri/examples)| for examples and test cases|

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

