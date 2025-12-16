#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "../students.dat"

#pragma pack(push, 1)
typedef struct {
    char name[52];
    int  kor;
    int  eng;
    int  math;
    int  total;
    float avg;
    unsigned char pad[4];
} Student;
#pragma pack(pop)

typedef struct Node {
    Student s;
    struct Node* next;
} Node;

/* ---------- Utility ---------- */
void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void free_list(Node** head, Node** tail) {
    Node* p = *head;
    while (p) {
        Node* next = p->next;
        free(p);
        p = next;
    }
    *head = *tail = NULL;
}

void add_student(Node** head, Node** tail, Student s) {
    Node* n = malloc(sizeof(Node));
    n->s = s;
    n->next = NULL;

    if (*head == NULL)
        *head = *tail = n;
    else {
        (*tail)->next = n;
        *tail = n;
    }
}

int list_count(Node* head) {
    int c = 0;
    while (head) {
        c++;
        head = head->next;
    }
    return c;
}

/* ---------- File I/O ---------- */
void load_dat(Node** head, Node** tail) {
    FILE* fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        printf("파일 열기 실패\n");
        return;
    }

    free_list(head, tail);

    Student s;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        add_student(head, tail, s);
    }

    fclose(fp);
    printf("읽기 완료: %d명\n", list_count(*head));
}

void save_dat(Node* head) {
    FILE* fp = fopen(DATA_FILE, "wb");
    if (!fp) {
        printf("저장 실패\n");
        return;
    }

    while (head) {
        fwrite(&head->s, sizeof(Student), 1, fp);
        head = head->next;
    }

    fclose(fp);
    printf("저장 완료\n");
}

/* ---------- Input ---------- */
Student input_student(void) {
    Student s;
    memset(&s, 0, sizeof(s));

    printf("이름: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("국어: "); scanf("%d", &s.kor);
    printf("영어: "); scanf("%d", &s.eng);
    printf("수학: "); scanf("%d", &s.math);
    flush_stdin();

    s.total = s.kor + s.eng + s.math;
    s.avg = s.total / 3.0f;

    memset(s.pad, 0xCC, sizeof(s.pad));
    return s;
}

/* ---------- Report ---------- */
int cmp(const void* a, const void* b) {
    Student* s1 = (Student*)a;
    Student* s2 = (Student*)b;
    return s2->total - s1->total;
}

void print_report(Node* head) {
    int n = list_count(head);
    if (n == 0) {
        printf("데이터 없음\n");
        return;
    }

    Student* arr = malloc(sizeof(Student) * n);
    Node* p = head;
    for (int i = 0; i < n; i++) {
        arr[i] = p->s;
        p = p->next;
    }

    qsort(arr, n, sizeof(Student), cmp);

    printf("\n[성적표]\n");
    printf("-------------------------------------------------------------\n");
    printf("등수  이름                         국어 영어 수학 총점 평균\n");
    printf("-------------------------------------------------------------\n");

    int rank = 1;
    for (int i = 0; i < n; i++) {
        if (i > 0 && arr[i].total < arr[i - 1].total)
            rank = i + 1;

        printf("%-4d %-28s %4d %4d %4d %4d %6.2f\n",
            rank,
            arr[i].name,
            arr[i].kor,
            arr[i].eng,
            arr[i].math,
            arr[i].total,
            arr[i].avg);
    }

    printf("-------------------------------------------------------------\n\n");
    free(arr);
}

/* ---------- Main ---------- */
int main(void) {
    Node* head = NULL;
    Node* tail = NULL;
    int sel;

    while (1) {
        printf("[Menu]\n");
        printf("1. .dat 파일에서 데이터 읽기\n");
        printf("2. 추가 학생 정보 입력\n");
        printf("3. .dat 파일 저장\n");
        printf("4. 성적 확인\n");
        printf("5. 종료\n");
        printf("-------------------\n");
        printf("선택(1~5): ");

        scanf("%d", &sel);
        flush_stdin();

        switch (sel) {
        case 1: load_dat(&head, &tail); break;
        case 2: add_student(&head, &tail, input_student()); break;
        case 3: save_dat(head); break;
        case 4: print_report(head); break;
        case 5:
            free_list(&head, &tail);
            return 0;
        default:
            printf("잘못된 선택\n");
        }
    }
}
