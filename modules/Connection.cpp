export module Connection;

import Common;
import Socket;

export
{
	template <typename T>
	concept Connection = requires(T const& t)
	{
		{remoteIP (t)} -> String;
		{socket (t)} -> Socket;
	};

	struct connection
	{
		connection(int sockid) noexcept : _sockid{sockid}
		{

			// connect (_sockid, )
		}
		connection(connection &&) = default;
		connection(connection const &) = default;
		

		auto read() -> String auto &&
		{
			auto len = getpagesize();
			char buf[len];
			int numbytes;
			if ((numbytes = recv(_sockid, buf, len - 1, 0)) == -1)
			{
				perror("recv error");
				throw;
			}
			buf[numbytes] = '\0';
			return std::move(std::string{buf});
		}
		void write(std::string &&src)
		{
			if (sendall(_sockid, src.c_str(), src.size()) == -1)
			{
				perror("sendall error");
				throw;
			}
		}
		friend auto operator<<(std::ostream &os, connection const &me) -> std::ostream&
		{
			os << remoteIP (me);
			return os;
		}

		friend auto remoteIP(connection const& me) -> std::string
		{
			// return "hej";
			struct
			{
				sockaddr_storage addr;
				char ip_address [INET6_ADDRSTRLEN];
				int port;
				unsigned int len = sizeof(addr);
			} detail;

			if (getpeername(me._sockid, (struct sockaddr *)&detail.addr, &detail.len) == -1)
			{
				perror("getpeername error");
				throw;
			}

			// deal with both IPv4 and IPv6:
			if (detail.addr.ss_family == AF_INET)
			{
				struct sockaddr_in *s = (struct sockaddr_in *)&detail.addr;
				detail.port = ntohs(s->sin_port);
				inet_ntop(AF_INET, &s->sin_addr, detail.ip_address, sizeof detail.ip_address);
			}
			else
			{ // AF_INET6
				struct sockaddr_in6 *s = (struct sockaddr_in6 *)&detail.addr;
				detail.port = ntohs(s->sin6_port);
				inet_ntop(AF_INET6, &s->sin6_addr, detail.ip_address, sizeof detail.ip_address);
			}
			// std::cout << detail.ip_address << std::endl;

			return std::string {detail.ip_address};//std::move (std::string {detail.ip_address});
		}

		friend auto socket (connection const& me) -> int 
		{
			return me._sockid;
		}

		//  auto socket ()

	private:
		// std::string _ip_address;
		int _sockid;
	};


}
