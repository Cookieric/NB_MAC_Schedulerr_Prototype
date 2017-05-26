#include <stdio.h>
#include <stdlib.h>

// struct node {
//     int data;
//     struct node *next;
// };

// typedef struct node Node;

typedef struct node {
    int data;
    struct node *next;
}Node;

typedef struct {
	int front;
	int *number;
	int size;
}queue;

void createq();
void showfront();
void enqueue(int);
void dequeue();
void showqueue();

Node *front, *rear;

int main(void) {
    int input, select;
   	queue Q={0};
    printf("%d,%d\n", Q.front,Q.size);
    createq();

    while(1) {
        printf("Plz Insert option(-1 End):");
        printf("\n(1)Insert data to queue");
        printf("\n(2)Show Queue front side");
        printf("\n(3)Delete front data");
        printf("\n(4)Show all queue data");
        printf("\n\$c>");
        scanf("%d", &select);

        if(select == -1)
            break;

        switch(select) {
            case 1:
                printf("\nValue:");
                scanf("%d", &input);
                enqueue(input);
                break;
            case 2:
                showfront();
                break;
            case 3:
                dequeue();
                break;
            case 4:
                showqueue();
                break;
            default:
                printf("\nOption error!");
        }
    }

    printf("\n");

    return 0;
}

void createq() {
    front = rear = (Node*) malloc(sizeof(Node));
    front->next = rear->next = NULL;
}

void showfront(){
    if(front->next == NULL)
        printf("\nQueue is empty!");
    else
        printf("\nFirst Value:%d\n", front->next->data);
}

void enqueue(int data) {
    Node *newnode;

    newnode = (Node*) malloc(sizeof(Node));

    if(front->next == NULL)
        front->next = newnode;

    newnode->data = data;
    newnode->next = NULL;
    rear->next = newnode;//Update next node from NULL to newnode
    rear = newnode;//Update currentNode to next node
}

void dequeue() {
    Node* tmpnode;

    if(front->next == NULL) {
        printf("\nQueue is empty!");
        return;
    }

    tmpnode = front->next;
    front->next = tmpnode->next;
    free(tmpnode);
}

void showqueue() {
    Node* tmpnode;

    tmpnode = front->next;

    printf("\nQueue Content:");
    while(tmpnode != NULL) {
        printf("%d ", tmpnode->data);
        tmpnode = tmpnode->next;
    }
    printf("\n");
}
