#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

#define MAX_BLOCKS 10
#define MAX_PARAMS 3
#define NAME_SIZE 50
#define PARAM_SIZE 256
#define REASON_SIZE 256
#define BUFFER_SIZE 1024

#define DEFAULT_PROCESS_FILE "./process"

struct SwInfo {
    char name[NAME_SIZE];
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
        mkdir("./log", 0700); // 존재하지 않으면 생성
    }
}

// 1. S/W 블록 읽기 관련 함수들
// 1.1 텍스트 파일 읽기
void readFileList(const char *fileName);

// 1.2 텍스트 파일의 각 라인을 처리해 S/W 블록 정보 읽기
void settingSWBlock(char *line);

// 2. S/W 블록 초기화 관련 함수들
// 2.1 S/W 블록 정보들을 기반으로 프로세스 초기화 및 실행
void initializeProcesses();

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

// 6.2 현재 시간을 yyyy.MM.dd. hh.mm.ss 형식으로 제공
char *getCurrentTimeStr();

// 7. 에러 관련 함수들
// 7.1 에러 메세지를 출력하고 상태 '1'로 종료
void exitErrorMessage(char *message) {
    printf("Error: %s\n", message);
    exit(1);
}

// 7.2 상태 '1'로 종료
void exitError() {
    exit(1);
}

// 메인 함수
int main() {

    // 로그 파일을 저장할 디렉토리 생성
    createLogDirectory();

    // 현재 프로세스를 데몬으로 전환
    daemonize();

    // S/W 블록들을 읽어들임
    readFileList("swblocks.txt");

    // 프로세스들 초기화
    initializeProcesses();

    // 데모닝
    while (1) {
        sleep(10);
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

    char *line = strtok(buffer, "\n");
    while (line && blockCount < MAX_BLOCKS) {
        settingSWBlock(line);
        line = strtok(NULL, "\n");
    }
    close(fd);
}

void settingSWBlock(char *line) {
    struct SwInfo *block = &blocks[blockCount++];
    char *token = strtok(line, ";");
    strncpy(block->name, token, NAME_SIZE - 1);

    // i 변수를 반복문 외부에서 사용해야 하므로, for문보다 while문 코드가 더 예쁨
    int i = 0;
    while ((token = strtok(NULL, ";")) != NULL && i < MAX_PARAMS) {
        strncpy(block->params[i++], token, PARAM_SIZE - 1);
    }
    block->paramCount = i;
    block->restartCount = 0;
    block->reason[0] = '\0';
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    setsid();
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void initializeProcesses() {

    for (int i = 0; i < blockCount; i++) {
        struct SwInfo *block = &blocks[i];
        block->startTime = time(NULL);

        pid_t pid = fork();

        if (pid == 0) {

            char *args[MAX_PARAMS + 2];
            serializeArguments(args, block);

            execv(DEFAULT_PROCESS_FILE, args);

            // exec 계열 함수가 호출 프로세스를 대체하지 못했으므로 실행 실패
            exitErrorMessage("Fail to execute process(s/w block).");
        } else if (pid > 0) {
            monitorProcess(block, pid);
        }
    }
}

void serializeArguments(char *args[PARAM_SIZE + 2], struct SwInfo *block) {
    args[0] = block->name;
    // 매개변수들 삽입
    for (int j = 0; j < block->paramCount; j++) {
        args[j + 1] = block->params[j];
    }
    // 마지막 배열값은 NULL이어야 하므로 삽입
    args[block->paramCount + 1] = NULL;
}

void monitorProcess(struct SwInfo *block, pid_t pid) {
    int status;
    waitpid(pid, &status, 0);
    char buffer[BUFFER_SIZE];
    if (WIFEXITED(status)) {
        sprintf(buffer, "Exit(%d)", WEXITSTATUS(status));
        restartProcess(block, buffer);
    } else if (WIFSIGNALED(status)) {
        sprintf(buffer, "Signal(%d, %s)", WTERMSIG(status), strsignal(WTERMSIG(status)));
        restartProcess(block, buffer);
    }
}

void restartProcess(struct SwInfo *block, char *reasonStr) {
    block->restartCount++;
    strncpy(block->reason, reasonStr, REASON_SIZE - 1);
    block->reason[REASON_SIZE - 1] = '\0';
    printSWBlocksInfo();
    initializeProcesses();
}

void printSWBlocksInfo() {
    // 쓰기 전용, 파일이 없을 경우 생성, 덮어 쓰기, rw--w--w- 권한
    int fd = open("./log/restart.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        exitErrorMessage("Fail to open log file.");
    }

    char buffer[BUFFER_SIZE];
    for (int i = 0; i < blockCount; i++) {
        struct SwInfo *block = &blocks[i];

        // 로그 내용을 버퍼에 작성
        int len = snprintf(buffer, sizeof(buffer), "%-15s %-15d %-25s %s\n",
                           block->name, block->restartCount, getCurrentTimeStr(), block->reason);

        // 버퍼 내용을 파일에 쓰기
        if (write(fd, buffer, len) < 0) {
            close(fd);
            exitErrorMessage("Fail to write to log file.");
        }
    }

    // 파일 닫기
    close(fd);
}

char *getCurrentTimeStr() {
    static char timeStr[30];
    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y.%m.%d %H:%M:%S", timeInfo);
    return timeStr;
}
