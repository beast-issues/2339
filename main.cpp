#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <iostream>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;
using stream_t = websocket::stream<ssl::stream<tcp::socket>>;

std::string hexify(std::string const& in)
{
    std::string result;
    result.reserve(in.size() * 2);

    auto to_hex = [](unsigned char c) -> std::string
    {
        static const char hexdigit[] = "0123456789abcdef" ;
        auto result = std::string(4, ' ');
        result [0] = '0';
        result [1] = 'x';
        result [2] = hexdigit[(c >> 4) & 0xf];
        result [3] = hexdigit[c & 0xf];
        return result;

    };

    for (auto c:in)
    {
        if (std::isprint(c))
            result += c;
        else
            result += to_hex(c);
    }
    return result;
}
int
main()
{

    std::string host = "vstream.binance.com";
    auto const port = "443";
    auto const path = "/ws/BTC-211112-59000-P@depth20";
    auto const rpcJson = "{\"method\":\"BINARY\", \"params\":[\"false\"], \"id\":1}";

    net::io_context ioc;
    tcp::resolver resolver{ioc};

    ssl::context ctx{ssl::context::sslv23};
    ctx.set_verify_mode(ssl::verify_none);

    stream_t s{ioc, ctx};
    net::connect(beast::get_lowest_layer(s), resolver.resolve(host, port));

    // SSL handshake
    s.next_layer().handshake(ssl::stream_base::client);

    s.handshake(host + ":" + port, path);

    std::cout << "connected." << std::endl;

    // send request to the websocket
    s.write(net::buffer("{\"method\":\"BINARY\", \"params\":[\"false\"], \"id\":1}"));

    {
        boost::beast::multi_buffer m_buf;
        s.read(m_buf);
        std::string strbuf = hexify(beast::buffers_to_string(m_buf.data()));
        m_buf.consume(m_buf.size());

        std::cout << "s. " << strbuf << std::endl;
    }
    return 0;
}