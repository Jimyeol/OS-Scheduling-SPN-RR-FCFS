#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20               //  시간할당량이 20이라는 것을 나타낸다

int total = 0;

typedef struct process *process_node;

struct process {
	int process_id;						// 삽입되는 프로세스의 id
	int priority;					    // 프로세스의 priority 프로세스 교체전략에서는 큰 의미 없다
	int computing_time;				    // 프로세스가 실행되는 시간
	int turn_around_time;				// 프로세스가 들어온 다음부터 종료될 때 까지의 총 시간
	int remain_time;					// TIME_QUANTUM이 지나면 프로세스가 교체되는데 종료되기까지 남아있는 컴퓨팅타임이 얼마나 되는지를 저장하는 변수
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

void insert_process(int process_id, int priority, int computing_time) {             // 프로세스 삽입함수 round robin 전략은 TIME_QUANTUM이 지나면 프로세스를 교체하는 형태의 함수이다.
	process_node node = (process_node)malloc(sizeof(process_node) * 6);             // 새로운 노드를 생성하고 메모리를 할당한다. 새로운노드에 프로세스에 대한 정보가 들어간다.
	process_node temp = NULL;
	if (node == NULL) printf("메모리할당에러");
	node->process_id = process_id;
	node->priority = priority;
	node->computing_time = computing_time;
	node->turn_around_time = computing_time;
	node->remain_time = computing_time;
	node->in_time = total;
	node->left_link = NULL;
	node->right_link = NULL;

	temp = phead->right_link;

	if (temp == NULL) {                   // 라운드로빈 알고리즘의 경우 프로세스를 삽입하는 경우 FCFS 알고리즘과 동일하게 먼저 삽입된 프로세스를 먼저 배치한다.
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



void time_quantum() {                     // TIME_QUANTUM만큼시간이 지남을 표현해주는 함수. cpu를 잡고있는 프로세스는 큐에 가장 마지막에 스케줄 되고 새로운 프로세스가 cpu를 잡게된다.
	process_node removed = (process_node)malloc(sizeof(process_node));
	process_node temp2 = (process_node)malloc(sizeof(process_node));

	int temp;
	int prev;

	temp2 = phead->right_link;
	removed = phead->right_link;
	temp = TIME_QUANTUM;
	prev = removed->remain_time;

	if (removed == NULL) printf("메모리할당실패");
	if (phead->right_link == NULL) {
		printf("프로세스가 없습니다.");
	}

	while (removed->remain_time - temp < 0) {            // 현재 CPU를 선점하고 있는 프로세스가 시간할당량만큼 시간이 지난 결과 종료되어야 한다면 프로세스를 종료하고 화면에 출력해준다.
		total += prev;
		removed->turn_around_time = total - removed->in_time;
		printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
		phead->right_link = removed->right_link;
		removed->right_link->left_link = phead;
		removed = phead->right_link;
		temp = TIME_QUANTUM;
	}

	if (removed->remain_time - temp >= 0) {         
		while (temp2->right_link != NULL) {              
			temp2 = temp2->right_link;
		}
		if (removed->remain_time - temp > 0) {			   // 현재 CPU를 선점하고 있는 프로세스가 시간할당량 만큼 시간이 지난 결과, 프로세스의 computing_time이 모두 소모되지 않고
			total += TIME_QUANTUM;						   // 남아있다면 현재CPU를 선점하고 있는 프로세스를 큐의 마지막으로 보내주고, 다음 스케줄 될 예정인 프로세스가 CPU를 선점한다.
			removed->remain_time -= TIME_QUANTUM;
			removed->turn_around_time = total;
			if (removed->right_link == NULL) return;
			phead->right_link = removed->right_link;
			removed->right_link->left_link = phead;
			removed->right_link = NULL;
			removed->left_link = temp2;
			temp2->right_link = removed;
		}
		else {											  // 현재 CPU를 선점하고 있는 프로세스가 시간할당량만큼 시간이 지난 결과, 프로세스의 computing_time을 딱 맞게 모두 소모할 경우 
			removed->remain_time -= TIME_QUANTUM;		  // 해당 프로세스의 정보를 화면에 출력해준다.
			total += TIME_QUANTUM;
			removed->turn_around_time = total - removed->in_time;
			printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
			phead->right_link = removed->right_link;
			removed->right_link->left_link = phead;
		}
	}
}

void print_schedule(void) {
	while (1) {							    	// 현재 큐에 들어있는 모든 프로세스들이 종료될 때 까지 round robin 알고리즘을 반복하고 프로세스가 종료되면 해당 프로세스를 출력해준다.
		if (phead->right_link == NULL) break;   // 시간할당량이 지남에 따라 계속 프로세스가 교체되는 알고리즘이므로 시간의 흐름에 따라 스케줄링이 바뀔수 있도록 함수를 구성하였다
		process_node removed = (process_node)malloc(sizeof(process_node));
		process_node temp2 = (process_node)malloc(sizeof(process_node));

		int temp;
		int prev;

		temp2 = phead->right_link;
		removed = phead->right_link;
		temp = TIME_QUANTUM;
		prev = removed->remain_time;

		if (removed == NULL) printf("메모리할당실패");
		if (phead->right_link == NULL) {
			printf("프로세스가 없습니다.");
		}

		while (removed->remain_time - temp < 0) {       // 현재 CPU를 선점하고 있는 프로세스가 시간할당량만큼 시간이 지난 결과 종료되어야 한다면 프로세스를 종료하고 화면에 출력해준다.
			total += prev;									 
			removed->turn_around_time = total - removed->in_time;
			printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
			if (removed->right_link == NULL) {
				phead->right_link = NULL;
				break;
			}
			phead->right_link = removed->right_link;
			removed->right_link->left_link = phead;
			removed = phead->right_link;
			temp = TIME_QUANTUM;
		}

		if (removed->remain_time - temp >= 0) {
			while (temp2->right_link != NULL) {
				temp2 = temp2->right_link;
			}
			if (removed->remain_time - temp > 0) {         // 현재 CPU를 선점하고 있는 프로세스가 시간할당량 만큼 시간이 지난 결과, 프로세스의 computing_time이 모두 소모되지 않고
				total += TIME_QUANTUM;					   // 남아있다면 현재CPU를 선점하고 있는 프로세스를 큐의 마지막으로 보내주고, 다음 스케줄 될 예정인 프로세스가 CPU를 선점한다.
				removed->remain_time -= TIME_QUANTUM;
				removed->turn_around_time = total;
				if (removed->right_link == NULL) {
					removed->turn_around_time = removed->turn_around_time + removed->remain_time - removed->in_time;
					printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
					return;
				}
				phead->right_link = removed->right_link;
				removed->right_link->left_link = phead;
				removed->right_link = NULL;
				removed->left_link = temp2;
				temp2->right_link = removed;
			}
			else {											 // 현재 CPU를 선점하고 있는 프로세스가 시간할당량만큼 시간이 지난 결과, 프로세스의 computing_time을 딱 맞게 모두 소모할 경우
				removed->remain_time -= TIME_QUANTUM;		 // 해당 프로세스의 정보를 화면에 출력해준다.
				total += TIME_QUANTUM;
				removed->turn_around_time = total - removed->in_time;
				printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
				if (removed->right_link == NULL) {
					return;
				}
				phead->right_link = removed->right_link;
				removed->right_link->left_link = phead;
			}
		}
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
