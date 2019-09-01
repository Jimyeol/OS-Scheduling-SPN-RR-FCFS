#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20
#define TRUE 1


int total = 0;

typedef struct process *process_node;

struct process {
	int process_id;						// ���ԵǴ� ���μ����� id
	int priority;					    // ���μ����� priority ���μ��� ��ü���������� ū �ǹ� ����
	int computing_time;				    // ���μ����� ����Ǵ� �ð�
	int turn_around_time;			    // ���μ����� ���� �������� ����� �� ������ �� �ð�
	int have_process;					// cpu�� �Ҵ�ް� �ִ� ���μ����� �����Ҽ� �ְ� ���ִ� ����. �ش纯���� TRUE 1 �� �����Ǿ������� ����Ǳ� ���� ��ü���� �ʴ´�.
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

void insert_process(int process_id, int priority, int computing_time) {        // ���μ��� �����Լ� spn ������ ���� computing_time�� ���� ���μ����� ���� �����ٵǴ� ������ �Լ��̴�.
	process_node node = (process_node)malloc(sizeof(process_node) * 6);
	process_node temp = NULL;
	process_node prev = NULL;
	if (node == NULL) printf("�޸��Ҵ翡��");
	node->process_id = process_id;
	node->priority = priority;
	node->computing_time = computing_time;
	node->turn_around_time = computing_time;
	node->in_time = total;
	node->left_link = NULL;
	node->right_link = NULL;

	temp = phead->right_link;
	prev = temp;

	if (temp == NULL) {                            // ���ԵǾ��ִ� ���μ����� �ϳ��� ���� �� ���μ����� ť�� ���� �տ� �����Ѵ�.
		phead->right_link = node;
		node->left_link = phead;

	}
	else if (node->computing_time < temp->turn_around_time) {  // ť�� ���� ó���� �ִ� ���μ������� ���� �����ϴ� ���μ����� computing_time�� ���� ��쿡��
		if (temp->have_process == TRUE) {					   // ���� ó���� �ִ� ���μ����� CPU�� �����ϰ� �ִٸ� ���λ����ϴ� ���μ����� �ι�°�� ���Եȴ�.
			if (temp->right_link == NULL) {                    // ���� ó���� �ִ� ���μ����� CPU�� �����ϰ� ���� �ʴٸ� ���λ����ϴ� ���μ����� ť�� �� ������ ���Եȴ�.
				temp->right_link = node;
				node->left_link = temp;
			}
			else {
				temp->right_link->left_link = node;
				node->right_link = temp->right_link;
				temp->right_link = node;
				node->left_link = temp;
			}
		}
		else {
			phead->right_link = node;
			node->left_link = phead;
			node->right_link = temp;
			temp->left_link = node;
		}
	}
	else if (temp->turn_around_time <= node->computing_time) { // ���� �����ϴ� ���μ����� �̹� ���ԵǾ��ִ� ���μ������� computing_time�� ���ϴµ� computing_time�� ���� �������
		while (temp->right_link != NULL && (temp->right_link->turn_around_time <= node->computing_time)) { // ť�� ���ʿ� ���� �ǵ��� �Ѵ�.
			prev = temp;
			temp = temp->right_link;
		}
		if (temp->right_link != NULL && temp->right_link->computing_time > node->computing_time && (temp->turn_around_time <= node->computing_time)) {
			temp->right_link->left_link = node;
			node->right_link = temp->right_link;
			node->left_link = temp;
			temp->right_link = node;
		}
		else {
			node->left_link = temp;
			temp->right_link = node;
		}
	}

}


void time_quantum() {                     // TIME_QUANTUM��ŭ�ð��� ������ ǥ�����ִ� �Լ�. �ð��Ҵ緮��ŭ �ð��� �������� spn �˰��򿡼� ���μ����� ��ü�� ������ ������ �ʴ´�
	process_node removed = (process_node)malloc(sizeof(process_node));
	int temp = TIME_QUANTUM;
	int prev = temp;
	temp = TIME_QUANTUM - phead->right_link->turn_around_time;

	if (removed == NULL) printf("�޸��Ҵ����");
	if (phead->right_link == NULL) {
		printf("���μ����� �����ϴ�.");
	}

	while (temp >= 0) {                // spn�� CPU�� ������ ���μ����� ����Ǳ� ������ ��ü���� �ʴ� �˰����̴�. �ð��Ҵ緮�� ���������� ���� CPU�� �����ϰ� �ִ� ���μ�����
		removed = phead->right_link;   // ����� ��� ���μ����� turn_around_time�� ������ְ� ȭ�鿡 ���μ��� ������ ����� �� ���μ����� �����Ѵ�.
		total += removed->turn_around_time;
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
		phead->right_link->have_process = TRUE;
	}
}

void print_schedule(void) {                // ���� ť�� ����ִ� ��� ���μ������� ����� �� ���� �����ٸ��� �ݺ��Ѵ�. ���μ����� ����Ǹ� ������ش�.
	process_node node2 = NULL;
	node2 = phead->right_link;

	while (node2 != NULL) {                 // spn�� CPU�� ������ ���μ����� ����Ǳ� ������ ��ü���� �ʴ� �˰����̴�. fcfs�� �����ϰ�, ������ ���μ����� ����Ǳ� ������ �ݺ����� �ǽ��Ͽ�
		total += node2->turn_around_time;   // ���μ����� turn_around_time�� ������ְ� ȭ�鿡 ���μ��� ������ ����� �� ���μ����� �����Ѵ�.
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