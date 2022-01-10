export module Http;

import std;

export 
{
	auto const http_status = std::unordered_map <int, std::string>
	{
		{200, "OK"},
		{301, "Moved Permanently"},
		{400, "Bad Request"},
		{404, "Not Found"},
		{505, "HTTP Version Not Supported"}
	};

	using http_status_t = typename decltype (http_status)::value_type;

	struct http_request_line
	{
		std::string request_type;
		float version;
		std::string url;

		friend auto operator << (std::ostream& os, http_request_line const& me) -> std::ostream&
		{
			os << me.request_type << ' ' << me.url << ' ' << "HTTP/" << std::fixed << std::setprecision (1) << me.version << "\r\n";
			return os;
		}

		static auto parse (std::string const& s) -> std::optional <http_request_line>
		{
			auto r = http_request_line {};
			return r;
		}
	};

	struct http_response_status_line
	{
		double version;
		std::string status;

		static auto parse (std::string const& s) -> std::optional <http_response_status_line>
		{
			auto r = http_response_status_line {};
			return r;
		}
	};

	struct http_header 
	{
		std::string name;
		std::string value;

		friend auto operator << (std::ostream& os, http_header const& me) -> std::ostream&
		{
			os << me.name << ": " << me.value << "\r\n";
			return os;
		}

		static auto parse (std::string const& s) -> std::optional <http_header>
		{
			auto r = http_header {};
			return r;
		}
	};



	struct http_request
	{
		http_request_line request_line;
		std::vector <http_header> headers;

		friend auto operator << (std::ostream& os, http_request const& me) -> std::ostream&
		{
			os << me.request_line << '\n';
			for (auto const& header : me.headers)
				os << header << "\r\n";
			return os;
		}

		static auto parse (std::string const& s) -> std::optional <http_request>
		{
			auto request = http_request {};

			auto ss = std::stringstream {s};
			auto line = std::string {};


			// parse request line
		 	std::getline (ss, line);

			if (auto request_line = http_request_line::parse (line);
				request_line.has_value ())
			{
				request.request_line = request_line.value ();

			} else 
			{
				return std::nullopt;
			}

			// parse headers
			while (std::getline (ss, line))
			{
				if (auto header = http_header::parse (line);
					header.has_value ())
				{
					request.headers.push_back (header.value ());
					
				} else 
				{
					return std::nullopt;
				}
			}

			return request;
		}
	};

	struct http_response 
	{
		http_response_status_line status_line;
		std::vector <http_header> headers;

		static auto parse (std::string const& s) -> std::optional <http_response>
		{
			auto r = http_response {};
			return r;
		}
	};


	
}