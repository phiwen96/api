export module Caller;

import Common;

export 
{
	struct caller 
	{
		char ip_address [INET6_ADDRSTRLEN];
		int sockid;
	};
}