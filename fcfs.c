#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20


int total = 0;

typedef struct process *process_node;

struct process {
	int process_id;						// 삽입되는 프로세스의 id
	int priority;					    // 프로세스의 priority 프로세스 교체전략에서는 큰 의미 없다
	int computing_time;				    // 프로세스가 실행되는 시간
	int turn_around_time;			    // 프로세스가 들어온 다음부터 종료될 때 까지의 총 시간
	int in_time;                        // 프로세스가 들어온시간, 현재시간 - in_time 으로 프로세스의 turn_around_time을 계산할 수 있다.
	process_node left_link, right_link;
};

typedef struct process_head *head_node;
struct process_head {                    // 프로세스가 처음 삽입되면 head->right_link에 연결된다. process들을 연결해주는 리스트의 시작부분이다.
	process_node left_link;
	process_node right_link;
};

head_node phead;

void error(char *message)                // 에러메세지 출력을 위한 함수
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

void initialize_process(void)                            // 프로세스를 삽입하기 위해서 초기화 되어야 하는 부분을 표현함 함수
{
	phead = (head_node)malloc(sizeof(head_node));        // 최초에 프로세스를 생성하기 위해서는 process를 연결해주는 시작부분 head의 메모리를 동적할당 해 주어야 한다.
	phead->left_link = NULL;
	phead->right_link = NULL;
}

void insert_process(int process_id, int priority, int computing_time) {             // 프로세스 삽입함수 fcfs 전략에 따라 먼저 들어온 프로세스가 먼저 스케줄되는 형태의 함수이다.
	process_node node = (process_node)malloc(sizeof(process_node) * 7);
	process_node temp = NULL;
	if (node == NULL) printf("메모리할당에러");
	node->process_id = process_id;
	node->priority = priority;
	node->computing_time = computing_time;
	node->turn_around_time = computing_time;
	node->in_time = total;
	node->left_link = NULL;
	node->right_link = NULL;

	temp = phead->right_link;

	if (temp == NULL) {                                // 삽입된 프로세스는 먼저 삽입된 순서대로 스케줄링 된다.
		phead->right_link = node;
		node->left_link = phead;

	}
	else {
		while (temp->right_link != NULL) {
			temp = temp->right_link;
		}
		temp->right_link = node;
		node->left_link = temp;
	}
}


void time_quantum() {                     // TIME_QUANTUM만큼시간이 지남을 표현해주는 함수 fcfs 알고리즘에서 프로세스의 교체에 영향을 주지는 않는다
	process_node removed = (process_node)malloc(sizeof(process_node));
	int temp = TIME_QUANTUM;
	int prev = temp;
	temp = TIME_QUANTUM - phead->right_link->turn_around_time;

	if (removed == NULL) printf("메모리할당실패");
	if (phead->right_link == NULL) {
		printf("프로세스가 없습니다.");
	}

	while (temp >= 0) {                               // fcfs 알고리즘의 퍼포먼스에 영향을 미치지는 않지만 시간할당량 만큼 시간이 지남을 나타내주고
		removed = phead->right_link;				  // 시간할당량만큼 시간이 지나는동안 현재 CPU를 선점하고 있는 프로세스의 computing_time이 전부 소모되어 종료될 경우
		total += removed->turn_around_time;           // 프로세스의 모든 정보를 화면에 출력시켜주고 해당 프로세스는 종료된다.
		removed->turn_around_time = total - removed->in_time;
		printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
		phead->right_link = removed->right_link;
		removed->right_link->left_link = phead;
		if (temp > 0) {
			prev = temp;
			temp -= phead->right_link->turn_around_time;
		}
		else {
			break;
		}
	}
	if (temp < 0) {
		phead->right_link->turn_around_time -= prev;
		total += prev;
	}
}

void print_schedule(void) {                // 현재 큐에 들어있는 모든 프로세스들이 종료될 때 까지 스케줄링을 반복한다. 프로세스가 종료되면 출력해준다.
	process_node node2 = NULL;
	node2 = phead->right_link;

	while (node2 != NULL) {                // fcfs 알고리즘에 따라 먼저 삽입된 프로세스 먼저 종료가 되는데, 마지막 프로세스가 종료되기 전까지 반복문을 실시하여 
		total += node2->turn_around_time;  // 프로세스의 turn_around_time을 계산해주고 화면에 프로세스 정보를 출력한 뒤 프로세스를 종료한다.
		node2->turn_around_time = total - node2->in_time;
		printf("%2d %12d %10d %17d\n", node2->process_id, node2->priority, node2->computing_time, node2->turn_around_time);
		node2 = node2->right_link;
	}
}

int main()
{
	FILE* File_Stream = fopen("input_file.txt", "r");
	int type = 0;
	int p_id;
	int pri;
	int com_time;

	if (File_Stream == NULL) {
		printf("파일을 열지 못하였습니다.");
		exit(1);
	}
	initialize_process();
	printf("Process ID   priority   computing_time   turn_around_time\n");

	while (type != -1) {
		fscanf(File_Stream, "%d %d %d %d", &type, &p_id, &pri, &com_time);
		switch (type)
		{
		case 0:
			insert_process(p_id, pri, com_time);
			break;
		case 1:
			time_quantum();
			break;
		case -1:
			print_schedule();
			break;
		default:
			printf("잘못된 type의 값입니다.\n");
			return 0;
		}
	}

	if (fclose(File_Stream) == EOF) {
		printf("파일을 닫지 못하였습니다.");
	}
	return 0;

}