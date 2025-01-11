#include "binary_valentine/https/https_request_helper.h"

#include <iterator>
#include <string>
#include <type_traits>

#ifdef WIN32
#	include <winhttp.h>
#	pragma comment(lib, "winhttp")
#endif //#ifdef WIN32

#include "binary_valentine/core/core_error.h"
#include "binary_valentine/version.h"

namespace bv::https
{

namespace
{
#ifdef WIN32
struct hinternet_closer final
{
	using pointer = HINTERNET;

	void operator()(pointer h) const
	{
		::WinHttpCloseHandle(h);
	}
};

using hinternet_handle = std::unique_ptr<HINTERNET, hinternet_closer>;

template<typename Str>
std::wstring ascii_to_wstring(const Str& str)
{
	std::wstring result;
	result.reserve(std::size(str));
	using unsigned_source_char_type = std::make_unsigned_t<typename Str::value_type>;
	for (auto it = std::cbegin(str); it != std::end(str); ++it)
		result.push_back(static_cast<unsigned_source_char_type>(*it));

	return result;
}

std::wstring get_path_with_query(const boost::url_view& url)
{
	auto result = ascii_to_wstring(url.encoded_path());
	if (url.has_query())
	{
		result += L'?';
		result += ascii_to_wstring(url.encoded_query());
	}
	return result;
}
#endif //#ifdef WIN32

[[noreturn]] void throw_unable_to_make_https_request(const boost::url_view& url)
{
	throw core::core_error(core::core_errc::unable_to_make_https_request,
		"Unable to make https request to "
			+ std::string(static_cast<boost::core::string_view>(url)));
}
} //namespace

boost::asio::awaitable<std::string> https_request_helper::get(
	const boost::url_view& url, const options& opts)
{
	//TODO: use boost::beast with boost::asio and OpenSSL

#ifdef WIN32
	hinternet_handle session(
		::WinHttpOpen(ascii_to_wstring(version::tool_name).c_str(),
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0));

	if (!session)
		throw_unable_to_make_https_request(url);

	::WinHttpSetTimeouts(session.get(),
		static_cast<int>(opts.resolve_timeout),
		static_cast<int>(opts.connect_timeout),
		static_cast<int>(opts.send_timeout),
		static_cast<int>(opts.read_timeout));

	hinternet_handle connection(
		::WinHttpConnect(session.get(),
			ascii_to_wstring(url.encoded_host()).c_str(),
			INTERNET_DEFAULT_HTTPS_PORT, 0));

	if (!connection)
		throw_unable_to_make_https_request(url);

	hinternet_handle request(
		::WinHttpOpenRequest(
			connection.get(),
			L"GET",
			get_path_with_query(url).c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE));

	if (!request)
		throw_unable_to_make_https_request(url);

	if (!::WinHttpSendRequest(request.get(),
		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0,
		0, 0))
	{
		throw_unable_to_make_https_request(url);
	}

	if (!::WinHttpReceiveResponse(request.get(), NULL))
		throw_unable_to_make_https_request(url);

	std::string result;
	DWORD size = 0;
	do
	{
		if (!::WinHttpQueryDataAvailable(request.get(), &size))
			throw_unable_to_make_https_request(url);

		std::size_t old_size = result.size();
		result.resize(old_size + size);

		DWORD downloaded = 0;
		if (!::WinHttpReadData(request.get(), &result[old_size],
			size, &downloaded) || downloaded != size)
		{
			throw_unable_to_make_https_request(url);
		}
	}
	while (size > 0);

	co_return result;
#endif //#ifdef WIN32

	//TODO: add linux fallback
	throw_unable_to_make_https_request(url);
}

} //namespace bv::https
