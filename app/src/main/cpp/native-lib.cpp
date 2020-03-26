#include <jni.h>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define DEFAULT_IPADDR "192.168.0.101"
#define DEFAULT_PORT "54213"

extern "C" JNIEXPORT jstring JNICALL
Java_com_jox0_simpleclient_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    int sockfd;
    int numbytes;
    char buf[512];
    struct addrinfo hints, *serverInfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int iResult;
    iResult = getaddrinfo(DEFAULT_IPADDR, DEFAULT_PORT, &hints, &serverInfo);
    if (iResult != 0) {
        std::string estr = "getaddrinfo:" + std::string(gai_strerror(iResult));
        return env->NewStringUTF(estr.c_str());
    }

    for(p = serverInfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }
    freeaddrinfo(serverInfo);
    if (p == NULL) {
        close(sockfd);
        return env->NewStringUTF("connect failed");
    }

    iResult = send(sockfd, "hello, server", 13, 0);
    if (iResult == -1) {
        close(sockfd);
        return env->NewStringUTF("send failed");
    }
    numbytes = recv(sockfd, buf, 511, 0);
    if (numbytes == -1) {
        close(sockfd);
        return env->NewStringUTF("recv failed");
    }
    close(sockfd);
    buf[numbytes] = '\0';

    return env->NewStringUTF(buf);
}