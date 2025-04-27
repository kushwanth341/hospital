#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define MAX_DEPTS 20

typedef struct {
    char name[50];
    int age;
    int priority;
} Patient;

typedef struct {
    char deptName[30];
    Patient patients[MAX];
    int front, rear;
} Queue;

Queue departments[MAX_DEPTS];
int dept_count = 0;
const char *filename = "hospital_data.txt";

void initQueue(Queue *q, const char *name) {
    strcpy(q->deptName, name);
    q->front = q->rear = -1;
}

int isFull(Queue *q) {
    return (q->rear + 1) % MAX == q->front;
}

int isEmpty(Queue *q) {
    return q->front == -1;
}

void enqueue(Queue *q, char name[], int age, int priority) {
    if (isFull(q)) return;
    if (isEmpty(q)) q->front = 0;
    q->rear = (q->rear + 1) % MAX;
    strcpy(q->patients[q->rear].name, name);
    q->patients[q->rear].age = age;
    q->patients[q->rear].priority = priority;
}

Patient dequeue(Queue *q) {
    Patient p = {"", -1, -1};
    if (isEmpty(q)) return p;
    int highIndex = q->front;
    int i = q->front;
    while (1) {
        if (q->patients[i].priority > q->patients[highIndex].priority)
            highIndex = i;
        if (i == q->rear) break;
        i = (i + 1) % MAX;
    }
    p = q->patients[highIndex];
    int next = (highIndex + 1) % MAX;
    while (highIndex != q->rear) {
        q->patients[highIndex] = q->patients[next];
        highIndex = next;
        next = (next + 1) % MAX;
    }
    q->rear = (q->rear - 1 + MAX) % MAX;
    if (q->rear < q->front) q->front = -1, q->rear = -1;
    return p;
}

void sortPatientsByName(Patient temp[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(temp[i].name, temp[j].name) > 0) {
                Patient t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }
}

void sortPatientsByAge(Patient temp[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (temp[i].age > temp[j].age) {
                Patient t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }
}

void displayQueue(Queue *q) {
    if (isEmpty(q)) {
        printf("\033[33mNo patients in %s department.\033[0m\n", q->deptName);
        return;
    }

    int count = (q->rear - q->front + MAX) % MAX + 1;
    Patient temp[MAX];
    int i = q->front;
    int idx = 0;

    while (1) {
        temp[idx++] = q->patients[i];
        if (i == q->rear) break;
        i = (i + 1) % MAX;
    }

    int sortChoice;
    printf("\nSort patients by:\n1. Name\n2. Age\n3. No Sorting\nEnter choice: ");
    scanf("%d", &sortChoice);

    if (sortChoice == 1)
        sortPatientsByName(temp, count);
    else if (sortChoice == 2)
        sortPatientsByAge(temp, count);

    printf("\n\033[1mPatients in %s Department:\033[0m\n", q->deptName);
    printf("--------------------------------------\n");

    for (i = 0; i < count; i++) {
        printf("%sName: %s\033[0m | Age: %d | Priority: %s\n",
               temp[i].priority == 1 ? "\033[1;31m" : "\033[1;32m",
               temp[i].name,
               temp[i].age,
               temp[i].priority == 1 ? "CRITICAL" : "NORMAL");
    }

    printf("--------------------------------------\n");
}

void listDepartments() {
    for (int i = 0; i < dept_count; i++) {
        printf("%d. %s\n", i + 1, departments[i].deptName);
    }
}

void saveData() {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    fprintf(fp, "%d\n", dept_count);
    for (int d = 0; d < dept_count; d++) {
        fprintf(fp, "%s\n", departments[d].deptName);
        Queue *q = &departments[d];
        int i = q->front;
        if (isEmpty(q)) {
            fprintf(fp, "0\n");
            continue;
        }
        fprintf(fp, "%d\n", (q->rear - q->front + MAX) % MAX + 1);
        while (1) {
            fprintf(fp, "%s;%d;%d\n", q->patients[i].name, q->patients[i].age, q->patients[i].priority);
            if (i == q->rear) break;
            i = (i + 1) % MAX;
        }
    }
    fclose(fp);
}

void loadData() {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    fscanf(fp, "%d\n", &dept_count);
    for (int i = 0; i < dept_count; i++) {
        fgets(departments[i].deptName, sizeof(departments[i].deptName), fp);
        departments[i].deptName[strcspn(departments[i].deptName, "\n")] = '\0';
        departments[i].front = departments[i].rear = -1;
        int patient_count;
        fscanf(fp, "%d\n", &patient_count);
        for (int j = 0; j < patient_count; j++) {
            char line[100], pname[50];
            int age, priority;
            fgets(line, sizeof(line), fp);
            sscanf(line, "%[^;];%d;%d", pname, &age, &priority);
            enqueue(&departments[i], pname, age, priority);
        }
    }
    fclose(fp);
}

int main() {
    loadData();
    if (dept_count == 0) {
        printf("\n\033[1m--- Setup Hospital Departments ---\033[0m\n");
        printf("Enter number of departments: ");
        scanf("%d", &dept_count);
        for (int i = 0; i < dept_count; i++) {
            printf("Enter Department %d Name: ", i + 1);
            scanf(" %[^\n]", departments[i].deptName);
            initQueue(&departments[i], departments[i].deptName);
        }
    }

    int choice, dept, age, priority;
    char name[50];

    while (1) {
        printf("\n\033[1m--- Hospital Queue Management ---\033[0m\n");
        printf("1. Register Patient\n2. Process Patient\n3. Display Queue\n4. Exit\nEnter choice: ");
        scanf("%d", &choice);
        if (choice == 4) break;

        printf("\nSelect Department:\n");
        listDepartments();
        printf("Enter choice: ");
        scanf("%d", &dept);

        if (dept < 1 || dept > dept_count) {
            printf("\033[31mInvalid Department!\033[0m\n");
            continue;
        }

        Queue *selected = &departments[dept - 1];

        if (choice == 1) {
            printf("Enter Patient Name: ");
            scanf(" %[^\n]", name);
            printf("Enter Patient Age: ");
            scanf("%d", &age);
            printf("Priority (1-Critical, 0-Normal): ");
            scanf("%d", &priority);
            enqueue(selected, name, age, priority);
        } else if (choice == 2) {
            Patient p = dequeue(selected);
            if (p.age == -1)
                printf("\033[33mNo patient to process in %s.\033[0m\n", selected->deptName);
            else
                printf("\033[36mProcessing Patient: %s, Age: %d, Priority: %s\033[0m\n",
                       p.name, p.age, p.priority == 1 ? "CRITICAL" : "NORMAL");
        } else if (choice == 3) {
            displayQueue(selected);
        }
    }

    saveData();
    printf("\033[35mData saved! Exiting... Thank you!\033[0m\n");
    return 0;
}