#include<iostream>

using namespace std;
struct Queue {
    int rear;
    int front;
    int* arr;
    int capacity;

    Queue(int size){
        rear=-1;
        front=-1;
        capacity = 5;
    }

    ~Queue(){
        delete[] arr;
    }

    bool isEmpty(){
        return front == -1;
    }

    bool isFull(){
        return rear == capacity - 1;
    }

    void enqueue(int val){
        if(isFull()){
            resize();
        }

        if(isEmpty()){
            front = 0;
        }
        rear++;
        arr[rear] = val;

        cout<<val<<" inserted\n";
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

    void resize(){
        int newCapacity =  capacity * 2;
        int *newArray = new int[newCapacity];
        int j=0;
        for(int i=front; i<=rear; i++){
            newArray[j++]=arr[i];
        }
        delete arr;
        arr=newArray;
        j=j-1;
        front=0;
        capacity=newCapacity;
        cout<<"Queue Resized to "<<capacity<<".\n";
    }
};

int main(){
    Queue q(5);
    q.enqueue(10);
    q.enqueue(30);
    q.enqueue(50);
    q.enqueue(10);
    q.enqueue(30);
    q.enqueue(50);

    
    q.display();
    q.dequeue();
    q.display();
    return 0;
}