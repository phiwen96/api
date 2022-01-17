export module Connection;

import Common;

export 
{
	template <typename T>
	concept Connection = requires (T t, std::string&& s)
	{
		t.read (s); 
		t.write ("hej");
	};

	struct connection 
	{
		connection (std::string&& ip_address, int sockid) noexcept : _ip_address {std::move (ip_address)}, _sockid {sockid}
		{

		}
		connection (connection&&) = default;
		connection (connection const&) = default;
		auto read (auto&& dst) noexcept
		{
			
		}

	private:
		int _sockid;
	};
}

