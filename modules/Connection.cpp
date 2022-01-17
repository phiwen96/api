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
		
		auto read(auto &dst)
		{
			if (recv(_sockid, &dst, sizeof (dst)) == -1)
			{
				perror("recv error");
				throw;
			}
		}
		auto write(auto &&src)
		{
			if (sendall(_sockid, src, std::move(src)) == -1)
			{
				perror("sendall error");
				throw;
			}
		}

	private:
		std::string _ip_address;
		int _sockid;
	};
}
