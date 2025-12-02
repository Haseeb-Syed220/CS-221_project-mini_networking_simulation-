#include<iostream>
using namespace std;
class UserNode{
    int id;
    string username;
    UserNode* next;
    FriendNode* friends;
    public:
    UserNode(int uid, string un){
        id=uid;
        username=un;
        next=nullptr;
    }
};
class FriendNode{
    UserNode* user;
    FriendNode* next;
    FriendNode(UserNode* un){
        user=un;
        next=nullptr;
    }
};

int main(){

}