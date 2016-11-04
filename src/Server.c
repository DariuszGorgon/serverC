/*
 ============================================================================
 Name        : Server.c
 Author      : AJ
 Version     :
 Copyright   : struk
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 5600    // port, z którym będą się łączyli użytkownicy
#define MAXDATASIZE 100
#define BACKLOG 10     // jak dużo możę być oczekujących połączeń w kolejce

void sigchld_handler( int s )
{
    while( wait( NULL ) > 0 );

}

int main( void )
{
    int sockfd, new_fd,info,por; // nasłuchuj na sock_fd, nowe połaczenia na new_fd
    struct sockaddr_in serv_addr; // informacja o moim adresie
    struct sockaddr_in client_addr; // informacja o adresie osoby łączącej się
    int sin_size;
    char buf2[MAXDATASIZE];
    int yes = 1;

    if(( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == - 1 ) {
        perror( "socket" );
        exit( 1 );
    }

    if( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, & yes, sizeof( int ) ) == - 1 ) {
        perror( "setsockopt" );
        exit( 1 );
    }

    serv_addr.sin_family = AF_INET; // host byte order
    serv_addr.sin_port = htons( MYPORT ); // short, network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY; // uzupełnij moim adresem IP
    memset( &( serv_addr.sin_zero ), '\0', 8 ); // wyzeruj resztę struktury

    if( bind( sockfd,( struct sockaddr * ) & serv_addr, sizeof( struct sockaddr ) )
    == - 1 ) {
        perror( "bind" );
        exit( 1 );
    }

    if( listen( sockfd, BACKLOG ) == - 1 ) {
        perror( "listen" );
        exit( 1 );
    }



    while( 1 ) { // głowna pętla accept()
        sin_size = sizeof( struct sockaddr_in );
        if(( new_fd = accept( sockfd,( struct sockaddr * ) & client_addr,
        & sin_size ) ) == - 1 ) {
            perror( "accept" );
            continue;
        }
        printf( "server: got connection from %s\n",
        inet_ntoa( client_addr.sin_addr ) );
        if( !fork() ) {
        close(sockfd);
        if((info=  read( new_fd,buf2,MAXDATASIZE-1))==-1){
        perror("odbieranie");
        exit(1);
        }
        por = strcmp(buf2,'test1');
        if(por){
            close( sockfd );
            if( write( new_fd, "OK!\n", 4 ) == - 1 )
                 perror( "wysylanie" );
          }else{
            if(write( new_fd, "NO!\n", 4 ) == - 1 )
                 perror( "wysylanie" );
        }
            close( new_fd );
            exit( 0 );
        }
        close( new_fd );
    }

    return 0;
}
