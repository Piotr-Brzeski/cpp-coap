# cpp-coap

C++ COAP library based on [libcoap](https://libcoap.net).

## Build
Use [XcodeGen](https://github.com/yonaskolb/XcodeGen) to generate Xcode project
or [NinjaGen](https://github.com/Piotr-Brzeski/NinjaGen) to generate ninja build file.

### Dependencies
On macOS [libcoap](https://libcoap.net) and [openssl](https://www.openssl.org)
may be installed using [Homebrew](https://brew.sh)

```
brew install openssl
ln -s /opt/homebrew/opt/openssl@3/include/openssl /opt/homebrew/include/openssl
ln -s /opt/homebrew/opt/openssl@3/lib/libssl.dylib /opt/homebrew/lib
brew install libcoap
```

There is a [bug](https://github.com/openssl/openssl/issues/20753)
in the libcrypto library that prevents debugging the application.
[Fix](https://github.com/openssl/openssl/pull/20305) is ready,
but not merged to the current version of the openssl library.

As a workaround create `~/.lldbinit` file:
```
settings set platform.plugin.darwin.ignored-exceptions EXC_BAD_INSTRUCTION
process handle SIGILL -n true -p true -s false
```

## Examples

In order to build `coap-dtls-client` example application
create `configuration.h` file in the `examples` directory.
Content of the `configuration.h` file:

```
namespace configuration {

constexpr char const* ip = "127.0.0.1";
constexpr int port = 5684;
constexpr char const* identity = "name";
constexpr char const* key = "key";

}
```
