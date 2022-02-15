// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define MAXPENDING 20
#define BUFFERSIZE 1024
#define MAX_RESPONSE_SIZE 500

struct sockaddr_in createAdr(int port, struct sockaddr_in *address);
int tryParsePort(const char *string);
struct sockaddr_in bindAdr(int server_fd, struct sockaddr_in *address);
int parseArgs(int argc, const char *const *argv);
int getSocket();
char *parseUrl(const char *string);
void startListening(int server_fd);
int acceptConnection(int server_fd, struct sockaddr_in *address, int *addrlen);

char *getResponse(char *http);

char *getUserName();

char *getCpuName();

char *getLoad();

char *getNotValid();

char *getMsgBody(const char *http);

void failure(const char* err){
    fprintf(stderr,"%s", err);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    int port = parseArgs(argc, argv);

    int server_fd, remoteSocket;

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = getSocket();
    address = createAdr(port, &address);
    address = bindAdr(server_fd, &address);
    startListening(server_fd);
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        remoteSocket = acceptConnection(server_fd, &address, &addrlen);

        char buffer[BUFFERSIZE] = {0};
        read(remoteSocket , buffer, BUFFERSIZE);
        printf("%s\n", buffer);
        char *http = parseUrl(buffer);
        char *msg = getResponse(http);
        write(remoteSocket , msg , strlen(msg));
        printf("------------------Hello message sent-------------------");
        close(remoteSocket);
    }
    return 0;
}

char *getResponse(char *http) {
    char *response = malloc(sizeof(char) * MAX_RESPONSE_SIZE);
    char *msgBody = getMsgBody(http);
    if(msgBody == NULL){
        return getNotValid();
    }
    sprintf(response, "HTTP/1.1 200 OK"
                          "\nContent-Type: text/plain"
                          "\nContent-Length: %lu\n\n%s", strlen(msgBody), msgBody);
    free(msgBody);
    free(http);
    return response;
}

char *getMsgBody(const char *http) {
    char *msgBody;
    if(strcmp(http, "/hostname") == 0){
        msgBody = getUserName();
    }else if(strcmp(http, "/cpu-name") == 0){
        msgBody = getCpuName();
    }else if(strcmp(http, "/load") == 0){
        msgBody = getLoad();
    }else{
        return NULL;
    }
    return msgBody;
}

char *getNotValid() {
    return "HTTP/1.1 400 Bad Request"
           "\nContent-Type: text/plain"
           "\nContent-Length: %lu\n\n"
           "Url does not exist!";
}

char *getLoad() {
    /*
    PrevIdle = previdle + previowait
    Idle = idle + iowait

    PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
    NonIdle = user + nice + system + irq + softirq + steal

    PrevTotal = PrevIdle + PrevNonIdle
    Total = Idle + NonIdle

# differentiate: actual value minus the previous one
    totald = Total - PrevTotal
    idled = Idle - PrevIdle

    CPU_Percentage = (totald - idled)/totald
            */
    return "666%\n";
}

char *getCpuName() {
    FILE* file = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk '...", "r");
    char* name = (char*) malloc(sizeof(MAX_RESPONSE_SIZE) * sizeof(char ));
    fgets(name, MAX_RESPONSE_SIZE, file);
    return name;
}

char *getUserName() {
    return "/merlin/fadopyco\n";
}

char *parseUrl(const char *string){
    size_t len = strlen(string);
    if(len < 10){
        failure("Failed parsing HTTP header.");
    }

    // Check starting with 'GET '
    char *pre = "GET ";
    char actualPre[5];
    memcpy( actualPre, string, 4);
    actualPre[4] = '\0';
    if(strcmp(pre, actualPre) != 0){
        return "not GET header";
    }

    // get HTTP substring
    size_t lenToSpace = 0;
    size_t init = 4;
    while(string[init + lenToSpace] != ' '){
        lenToSpace++;
    }

    int maxHttpSize = 100;
    char *http = (char*)calloc( maxHttpSize, sizeof (char));
    memcpy( http, &string[init], lenToSpace);
    return http;
}
int acceptConnection(int server_fd, struct sockaddr_in *address, int *addrlen) {
    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *) address, (socklen_t*) addrlen)) < 0)
    {
        failure("Failure in accepting new socket!");
    }
    return new_socket;
}

void startListening(int server_fd) {
    if (listen(server_fd, MAXPENDING) < 0)
    {
        failure("Failure listeing!");
    }
}

struct sockaddr_in createAdr(int port, struct sockaddr_in *address) {
    (*address).sin_family = AF_INET;
    (*address).sin_addr.s_addr = INADDR_ANY;
    (*address).sin_port = htons(port );
    memset((*address).sin_zero, '\0', sizeof (*address).sin_zero);

    return (*address);
}

struct sockaddr_in bindAdr(int server_fd, struct sockaddr_in *address) {
    if (bind(server_fd, (struct sockaddr *) address, sizeof((*address))) < 0)
    {
        failure("In bind!");
    }
    return (*address);
}

int getSocket() {
    int server_fd;// Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        failure("In socket");
    }
    return server_fd;
}

int parseArgs(int argc, const char *const *argv) {
    if(argc < 2){
        failure("No port specified!");
    }

    int port;
    if((port = tryParsePort(argv[1])) == 0){
        failure("Invalid port provided!");
    }
    return port;
}

int tryParsePort(const char *string) {
    int port;
    if((port = atoi(string)) == 0){
        failure("Second argument is not an integer!");
    }
    if(port < 1024 || port > 65535){
        failure("Port number out of range!\nValid range is <1024, 65535>.");
    }
    return port;
}
