#include<iostream>

using namespace std;

struct Node{
    int data;
    Node* next;
    Node(int d){
        this->data = d;
        this->next = NULL;
    }
};

struct Queue{
    Node* head;
    Node* tail;
    int size;

    Queue(){
        head = NULL;
        tail = NULL;
        size = 0;
    }

    bool isEmpty(){
        return size == 0;
    }

    int getSize(){
        return size;
    }

    void enqueue(int d){
        Node* newNode = new Node(d);
        if(head==NULL){
            head=newNode;
            tail=newNode;
        }else{
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void dequeue(){
        if(isEmpty()){
            return;
        }
        Node* temp=head;
        head=head->next;
        if(head == NULL){
            tail = NULL;
        }

        delete temp;
        size--;
    }

    int peek(){
        if(isEmpty()){
            return 0;
        }
        return head->data;
    }

    void display(){
        if(isEmpty()){
            cout<<"NULL\n";
            return;
        }
        Node* current=head;
        while(current != NULL){
            cout<<current->data<<"->";
            current=current->next;
        }
        cout<<"NULL\n";
    }
};

int main(){
    Queue q;
    q.enqueue(10);
    q.enqueue(30);
    q.enqueue(40);
    q.enqueue(50);
    q.display();
    q.dequeue();
    q.enqueue(60);
    q.display();
    cout<<q.peek()<<endl;

    return 0;
}