export module Http;

import std;

export
{
	using std::cout, std::endl;

	auto const http_status = std::unordered_map<int, std::string>{
		{200, "OK"},
		{301, "Moved Permanently"},
		{400, "Bad Request"},
		{404, "Not Found"},
		{505, "HTTP Version Not Supported"}};

	using http_status_t = typename decltype(http_status)::value_type;

	struct http_request_line
	{
		std::string request_type;
		float version;
		std::string url;

		friend auto operator<<(std::ostream &os, http_request_line const &me) -> std::ostream &
		{
			os << me.request_type << ' ' << me.url << ' ' << "HTTP/" << std::fixed << std::setprecision(1) << me.version << "\r\n";
			return os;
		}

		static auto parse(std::string const &line_in) -> std::optional<http_request_line>
		{
			auto line_out = http_request_line{};

			if (auto i = line_in.find(' ');
				i != std::string::npos)
			{
				line_out.request_type = std::string {line_in.begin (), line_in.begin() + i};
				// cout << "request type:" << line_out.request_type << endl;
				if (auto j = line_in.find ("HTTP/"); 
					j != std::string::npos)
				{
					line_out.url = std::string {line_in.begin() + i + 1, line_in.begin() + j};
					// cout << "url:" << line_out.url << endl;
					auto version = std::string {line_in.begin() + j + 5, line_in.end()};
					// cout << "version:" << version << endl;

					line_out.version = std::stof (version);
				} 

				else 
				{
					std::cout << "Failed to parse url" << std::endl;
					return std::nullopt;
				}
			}

			else 
			{
				std::cout << "Failed to parse request type" << std::endl;
				return std::nullopt;
			}

			return line_out;
		}
	};

	struct http_header
	{
		std::string name;
		std::string value;

		friend auto operator<<(std::ostream &os, http_header const &me) -> std::ostream &
		{
			os << me.name << ": " << me.value << "\r\n";
			return os;
		}

		static auto parse(std::string const &header_in) -> std::optional<http_header>
		{
			// std::cout << "header in:" << header_in << std::endl;
			auto header_out = http_header{};

			if (auto i = header_in.find(": "); i != std::string::npos)
			{
				header_out.name = std::string{header_in.begin(), header_in.begin() + i};
				// std::cout << "name:" << header_out.name << std::endl;

				header_out.value = std::string{header_in.begin() + i + 2, header_in.end()};
				// std::cout << "value:" << header_out.value << std::endl;

				if (header_out.value.size() == 0)
				{
					std::cout << "error parsing header-value" << std::endl;
					return std::nullopt;
				}
			}
			else
			{
				std::cout << "error parsing header-name" << std::endl;
				return std::nullopt;
			}

			return header_out;
		}
	};

	struct http_request
	{
		http_request_line request_line;
		std::vector<http_header> headers;
		std::string data;

		friend auto operator<<(std::ostream &os, http_request const &me) -> std::ostream &
		{
			os << me.request_line;

			for (auto const &header : me.headers)
			{
				os << header;
			}

			if (me.data.size() > 0)
			{
				os << "\r\n" << me.data;
			}

			return os;
		}

		static auto parse (std::string s) -> std::optional<http_request>
		{
			auto request = http_request{};

			if (auto i = s.find ("\r\n\r\n")) // cut data from string 
			{
				request.data = std::string (s.begin () + i + 4, s.end ());
				s.erase (i);
			}

			auto ss = std::stringstream{s};
			auto line = std::string{};

			std::getline(ss, line); // get request-line

			if (auto request_line = http_request_line::parse(line);
				request_line.has_value())
			{
				request.request_line = request_line.value();
			}
			else
			{
				std::cout << "Failed to parse request line" << std::endl;
				return std::nullopt;
			}			

			while (std::getline(ss, line)) // get header
			{
				if (auto header = http_header::parse(line);
					header.has_value())
				{
					request.headers.push_back(header.value());
				}
				else
				{
					return std::nullopt;
				}
			}

			return request;
		}
	};

	struct http_status_line
	{
		float version;
		int status_code;
		std::string status_phrase;

		static auto parse(std::string const &line_in) -> std::optional<http_status_line>
		{
			auto line_out = http_status_line{};
			auto version = std::string {};
			auto status_code = std::string {};

			auto stream = std::stringstream {line_in};

			stream >> version;
			stream >> status_code;
			stream >> line_out.status_phrase;

			if (auto i = version.find ("HTTP/");
				i != std::string::npos)
			{
				line_out.version = std::stof (std::string {version.begin() + i + 5, version.end ()});

			} else 
			{
				std::cout << "Failed to find http version" << endl;
				return std::nullopt;
			}

			line_out.status_code = std::stof (status_code);

			return line_out;
		}

		friend auto operator<<(std::ostream &os, http_status_line const &me) -> std::ostream &
		{
			os << "HTTP/" << std::fixed << std::setprecision(1) << me.version << ' ' << me.status_code << ' ' << me.status_phrase << "\r\n";
			return os;
		}
	};



	struct http_response
	{
		http_status_line status_line;
		std::vector<http_header> headers;

		static auto parse(std::string const &response_in) -> std::optional<http_response>
		{
			auto response_out = http_response {};
			auto line = std::string {};
			auto stream = std::stringstream {response_in};
			getline (stream, line);

			if (auto status_line = http_status_line::parse (line);
				status_line.has_value ())
			{
				response_out.status_line = status_line.value ();

				while (getline (stream, line))
				{
					if (auto header = http_header::parse (line);
						header.has_value ())
					{
						response_out.headers.push_back (header.value ());

					} else 
					{
						return std::nullopt;
					}
				}

			} else 
			{
				return std::nullopt;
			}

			return response_out;
		}

		friend auto operator<<(std::ostream &os, http_response const &me) -> std::ostream &
		{
			os << me.status_line;
			for (auto const &header : me.headers)
				os << header;
			return os;
		}
	};

	inline auto to_string(auto const &request)->std::string
	{
		auto result = std::string{};
		auto stream = std::stringstream{result};
		stream << request;
		return stream.str();
	}
}