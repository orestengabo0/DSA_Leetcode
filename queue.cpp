#include<iostream>

using namespace std;

const int SIZE = 5;

class Queue {
    private:
    int rear;
    int front;
    int arr[SIZE];

    public:
    Queue(){
        rear=-1;
        front=-1;
    }

    bool isEmpty(){
        return front == -1;
    }

    bool isFull(){
        return rear == SIZE - 1;
    }

    void enqueue(int val){
        if(isFull()){
            cout<<"Overflow.\n";
            return;
        }

        if(rear == -1){
            front = 0;
        }
        rear++;
        arr[rear] = val;

        cout<<val<<"inserted\n";
    }

    void dequeue(){
        if(isEmpty()){
            cout<<"Underflow.\n";
            return;
        }

        if(rear == front){
            rear = -1;
            front = -1;
        }
        front++;
        cout<<"Front Removed.\n";
    }

    int peek(){
        if(isEmpty()){
            cout<<"UnderFlow.\n";
            return -1;
        }
        return arr[front];
    }

    void display(){
        if(isEmpty()){
            cout<<"UnderFlow.\n";
            return;
        }
        for(int i=front; i <= rear; i++){
            cout<<arr[i]<<" ";
        }
        cout<<endl;
    }
};

int main(){
    Queue q;
    q.enqueue(10);
    q.enqueue(30);
    q.enqueue(50);

    
    q.display();
    q.dequeue();
    q.display();
    return 0;
}