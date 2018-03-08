
struct print_request {
    int size;
    int userId;
} printRequest;

void signal_handler(int signo);

int printThreadFunc();

int userThreadFunc();
