#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20               //  �ð��Ҵ緮�� 20�̶�� ���� ��Ÿ����

int total = 0;

typedef struct process *process_node;

struct process {
	int process_id;						// ���ԵǴ� ���μ����� id
	int priority;					    // ���μ����� priority ���μ��� ��ü���������� ū �ǹ� ����
	int computing_time;				    // ���μ����� ����Ǵ� �ð�
	int turn_around_time;				// ���μ����� ���� �������� ����� �� ������ �� �ð�
	int remain_time;					// TIME_QUANTUM�� ������ ���μ����� ��ü�Ǵµ� ����Ǳ���� �����ִ� ��ǻ��Ÿ���� �󸶳� �Ǵ����� �����ϴ� ����
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

void insert_process(int process_id, int priority, int computing_time) {             // ���μ��� �����Լ� round robin ������ TIME_QUANTUM�� ������ ���μ����� ��ü�ϴ� ������ �Լ��̴�.
	process_node node = (process_node)malloc(sizeof(process_node) * 6);             // ���ο� ��带 �����ϰ� �޸𸮸� �Ҵ��Ѵ�. ���ο��忡 ���μ����� ���� ������ ����.
	process_node temp = NULL;
	if (node == NULL) printf("�޸��Ҵ翡��");
	node->process_id = process_id;
	node->priority = priority;
	node->computing_time = computing_time;
	node->turn_around_time = computing_time;
	node->remain_time = computing_time;
	node->in_time = total;
	node->left_link = NULL;
	node->right_link = NULL;

	temp = phead->right_link;

	if (temp == NULL) {                   // ����κ� �˰����� ��� ���μ����� �����ϴ� ��� FCFS �˰���� �����ϰ� ���� ���Ե� ���μ����� ���� ��ġ�Ѵ�.
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



void time_quantum() {                     // TIME_QUANTUM��ŭ�ð��� ������ ǥ�����ִ� �Լ�. cpu�� ����ִ� ���μ����� ť�� ���� �������� ������ �ǰ� ���ο� ���μ����� cpu�� ��Եȴ�.
	process_node removed = (process_node)malloc(sizeof(process_node));
	process_node temp2 = (process_node)malloc(sizeof(process_node));

	int temp;
	int prev;

	temp2 = phead->right_link;
	removed = phead->right_link;
	temp = TIME_QUANTUM;
	prev = removed->remain_time;

	if (removed == NULL) printf("�޸��Ҵ����");
	if (phead->right_link == NULL) {
		printf("���μ����� �����ϴ�.");
	}

	while (removed->remain_time - temp < 0) {            // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮��ŭ �ð��� ���� ��� ����Ǿ�� �Ѵٸ� ���μ����� �����ϰ� ȭ�鿡 ������ش�.
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
		if (removed->remain_time - temp > 0) {			   // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮 ��ŭ �ð��� ���� ���, ���μ����� computing_time�� ��� �Ҹ���� �ʰ�
			total += TIME_QUANTUM;						   // �����ִٸ� ����CPU�� �����ϰ� �ִ� ���μ����� ť�� ���������� �����ְ�, ���� ������ �� ������ ���μ����� CPU�� �����Ѵ�.
			removed->remain_time -= TIME_QUANTUM;
			removed->turn_around_time = total;
			if (removed->right_link == NULL) return;
			phead->right_link = removed->right_link;
			removed->right_link->left_link = phead;
			removed->right_link = NULL;
			removed->left_link = temp2;
			temp2->right_link = removed;
		}
		else {											  // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮��ŭ �ð��� ���� ���, ���μ����� computing_time�� �� �°� ��� �Ҹ��� ��� 
			removed->remain_time -= TIME_QUANTUM;		  // �ش� ���μ����� ������ ȭ�鿡 ������ش�.
			total += TIME_QUANTUM;
			removed->turn_around_time = total - removed->in_time;
			printf("%2d %12d %10d %17d\n", removed->process_id, removed->priority, removed->computing_time, removed->turn_around_time);
			phead->right_link = removed->right_link;
			removed->right_link->left_link = phead;
		}
	}
}

void print_schedule(void) {
	while (1) {							    	// ���� ť�� ����ִ� ��� ���μ������� ����� �� ���� round robin �˰����� �ݺ��ϰ� ���μ����� ����Ǹ� �ش� ���μ����� ������ش�.
		if (phead->right_link == NULL) break;   // �ð��Ҵ緮�� ������ ���� ��� ���μ����� ��ü�Ǵ� �˰����̹Ƿ� �ð��� �帧�� ���� �����ٸ��� �ٲ�� �ֵ��� �Լ��� �����Ͽ���
		process_node removed = (process_node)malloc(sizeof(process_node));
		process_node temp2 = (process_node)malloc(sizeof(process_node));

		int temp;
		int prev;

		temp2 = phead->right_link;
		removed = phead->right_link;
		temp = TIME_QUANTUM;
		prev = removed->remain_time;

		if (removed == NULL) printf("�޸��Ҵ����");
		if (phead->right_link == NULL) {
			printf("���μ����� �����ϴ�.");
		}

		while (removed->remain_time - temp < 0) {       // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮��ŭ �ð��� ���� ��� ����Ǿ�� �Ѵٸ� ���μ����� �����ϰ� ȭ�鿡 ������ش�.
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
			if (removed->remain_time - temp > 0) {         // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮 ��ŭ �ð��� ���� ���, ���μ����� computing_time�� ��� �Ҹ���� �ʰ�
				total += TIME_QUANTUM;					   // �����ִٸ� ����CPU�� �����ϰ� �ִ� ���μ����� ť�� ���������� �����ְ�, ���� ������ �� ������ ���μ����� CPU�� �����Ѵ�.
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
			else {											 // ���� CPU�� �����ϰ� �ִ� ���μ����� �ð��Ҵ緮��ŭ �ð��� ���� ���, ���μ����� computing_time�� �� �°� ��� �Ҹ��� ���
				removed->remain_time -= TIME_QUANTUM;		 // �ش� ���μ����� ������ ȭ�鿡 ������ش�.
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
