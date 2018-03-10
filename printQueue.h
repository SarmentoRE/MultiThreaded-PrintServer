
struct printRequest {
    int size;
    int userId;
};

void signal_handler(int signo);

void* printThreadFunc();

void* userThreadFunc();

void addJob(int size, int threadNum);

void removeJob(int index);
