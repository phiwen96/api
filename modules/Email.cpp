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
import RemoteClient;
#define BUFLEN 4096

#define EMAIL "testcase1234w@outlook.com"
#define PASSWORD "testcase1234p"
#define EMAIL_SERVER "smtp-mail.outlook.com";

export
{
	auto email(char const *to, char const *msg)->bool
	{

		return true;
	}
}