# cpp-coap

C++ COAP library based on [libcoap](https://libcoap.net).

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

}```