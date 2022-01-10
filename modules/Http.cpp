export module Http;

export 
{
	template <typename Http_request_type, typename Http_version, typename Url> 
	struct http_request_line
	{
		http_request_line (Http_request_type const& type, Http_version const& version, Url const& url) : _type {type}, _version {version}, _url {url}
		{

		}

	private:
		Http_request_type _type;
		Http_version _version;
		Url _url;
	};



	template <typename Http_version, typename Http_status_code, typename Http_status_phrase>
	struct http_response_status_line
	{
		http_response_status_line (Http_version const& version, Http_status_code const& code, Http_status_phrase const& phrase) : _version {version}, _code {code}, _phrase {phrase}
		{

		}

	private:
		Http_version _version;
		Http_status_code _code;
		Http_status_phrase _phrase;
	};
}