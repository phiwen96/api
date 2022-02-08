#include <stdio.h>

#include <stdlib.h>

#include <openssl/bio.h> /* BasicInput/Output streams */

#include <openssl/err.h> /* errors */

#include <openssl/ssl.h> /* core library */
export module Email;

export import RemoteServer;

import std;
import Darwin;

#define BuffSize 1024
using std::cout, std::endl;

constexpr auto EMAIL = "testcase1234w@outlook.com";
constexpr auto PASSWORD = "testcase1234p";
constexpr auto EMAIL_SERVER_ADDRESS = "smtp-mail.outlook.com";
constexpr auto EMAIL_SERVER_PORT = "587"; // TCP ports 465 or 587
// constexpr auto EMAIL_SERVER_PORT = "443";


export
{
	auto email(auto to, auto message)->bool {
		printf("OpenSSL version: %s\n", OpenSSL_version(SSLEAY_VERSION));
		return true;
	}
}