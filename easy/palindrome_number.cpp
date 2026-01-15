class Solution {
public:
    bool isPalindrome(int x) {
        std::vector<int> arr;
        if(x<0){
            return false;
        }
        int temp=x;
        while(temp>0){
            int digit = temp % 10;
            arr.push_back(digit);
            temp /= 10;
        }
        int reversed = 0;
        for(int d : arr){
            reversed = reversed * 10 + d;
        }
        return reversed == x ? true : false;
    }
};
