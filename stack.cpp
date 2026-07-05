#include<iostream>

using namespace std;

struct Stack{
    int* arr;
    int capacity;
    int nextIndex;

    Stack(){
        capacity=4;
        nextIndex=0;
        arr = new int[capacity];
    }

    Stack(int cap){
        capacity=cap;
        nextIndex=0;
        arr = new int[capacity];
    }

    bool isEmpty(){
        return nextIndex == 0;
    }

    bool isFull(){
        return nextIndex == capacity;
    }

    int size(){
        return nextIndex;
    }

    void push(int ele){
        if(isFull()){
            cout<<"Overflow.\n";
            return;
        }
        arr[nextIndex]=ele;
        nextIndex++;
    }

    void pop(){
        if(isEmpty()){
            cout<<"Underflow.";
            return;
        }
        nextIndex--;
        cout<<"Removed Item: "<<arr[nextIndex]<<endl;
    }

    int top(){
        if(isEmpty()){
            return 0;
        }
        return arr[nextIndex - 1];
    }

    void display(){
       for(int i=nextIndex - 1; i>=0; i--){
        cout<<arr[i]<<" ";
       } 
       cout<<endl;
    }
};

int main(){
    Stack st;
    st.push(12);
    st.push(4);
    st.push(6);

    st.display();
    st.pop();
    st.display();
    return 0;
}