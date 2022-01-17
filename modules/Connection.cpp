export module Connection;

import Common;

export
{
	template <typename T>
	concept Connection = requires(T t, std::string && s)
	{
		t.read(s);
		t.write("hej");
	};

	struct connection
	{
		connection(std::string &&ip_address, int sockid) noexcept : _ip_address{std::move(ip_address)}, _sockid{sockid}
		{
		}
		connection(connection &&) = default;
		connection(connection const &) = default;

		auto read() -> String auto&&
		{
			auto len = getpagesize();
			char buf [len];
			int numbytes;
			if ((numbytes = recv(_sockid, buf, len - 1, 0)) == -1)
			{
				perror("recv error");
				throw;
			}
			buf[numbytes] = '\0';
			return std::move (std::string {buf});
		}
		void write(auto &&src)
		{
			if (sendall(_sockid, src, std::move(src)) == -1)
			{
				perror("sendall error");
				throw;
			}
		}
		friend auto& operator << (std::ostream& os, connection const& me)
		{
			os << "remoteIP >> " << me._ip_address;
			return os;
		}

	private:
		std::string _ip_address;
		int _sockid;
	};
}
