#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20


int total = 0;

typedef struct process *process_node;

struct process {
	int process_id;						// ���ԵǴ� ���μ����� id
	int priority;					    // ���μ����� priority ���μ��� ��ü���������� ū �ǹ� ����
	int computing_time;				    // ���μ����� ����Ǵ� �ð�
	int turn_around_time;			    // ���μ����� ���� �������� ����� �� ������ �� �ð�
	int in_time;                        // ���μ����� ���½ð�, ����ð� - in_time ���� ���μ����� turn_around_time�� ����� �� �ִ�.
	process_node left_link, right_link;
};

typedef struct process_head *head_node;
struct process_head {                    // ���μ����� ó�� ���ԵǸ� head->right_link�� ����ȴ�. process���� �������ִ� ����Ʈ�� ���ۺκ��̴�.
	process_node left_link;
	process_node right_link;
};

head_node phead;

void error(char *message)                // �����޼��� ����� ���� �Լ�
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

void initialize_process(void)                            // ���μ����� �����ϱ� ���ؼ� �ʱ�ȭ �Ǿ�� �ϴ� �κ��� ǥ���� �Լ�
{
	phead = (head_node)malloc(sizeof(head_node));        // ���ʿ� ���μ����� �����ϱ� ���ؼ��� process�� �������ִ� ���ۺκ� head�� �޸𸮸� �����Ҵ� �� �־�� �Ѵ�.
	phead->left_link = NULL;
	phead->right_link = NULL;
}

void insert_process(int process_id, int priority, int computing_time) {             // ���μ��� �����Լ� fcfs ������ ���� ���� ���� ���μ����� ���� �����ٵǴ� ������ �Լ��̴�.
	process_node node = (process_node)malloc(sizeof(process_node) * 7);
	process_node temp = NULL;
	if (node == NULL) printf("�޸��Ҵ翡��");
	node->process_id = process_id;
	node->priority = priority;
	node->computing_time = computing_time;
	node->turn_around_time = computing_time;
	node->in_time = total;
	node->left_link = NULL;
	node->right_link = NULL;

	temp = phead->right_link;

	if (temp == NULL) {                                // ���Ե� ���μ����� ���� ���Ե� ������� �����ٸ� �ȴ�.
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


void time_quantum() {                     // TIME_QUANTUM��ŭ�ð��� ������ ǥ�����ִ� �Լ� fcfs �˰��򿡼� ���μ����� ��ü�� ������ ������ �ʴ´�
	process_node removed = (process_node)malloc(sizeof(process_node));
	int temp = TIME_QUANTUM;
	int prev = temp;
	temp = TIME_QUANTUM - phead->right_link->turn_around_time;

	if (removed == NULL) printf("�޸��Ҵ����");
	if (phead->right_link == NULL) {
		printf("���μ����� �����ϴ�.");
	}

	while (temp >= 0) {                               // fcfs �˰����� �����ս��� ������ ��ġ���� ������ �ð��Ҵ緮 ��ŭ �ð��� ������ ��Ÿ���ְ�
		removed = phead->right_link;				  // �ð��Ҵ緮��ŭ �ð��� �����µ��� ���� CPU�� �����ϰ� �ִ� ���μ����� computing_time�� ���� �Ҹ�Ǿ� ����� ���
		total += removed->turn_around_time;           // ���μ����� ��� ������ ȭ�鿡 ��½����ְ� �ش� ���μ����� ����ȴ�.
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

void print_schedule(void) {                // ���� ť�� ����ִ� ��� ���μ������� ����� �� ���� �����ٸ��� �ݺ��Ѵ�. ���μ����� ����Ǹ� ������ش�.
	process_node node2 = NULL;
	node2 = phead->right_link;

	while (node2 != NULL) {                // fcfs �˰��� ���� ���� ���Ե� ���μ��� ���� ���ᰡ �Ǵµ�, ������ ���μ����� ����Ǳ� ������ �ݺ����� �ǽ��Ͽ� 
		total += node2->turn_around_time;  // ���μ����� turn_around_time�� ������ְ� ȭ�鿡 ���μ��� ������ ����� �� ���μ����� �����Ѵ�.
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
		printf("������ ���� ���Ͽ����ϴ�.");
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
			printf("�߸��� type�� ���Դϴ�.\n");
			return 0;
		}
	}

	if (fclose(File_Stream) == EOF) {
		printf("������ ���� ���Ͽ����ϴ�.");
	}
	return 0;

}