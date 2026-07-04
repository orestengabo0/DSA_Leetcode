// Online C compiler to run C program online
#include <stdio.h>

void selectionSort(int arr[], int n){
    for(int i=0; i<n-1; i++){
        int targetIndex=i;
        for(int j=i+1; j<n-1; j++){
            if(arr[j] < arr[targetIndex]){
                targetIndex=j;
            }
        }
        int temp=arr[i];
        arr[i] = arr[targetIndex];
        arr[targetIndex]=temp;
    }
}

void insertionSort(int arr[], int n){
    int key,i,j;
    for(i=1; i<n; i++){
        key=arr[i];
        j=i-1;
        
        while(j>=0 && arr[j] > key){
            arr[j+1]=arr[j];
            j=j-1;
        }
        arr[j+1]=key;
    }
}

void bubbleSort(int arr[], int n){
    for(int i=0; i<n-1; i++){
        for(int j=0; j<n-1; j++){
            if(arr[j]>arr[j+1]){
                int temp=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=temp;
            }
        }
    }
}

void mergeArrays(int x[], int y[], int a[], int s, int e){
    int mid = (s+e)/2;
    int k=s;
    int i=s;
    int j=mid+1;
    while(i<=mid && j<=e){
        if(x[i] < y[j]){
            a[k]=x[i];
            i++;
            k++;
        }
        if(x[i] > y[j]){
            a[k]=y[j];
            j++;
            k++;
        }
    }
    
    while(i<=mid){
        if(x[i]<y[j]){
            a[k]=x[i];
            i++;
            k++;
        }
        if(x[i]>y[j]){
            a[k]=y[j];
            j++;
            k++;
        }
    }
}

void mergeSort(int arr[], int s, int e){
    int mid=(s+e)/2;
    int x[100],y[100];
    
    if(s>=e){
        return;
    }
    
    for(int i=0; i<=mid; i++){
        x[i]=arr[i];
    }
    
    for(int j=mid+1;j<=e;j++){
        y[j]=arr[j];
    }
    
    mergeSort(x,s,mid);
    mergeSort(y,mid+1,e);
    mergeArrays(x,y,arr,s,e);
}

void printArray(int arr[], int n){
    for(int i=0; i<n; i++){
        printf("%d ", arr[i]);
    }
}

int main() {
    int arr1[] = {5,2,6,3,1,-1,9};
    int len1=sizeof(arr1)/sizeof(arr1[0]);
    selectionSort(arr1,len1);
    printf("Selection Sort: ");
    printArray(arr1, len1);
    
    printf("\n");
    
    int arr2[] = {5,2,6,3,1,-1,9};
    int len2=sizeof(arr2)/sizeof(arr2[0]);
    insertionSort(arr2,len2);
    printf("Insertion Sort: ");
    printArray(arr2, len2);
    
    printf("\n");
    
    int arr3[] = {5,2,6,3,1,-1,9};
    int len3=sizeof(arr3)/sizeof(arr3[0]);
    bubbleSort(arr3,len3);
    printf("Bubble Sort: ");
    printArray(arr3, len3);
    
    printf("\n");
    
    int arr4[] = {5,2,6,3,1,-1,9};
    int len4=sizeof(arr4)/sizeof(arr4[0]);
    mergeSort(arr4,0,len4);
    printf("Merge Sort: ");
    printArray(arr4, len4);
    
    return 0;
}