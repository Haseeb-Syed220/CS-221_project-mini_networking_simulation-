#include<iostream>
using namespace std;
class UserNode{
    int id;
    string username;
    UserNode* next;
    UserNode* friends;
    public:
    UserNode(int id, string un){
        id=id;
        username=un;
    }
};

int main(){

}