#include<iostream>

using namespace std;

struct Node {
    int data;
    Node* next;

    Node(int d){
        this->data=d;
        this->next=NULL;
    }
};

struct Stack {
    Node* head;
    int size;

    Stack(){
        head=NULL;
        size=0;
    }

    void push(int d){
        Node* n = new Node(d);
        if(head==NULL){
            head=n;
        }else{
            n->next=head;
            head=n;
        }
        size++;
    }

    void pop(){
        if(head==NULL){
            return;
        }
        Node* temp=head;
        head=head->next;
        delete temp;
        size--;
    }

    void display(){
        Node* curr=head;
        while(curr!=NULL){
            cout<<curr->data<<"->";
            curr=curr->next;
        }
        cout<<"NULL\n";
    }
};

int main(){
    Stack st;
    st.push(12);
    st.push(34);
    st.push(2);

    st.display();

    st.pop();
    st.display();

    st.push(3);
    st.display();
}