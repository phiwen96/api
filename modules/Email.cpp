module;
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

// #include <base64_codec.h>
export module Email;
import RemoteServer;
#define BUFLEN 4096

#define EMAIL "testcase1234w@outlook.com"
#define PASSWORD "testcase1234p"
#define EMAIL_SERVER_ADDRESS "smtp-mail.outlook.com"
#define EMAIL_SERVER_PORT "25"

export
{
	auto email(char const *to, char const *msg)->bool
	{
		auto server = remote_server_t {EMAIL_SERVER_PORT, EMAIL_SERVER_ADDRESS};


		return true;
	}
}