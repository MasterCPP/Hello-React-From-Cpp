/*

MIT License

Copyright (c) 2020 Chris Mc prince.chrismc@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <iostream>
#include <map>

#include <restinio/all.hpp>
#include <restinio/tls.hpp>

#include <clara.hpp>
#include <fmt/format.h>

const char *content_type_by_file_extention(const restinio::string_view_t &ext) {
  // Incomplete list of mime types from here:
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types
  if (ext == "aac")
    return "audio/aac";
  if (ext == "abw")
    return "application/x-abiword";
  if (ext == "arc")
    return "application/octet-stream";
  if (ext == "avi")
    return "video/x-msvideo";
  if (ext == "azw")
    return "application/vnd.amazon.ebook";
  if (ext == "bin")
    return "application/octet-stream";
  if (ext == "bz")
    return "application/x-bzip";
  if (ext == "bz2")
    return "application/x-bzip2";
  if (ext == "csh")
    return "application/x-csh";
  if (ext == "css")
    return "text/css";
  if (ext == "csv")
    return "text/csv";
  if (ext == "doc")
    return "application/msword";
  if (ext == "docx")
    return "application/"
           "vnd.openxmlformats-officedocument.wordprocessingml.document";
  if (ext == "eot")
    return "application/vnd.ms-fontobject";
  if (ext == "epub")
    return "application/epub+zip";
  if (ext == "gif")
    return "image/gif";
  if (ext == "htm" || ext == "html")
    return "text/html";
  if (ext == "ico")
    return "image/x-icon";
  if (ext == "ics")
    return "text/calendar";
  if (ext == "jar")
    return "application/java-archive";
  if (ext == "jpeg" || ext == "jpg")
    return "image/jpeg";
  if (ext == "js")
    return "application/javascript";
  if (ext == "json")
    return "application/json";
  if (ext == "mid" || ext == "midi")
    return "audio/midi";
  if (ext == "mpeg")
    return "video/mpeg";
  if (ext == "mpkg")
    return "application/vnd.apple.installer+xml";
  if (ext == "odp")
    return "application/vnd.oasis.opendocument.presentation";
  if (ext == "ods")
    return "application/vnd.oasis.opendocument.spreadsheet";
  if (ext == "odt")
    return "application/vnd.oasis.opendocument.text";
  if (ext == "oga")
    return "audio/ogg";
  if (ext == "ogv")
    return "video/ogg";
  if (ext == "ogx")
    return "application/ogg";
  if (ext == "otf")
    return "font/otf";
  if (ext == "png")
    return "image/png";
  if (ext == "pdf")
    return "application/pdf";
  if (ext == "ppt")
    return "application/vnd.ms-powerpoint";
  if (ext == "pptx")
    return "application/"
           "vnd.openxmlformats-officedocument.presentationml.presentation";
  if (ext == "rar")
    return "archive	application/x-rar-compressed";
  if (ext == "rtf")
    return "application/rtf";
  if (ext == "sh")
    return "application/x-sh";
  if (ext == "svg")
    return "image/svg+xml";
  if (ext == "swf")
    return "application/x-shockwave-flash";
  if (ext == "tar")
    return "application/x-tar";
  if (ext == "tif" || ext == "tiff")
    return "image/tiff";
  if (ext == "ts")
    return "application/typescript";
  if (ext == "ttf")
    return "font/ttf";
  if (ext == "vsd")
    return "application/vnd.visio";
  if (ext == "wav")
    return "audio/x-wav";
  if (ext == "weba")
    return "audio/webm";
  if (ext == "webm")
    return "video/webm";
  if (ext == "webp")
    return "image/webp";
  if (ext == "woff")
    return "font/woff";
  if (ext == "woff2")
    return "font/woff2";
  if (ext == "xhtml")
    return "application/xhtml+xml";
  if (ext == "xls")
    return "application/vnd.ms-excel";
  if (ext == "xlsx")
    return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  if (ext == "xml")
    return "application/xml";
  if (ext == "xul")
    return "application/vnd.mozilla.xul+xml";
  if (ext == "zip")
    return "archive	application/zip";
  if (ext == "3gp")
    return "video/3gpp";
  if (ext == "3g2")
    return "video/3gpp2";
  if (ext == "7z")
    return "application/x-7z-compressed";

  return "application/text";
}

auto server_handler(const std::string &root_dir) {
  std::string server_root_dir;

  if (root_dir.empty()) {
    server_root_dir = "./";
  } else if (root_dir.back() != '/' && root_dir.back() != '\\') {
    server_root_dir = root_dir + '/';
  } else {
    server_root_dir = root_dir;
  }

  return [server_root_dir](auto req) {
    auto path = req->header().path();

    if (std::string::npos == path.find("..")) {
      // A nice path.
      auto file_path = server_root_dir + std::string{path.data(), path.size()};
      if (file_path.back() == '/') {
        file_path += "index.html";
      }
      const auto seperator = file_path.find_last_of('.');
      const auto ext = ( seperator == std::string::npos )
                           ? ""
                           : file_path.substr(seperator + 1);
      try {
        auto sf = restinio::sendfile(file_path);
        auto modified_at =
            restinio::make_date_field_value(sf.meta().last_modified_at());

        auto expires_at = restinio::make_date_field_value(
            std::chrono::system_clock::now() + std::chrono::hours(24 * 7));

        return req->create_response()
            .append_header(restinio::http_field::server, "RESTinio")
            .append_header_date_field()
            .append_header(restinio::http_field::last_modified,
                           std::move(modified_at))
            .append_header(restinio::http_field::expires, std::move(expires_at))
            .append_header(restinio::http_field::content_type,
                           content_type_by_file_extention(ext))
            .set_body(std::move(sf))
            .done();

      } catch (const std::exception &) {
        return req->create_response(restinio::status_not_found())
            .append_header_date_field()
            .connection_close()
            .done();
      }
    } else {
      // Bad path.
      return req->create_response(restinio::status_forbidden())
          .append_header_date_field()
          .connection_close()
          .done();
    }
  };
}

struct app_args_t {
  bool m_help{false};
  std::string m_address{"localhost"};
  std::uint16_t m_port{8080};
  std::size_t m_pool_size{1};
  std::string m_root_dir{"."};
  std::string m_certs_dir{"."};

  static app_args_t parse(int argc, const char *argv[]) {
    using namespace clara;

    app_args_t result;

    auto cli =
        Opt(result.m_address, "address")["-a"]["--address"](
            fmt::format("address to listen (default: {})", result.m_address)) |
        Opt(result.m_port, "port")["-p"]["--port"](
            fmt::format("port to listen (default: {})", result.m_port)) |
        Opt(result.m_pool_size, "thread-pool size")["-n"]["--thread-pool-size"](
            fmt::format("The size of a thread pool to run server (default: {})",
                        result.m_pool_size)) |
        Arg(result.m_root_dir, "root-dir")(
            fmt::format("server root dir (default: '{}')", result.m_root_dir)) |
        Arg(result.m_certs_dir, "certs-dir")(fmt::format(
            "server certs dir (default: '{}')", result.m_certs_dir)) |
        Help(result.m_help);

    auto parse_result = cli.parse(Args(argc, argv));
    if (!parse_result) {
      throw std::runtime_error{fmt::format("Invalid command-line arguments: {}",
                                           parse_result.errorMessage())};
    }

    if (result.m_help) {
      std::cout << cli << std::endl;
    }

    return result;
  }
};

int main(int argc, char const *argv[]) {
  using namespace std::chrono;

  try {
    const auto args = app_args_t::parse(argc, argv);

    if (args.m_help) {
      return 0;
    }

    using traits_t = restinio::single_thread_tls_traits_t<
        restinio::asio_timer_manager_t,
        restinio::single_threaded_ostream_logger_t>;

    namespace asio_ns = restinio::asio_ns;

    asio_ns::ssl::context tls_context{asio_ns::ssl::context::tls};
    tls_context.set_options(
        asio_ns::ssl::context::default_workarounds |
        asio_ns::ssl::context::no_sslv2 | asio_ns::ssl::context::no_sslv3 |
        asio_ns::ssl::context::no_tlsv1 | asio_ns::ssl::context::single_dh_use);

    tls_context.use_certificate_chain_file(args.m_certs_dir + "/server.pem");
    tls_context.use_private_key_file(args.m_certs_dir + "/key.pem",
                                     asio_ns::ssl::context::pem);
    tls_context.use_tmp_dh_file(args.m_certs_dir + "/dh2048.pem");

    restinio::run(restinio::on_thread_pool<traits_t>(args.m_pool_size)
                      .address("0.0.0.0")
                      .port(args.m_port)
                      .request_handler(server_handler(args.m_root_dir))
                      .read_next_http_message_timelimit(10s)
                      .write_http_response_timelimit(1s)
                      .handle_request_timeout(1s)
                      .tls_context(std::move(tls_context)));
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
