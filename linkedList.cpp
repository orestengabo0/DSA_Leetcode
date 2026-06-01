#include <iostream>

using namespace std;

struct Node
{
    int data;
    Node *next;
    Node(int d)
    {
        this->data = d;
        this->next = NULL;
    }
};

Node* addAtHead(Node *head, int d)
{
    Node *n = new Node(d);
    n->next = head;
    head = n;
    return head;
}

Node* addAtTail(Node* head, int d){
    Node* n = new Node(d);
    if(head==NULL){
        head = n;
        return head;
    }
    Node* tail = head;
    while(tail->next != NULL){
        tail = tail->next;
    }
    tail->next = n;
    return head;
}

Node* addAtPosition(Node* head, int d, int p){
    Node* n = new Node(d);

    if(p == 0){
        n->next = head;
        return n;
    }

    int count = 1;
    Node* temp = head;
    while(count < p && temp != NULL){
        count++;
        temp = temp->next;
    }
    
    if(temp == NULL){
        cout << "Position out of bounds!" << endl;
        delete n;
        return head;
    }
    n->next = temp->next;
    temp->next = n;

    return head;
}

Node* deleteHead(Node* head){
    if(head == NULL){
        return NULL;
    }
    Node* temp = head;
    head = head->next;
    delete temp;
    return head;
}

Node* deleteTail(Node* head){
    if(head == NULL){
        return NULL;
    }
    if(head->next == NULL){
        delete head;
        return NULL;
    }

    Node* secondLast = head;
    while(secondLast->next->next != NULL){
        secondLast = secondLast->next;
    }
    delete secondLast->next;
    secondLast->next = NULL;
    return head;
}

Node* deleteAtPosition(Node* head, int p){
    if(head == NULL){
        return NULL;
    }
    Node* temp = head;
    if(p == 0){
        head = head->next;
        delete temp;
        return head;
    }
    int count = 1;
    while(count < p && temp->next != NULL){
        count++;
        temp = temp->next;
    }

    if(temp==NULL){
        cout<<"Position out of bounds!"<<endl;
        return head;
    }

    Node* nodeToDelete = temp->next;
    temp->next = nodeToDelete->next;
    delete nodeToDelete;

    return head;
}

void printList(Node *head)
{
    while (head != NULL)
    {
        cout << head->data << "->";
        head = head->next;
    }
    cout << "NULL";
}

int main()
{
    Node *n1 = new Node(3);
    Node *n2 = new Node(1);
    Node *n3 = new Node(6);
    Node *n4 = new Node(2);
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;

    Node* head = n1;
    
    cout << "Original: ";
    printList(head);
    cout << "\n";
    
    head = deleteHead(head);  // Remove 3
    cout << "After deleting head: ";
    printList(head);  // Should show: 1->6->2->NULL
    cout << "\n";
    
    head = addAtHead(head,30);
    cout<<"After adding new Element at head: ";
    printList(head);
    cout<<"\n";

    head = addAtTail(head, 20);
    cout<<"After adding new Element at tail: ";
    printList(head);
    cout<<"\n";

    head = deleteTail(head);
    cout<<"After deleting tail: ";
    printList(head);
    cout<<"\n";

    head = addAtPosition(head, 10, 3);
    cout<<"After adding at position 3: ";
    printList(head);
    cout<<"\n";

    head = deleteAtPosition(head, 3);
    cout<<"After deleting at position 3: ";
    printList(head);
    cout<<"\n";
    
    return 0;
}