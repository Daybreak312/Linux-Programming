#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include "logger.h"

#define MAX_BLOCKS 10
#define MAX_PARAMS 3
#define NAME_SIZE 50
#define PARAM_SIZE 256
#define REASON_SIZE 256

#define BUFFER_SIZE 1024

#define DEFAULT_PROCESS_FILE "./process.out"
#define SWBLOCKS_INFO_FILE "./swblocks.txt"

struct SwInfo {
    char name[NAME_SIZE];
    int pid;
    char params[MAX_PARAMS][PARAM_SIZE];
    int paramCount;
    int restartCount;
    char reason[REASON_SIZE];
    time_t startTime;
};

struct SwInfo blocks[MAX_BLOCKS];
int blockCount = 0;

// 함수 선언

// 0. 프로그램 초기화
// 0.1 로그 파일을 담을 디렉토리 생성
void createLogDirectory() {
    struct stat st = {0};
    if (stat("./log", &st) == -1) {  // 'log' 디렉터리 존재 확인
        mkdir("./log", 0777); // 존재하지 않으면 생성
    }
}

// 0.2 자식 프로세스 종료 시그널 핸들러
void sigchldHandler(int sig);

// 1. S/W 블록 읽기 관련 함수들
// 1.1 텍스트 파일 읽기
void readFileList(const char *fileName);

// 1.2 텍스트 파일의 각 라인을 처리해 S/W 블록 정보 읽기
void settingSWBlock(char *line);

// 2. S/W 블록 초기화 관련 함수들
// 2.1 S/W 블록 정보들을 기반으로 프로세스 초기화 및 실행
void initializeProcesses();

void initializeProcess(struct SwInfo *block);

// 2.2 execv 함수 실행 시에 S/W 블록 정보를 매개변수(exev 함수의 argv)로 직렬화
// { block->name, [block->parameters], NULL } 로 구성됨.
void serializeArguments(char *args[PARAM_SIZE + 2], struct SwInfo *block);

// 3. S/W 블록 재초기화
void restartProcess(struct SwInfo *block, char *reasonStr);

// 4. S/W 블록 정보 출력
void printSWBlocksInfo();

// 5. 프로세스 모니터링
void monitorProcess(struct SwInfo *block, pid_t pid);

// 6. 유틸리티 함수들
// 6.1 데몬 전환
void daemonize();

// 7. 에러 관련 함수들
// 7.1 에러 메세지를 출력하고 상태 '1'로 종료
void exitErrorMessage(char *message) {
    error(message);
    exit(EXIT_FAILURE);
}

// 7.2 상태 '1'로 종료
void exitError() {
    exit(EXIT_FAILURE);
}

// 메인 함수
int main() {

    // 시그널 기반 자식 프로세스 관리
    signal(SIGCHLD, sigchldHandler);

    // 로그 파일을 저장할 디렉토리 생성
    createLogDirectory();

    // 현재 프로세스를 데몬으로 전환
    daemonize();

    // S/W 블록들을 읽어들임
    readFileList(SWBLOCKS_INFO_FILE);

    // 프로세스들 초기화
    initializeProcesses();

    // 계속 실행
    while (1) {
        sleep(10);
    }
}

void sigchldHandler(int sig) {
    int status;
    pid_t pid;

    // 비동기 시그널 핸들러에서 모든 자식 프로세스를 확인
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < blockCount; i++) {
            if (blocks[i].pid == pid) {  // 종료된 자식 프로세스 확인
                char buffer[BUFFER_SIZE];

                if (WIFEXITED(status)) {
                    sprintf(buffer, "Exit(%d)", WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    sprintf(buffer, "Signal(%d, %s)", WTERMSIG(status), strsignal(WTERMSIG(status)));
                } else {
                    sprintf(buffer, "Unknown(%d)", status);
                }

                restartProcess(&blocks[i], buffer); // 재시작 처리
                break;
            }
        }
    }
}

void readFileList(const char *fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) {
        exitErrorMessage("Fail to open file.");
    }

    // 파일 크기가 BUFFER_SIZE = 1024보다 작을 것이라 가정
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    if (bytesRead < 0) {
        close(fd);
        exitErrorMessage("Fail to read file.");
    }
    buffer[bytesRead] = '\0';

    char **status;
    char *line = strtok_r(buffer, "\n", status);
    while (line && blockCount < MAX_BLOCKS) {
        info("Reading line: %s", line);
        settingSWBlock(line);
        line = strtok_r(NULL, "\n", status);
    }
    close(fd);
}

void settingSWBlock(char *line) {
    struct SwInfo *block = &blocks[blockCount++];
    char **status;
    char *token = strtok_r(line, ";", status);
    strncpy(block->name, token, NAME_SIZE - 1);

    // i 변수를 반복문 외부에서 사용해야 하므로, for문보다 while문 코드가 더 예쁨
    int i = 0;
    while ((token = strtok_r(NULL, ";", status)) != NULL && i < MAX_PARAMS) {
        strncpy(block->params[i++], token, PARAM_SIZE - 1);
    }
    block->paramCount = i;
    block->restartCount = 0;
    block->reason[0] = '\0';
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) {
        printf("Daemon started. pid: %d\n", (int) pid);
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int logFd = open("/dev/null", O_RDWR);
    dup2(logFd, STDIN_FILENO);
    dup2(logFd, STDOUT_FILENO);
    dup2(logFd, STDERR_FILENO);
    if (logFd > 2) close(logFd);

    char buffer[20];
    getCurrentTimeStr(buffer);
    info("Daemon started at %s, pid: %d", buffer, (int) getpid());
}

void initializeProcesses() {

    for (int i = 0; i < blockCount; i++) {
        initializeProcess(&blocks[i]);
    }
}

void initializeProcess(struct SwInfo *block) {
    block->startTime = time(NULL);

    pid_t pid = fork();

    if (pid == 0) {

        char *args[MAX_PARAMS + 2];
        serializeArguments(args, block);

        execv(DEFAULT_PROCESS_FILE, args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        block->pid = pid;
        info("Starting process \"%s\", pid: %d", block->name, block->pid);
        return;
    }

    // exec 계열 함수가 호출 프로세스를 대체하지 못했거나,
    // fork()가 정상 실행되지 않음
    error("Fail to execute process(s/w block): %s", block->name);
    exit(EXIT_FAILURE);
}

void serializeArguments(char *args[MAX_PARAMS + 2], struct SwInfo *block) {
    args[0] = strdup(block->name);
    // 매개변수들 삽입
    for (int j = 0; j < block->paramCount; j++) {
        args[j + 1] = strdup(block->params[j]);
    }
    // 마지막 배열값은 NULL이어야 하므로 삽입
    args[block->paramCount + 1] = NULL;
}

void restartProcess(struct SwInfo *block, char *reasonStr) {
    block->restartCount++;
    strncpy(block->reason, reasonStr, REASON_SIZE - 1);
    block->reason[REASON_SIZE - 1] = '\0';

    info("Process \"%s\" was restarted. pid: %d", block->name, block->pid);

    printSWBlocksInfo();
    initializeProcess(block);
}

void printSWBlocksInfo() {
    // 쓰기 전용, 파일이 없을 경우 생성, 이어 쓰기, rw--w--w- 권한
    int fd = open("./log/restart.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        exitErrorMessage("Fail to open log file.");
    }

    // 로그
    info("print S/W Blocks information.");
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < blockCount; i++) {
        struct SwInfo *block = &blocks[i];
        char time[30];
        getCurrentTimeStr(time);

        // S/W 블록 정보를 버퍼에 작성
        int len = snprintf(buffer, sizeof(buffer), "%-15s %-15d %-25s %s\n",
                           block->name, block->restartCount, time, block->reason);

        // 로그로 출력
        info(buffer);
        // 파일로 출력
        if (write(fd, buffer, len) < 0) {
            close(fd);
            exitErrorMessage("Fail to write on log file.");
        }
    }

    if (write(fd, "\n\n", 2) < 0) {
        exitErrorMessage("Fail to write on log file.");
    }

    // 파일 닫기
    close(fd);
}
