export module Email;

export import RemoteServer;

import std;
import Darwin;
using std::cout, std::endl;

constexpr auto EMAIL = "testcase1234w@outlook.com";
constexpr auto PASSWORD = "testcase1234p";
constexpr auto EMAIL_SERVER_ADDRESS = "smtp-mail.outlook.com";
constexpr auto EMAIL_SERVER_PORT = "25";
// constexpr auto EMAIL_SERVER_PORT = "443";

export {
	auto email (auto to, auto message) -> bool {
		int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
   
    portno = atoi(EMAIL_SERVER_PORT);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
		return false;
	}
    server = gethostbyname(EMAIL_SERVER_ADDRESS);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return false;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
		return false;
	}

    n = read(sockfd,buffer,255);
    if (n < 0) {
         perror("ERROR reading from socket");
		 return false;
	}
    printf("%s\n",buffer);

 bzero(buffer,256);

/*------------------------------*/

// printf("EHLO");

//     strcpy(buffer,"ehlo smtp.primus.ca\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) {
//          perror("ERROR writing to socket");
// 		 return false;
// 	}
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) {
//          perror("ERROR reading from socket");
// 		 return false;
// 	}
//     printf("%s\n",buffer);

// /*------------------------------*/    

// /*------------------------------*/

// printf("AUTH");

//     strcpy(buffer,"AUTH LOGIN\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

// /*------------------------------*/ 

// /*------------------------------*/

// printf("AUTH UID");

//     strcpy(buffer,"amR1bnBoeUBwcmltdXMuY2E=\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

// /*------------------------------*/ 

// /*------------------------------*/

// printf("AUTH PWD");

//     strcpy(buffer,"YXAwbGwwMTE=\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

// /*------------------------------*/ 

// /*------------------------------*/

// printf("MAIL FROM");

//     strcpy(buffer,"MAIL FROM: test1@primus.ca\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

// /*------------------------------*/ 

// /*------------------------------*/

// printf("MAIL TO");

        
//     strcpy(buffer,"RCPT TO: test2@primustel.ca\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

// /*------------------------------*/       

// /*------------------------------*/

// printf("DATA");

//     strcpy(buffer,"DATA\r\n");

//     n = write(sockfd,buffer,strlen(buffer));

//     strcpy(buffer,"Subject: test\r\n");

//     n = write(sockfd,buffer,strlen(buffer));

//     strcpy(buffer,"SMTP MAIL TOOL TEST WORKS!!!\r\n");

//     n = write(sockfd,buffer,strlen(buffer));

//     strcpy(buffer,"\n\n");

//     n = write(sockfd,buffer,strlen(buffer));

//     strcpy(buffer,".\n");

//     n = write(sockfd,buffer,strlen(buffer));


// /*------------------------------*/ 

// /*------------------------------*/ 

//     strcpy(buffer,"quit\n");

//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0) 
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0) 
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);

/*------------------------------*/
		return true;
	}
}