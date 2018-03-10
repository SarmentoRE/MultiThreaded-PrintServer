
struct printRequest {
    int size;
    int userId;
};

void signal_handler(int signo);

int printThreadFunc();

int userThreadFunc();
