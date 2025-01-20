#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>
#include "database/database.h"


#include <boost/asio/ssl.hpp>
#include <http/core/http_client.h>
#include <wincrypt.h>

int main(int argc, char* argv[]) {

    using namespace obelisk::database;
    try {
        boost::asio::io_context ioctx;
        obelisk::http::core::http_client client(ioctx);
        client.send_request("https://www.baidu.com", "GET", {}, nullptr);

        boost::asio::ip::tcp::resolver resolver(ioctx);
        auto endpoints = resolver.resolve("www.baidu.com", "https");
        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket(ioctx, ctx);
        boost::asio::connect(ssl_socket.lowest_layer(),endpoints);
        ssl_socket.set_verify_mode(boost::asio::ssl::verify_peer);
        ssl_socket.set_verify_callback(boost::asio::ssl::host_name_verification("www.baidu.com"));
        ssl_socket.handshake(boost::asio::ssl::stream_base::client);

        std::string data = "GET / HTTP/1.1\r\n"
                           "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
                           "Accept-Encoding: gzip, deflate, br, zstd\r\n"
                           "Accept-Language: en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7\r\n"
                           "Connection: keep-alive\r\n"
                           "Cookie: BIDUPSID=5B2314423AECCB5C0D97F4B494249160; PSTM=1716798669; BD_UPN=12314753; MSA_WH=1252_903; BAIDUID=C59780FC31E41ED3B64F8CA75A602A3F:SL=0:NR=10:FG=1; BDUSS=JLZGxjQ21rQ2ZnWUt5S0pVUEcyb1VVTTB6MGI1UHZVRFNab3pLd1lDNjE3QUJuRVFBQUFBJCQAAAAAAAAAAAEAAAAsk1M-1MLPwtChscoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALVf2Wa1X9lmLT; BDUSS_BFESS=JLZGxjQ21rQ2ZnWUt5S0pVUEcyb1VVTTB6MGI1UHZVRFNab3pLd1lDNjE3QUJuRVFBQUFBJCQAAAAAAAAAAAEAAAAsk1M-1MLPwtChscoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALVf2Wa1X9lmLT; MCITY=-104%3A; H_PS_PSSID=60600_60360_60748_60794_60827; H_WISE_SIDS=60600_60360_60748_60794_60827; H_WISE_SIDS_BFESS=60600_60360_60748_60794_60827; sug=3; sugstore=1; ORIGIN=2; bdime=0; BDORZ=B490B5EBF6F3CD402E515D22BCDA1598; H_PS_645EC=5122AIxnNCBe%2F%2BT8ggT4Wn2Pu5F4VbMdVokYwg5R%2FYQbEZALfTAgqnO5qSgXJuHrzDAp; delPer=0; BD_CK_SAM=1; PSINO=7; BDSVRTM=0\r\n"
                           "Host: www.baidu.com\r\n"
                           "Sec-Fetch-Dest: document\r\n"
                           "Sec-Fetch-Mode: navigate\r\n"
                           "Sec-Fetch-Site: none\r\n"
                           "Sec-Fetch-User: ?1\r\n"
                           "Upgrade-Insecure-Requests: 1\r\n"
                           "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36\r\n"
                           "sec-ch-ua: \"Chromium\";v=\"128\", \"Not;A=Brand\";v=\"24\", \"Google Chrome\";v=\"128\"\r\n"
                           "sec-ch-ua-mobile: ?0\r\n"
                           "sec-ch-ua-platform: \"Windows\"\r\n\r\n";
        ssl_socket.write_some(boost::asio::buffer(data));

        boost::asio::streambuf sbuf;
        auto bytes_readed = ssl_socket.read_some(sbuf.prepare(10240));


        obelisk::http::http_server server(ioctx, R"(D:/webroot)");
        server.listen("0.0.0.0", 3308);
        ioctx.run();
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
