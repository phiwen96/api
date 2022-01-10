export module Http;

import std;

export 
{
	enum class http_request_type 
	{
		GET, 
		HEAD,
		POST,
		PUT,
		DELETE
	};

	enum class http_version 
	{
		_1_0,
		_1_1
	};

	auto const http_status = std::unordered_map <int, std::string>
	{

	};

	template <typename Http_request_type = http_request_type, typename Http_version = http_version, typename Url = std::string> 
	struct http_request_line
	{
		http_request_line (Http_request_type const& request_type, Http_version const& version, Url const& url) : _request_type {request_type}, _version {version}, _url {url}
		{

		}

		http_request_line (http_request_line const& other) : _request_type {other._request_type}, _version {other._version}, _url {other._url}
		{

		}

	private:
		Http_request_type _request_type;
		Http_version _version;
		Url _url;
	};



	template <typename Http_version = http_version, typename Http_status = typename decltype (http_status)::value_type>
	struct http_response_status_line
	{
		http_response_status_line (Http_version const& version, Http_status const& status) : _version {version}, _status {status}
		{

		}

		http_response_status_line (http_response_status_line const& other) : _version {other._version}, _status {other._status}
		{

		}

	private:
		Http_version _version;
		Http_status _status;
		// Http_status_code _code;
		// Http_status_phrase _phrase;
	};

	template <typename Header_name, typename Header_value>
	struct http_header 
	{
		http_header (Header_name const& name, Header_value const& value) : _name {name}, _value {value}
		{

		}

		http_header (http_header const& other) : _name {other._name}, _value {other._value}
		{

		}

	private:
		Header_name _name;
		Header_value _value;
	};



	template <typename Http_request_line, typename Header>
	struct http_request
	{
		// http_request (Http_request_line const& request_line, std::initializer_list <Header> headers) : _request_line {request_line}, _headers {headers}
		// {

		// }

		// http_request (http_request const& other) : _request_line {other._request_line}, _headers {other._headers}
		// {

		// }

		// http_request (std::string const& request)

	// private:
		Http_request_line _request_line;
		std::vector <Header> _headers;
	};







	template <typename Http_response_status_line, typename Header>
	struct http_response 
	{
		http_response (Http_response_status_line const& status_line, std::initializer_list <Header> headers) : _status_line {status_line}, _headers {headers}
		{

		}

		http_response (http_response const& other) : _status_line {other._status_line}, _headers {other._headers}
		{

		}

	private:
		Http_response_status_line _status_line;
		std::vector <Header> _headers;
	};

	
}