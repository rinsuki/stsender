#include <alsa/asoundlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>

#define AUDIO_CHANNEL 2
#define AUDIO_RATE 48000
#define AUDIO_BIT 16
#define BYTES_PER_FRAME (AUDIO_CHANNEL * AUDIO_BIT / 8)
#define BYTES_PER_SEC (BYTES_PER_FRAME * AUDIO_RATE)
// 8ms buffer should be enough
#define BUFFER_MSEC 4
#define BUFFER_SIZE (BYTES_PER_SEC / 1000) * BUFFER_MSEC

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    if (argc < 2) {
        if (argc < 1) return 2;
        printf("Usage: %s <alsa device name>\n", argv[0]);
        return 1;
    }
    // listen 12345/tcp
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))<0) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(12345),
        .sin_addr = { .s_addr = INADDR_ANY },
    };
    
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(sock, 1) < 0) {
        perror("listen");
        return 1;
    }

    while (1) {
        printf("Waiting for connection...\n");
        int accept_sock = accept(sock, NULL, NULL);
        if (accept_sock < 0) {
            perror("accept");
            return 1;
        }
        printf("accepted\n");

        // 別に fork する必要はあんまりないんだけど
        // TCP 切られた時になんかしれっと死んでしまうので
        // fork して死んでも親プロセスが生きてるようにする

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        
        if (pid != 0) {
            waitpid(pid, NULL, 0);
            continue;
        }

        // init alsa
        snd_pcm_t *pcm;
        if (snd_pcm_open(&pcm, argv[1], SND_PCM_STREAM_CAPTURE, 0) < 0) {
            perror("snd_pcm_open");
            return 1;
        }

        if (snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, AUDIO_CHANNEL, AUDIO_RATE, 0, 1000) < 0) {
            perror("snd_pcm_set_params");
            return 1;
        }

        printf("BUFFER_SIZE: %d (%d frames)\n", BUFFER_SIZE, BUFFER_SIZE / BYTES_PER_FRAME);

        uint8_t buf[BUFFER_SIZE];

        while (1) {
            int readed = snd_pcm_readi(pcm, &buf, BUFFER_SIZE/BYTES_PER_FRAME);
            if (readed == -EPIPE) {
                int err = snd_pcm_recover(pcm, readed, 0);
                if (err < 0) {
                    printf("snd_pcm_recover: %d\n", err);
                    return 2;
                }
                usleep(1000);
                continue;
            }
            if (readed < 0) {
                printf("err: %d\n", readed);
                return 2;
            }
            // printf("readed: %d\n", readed);
            if (write(accept_sock, &buf, readed * BYTES_PER_FRAME) < 0) {
                perror("write");
                break;
            }
        }
        printf("close tcp\n");
        close(accept_sock);
        snd_pcm_close(pcm);
        break;
    }
}