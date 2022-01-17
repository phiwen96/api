#include <poll.h>
#include <fcntl.h>

export module Caller;

import Common;


export 
{
	struct caller 
	{
		auto get_address () const -> auto const&
		{
			return _ip_address;
		}

		auto get_socket () const -> auto 
		{
			return _sockid;
		}

		friend auto operator << (std::ostream& os, caller const& me) -> std::ostream&
		{
			os << "caller: " << me.get_address ();
			return os;
		}

		// constructor
		caller (int servsockid) 
		{
			struct {
				sockaddr_storage addr;
				unsigned int len = sizeof (addr);
			} detail;

		
			auto polls = std::array <pollfd, 2> 
			{
				pollfd {
					.fd = servsockid,
					.events = POLLIN
				}, 

				pollfd {
					.fd = 0, // Standard input
					.events = POLLIN
				}
			};

			// if ()

			poll (polls.data(), 2, NULL);





			if ((_sockid = accept (servsockid, (struct sockaddr*) &detail.addr, &detail.len)) == -1)
			{
				perror ("accept error");
				throw;
			}

			inet_ntop (detail.addr.ss_family, get_in_addr((struct sockaddr *)&detail.addr), _ip_address, sizeof (_ip_address));

		}

		caller (caller&&) = default;
		caller (caller const&) = default;

	private:
		char _ip_address [INET6_ADDRSTRLEN];
		int _sockid;
	};
}