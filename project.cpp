#include<iostream>
using namespace std;
class UserNode{
    int id;
    string username;
    FriendList friends;
    public:
     UserNode* next;
    UserNode(int uid, string un){
        id=uid;
        username=un;
        next=nullptr;
    }
};
class FriendNode{
    public:
    UserNode* user;
    FriendNode* next;
    FriendNode(UserNode* un){
        user=un;
        next=nullptr;
    }
};
class FriendList{
FriendNode* head;
public:
FriendList(){
    head=nullptr;
}
void add_friend(UserNode* u){
    FriendNode* node=new FriendNode(u);
    if(head==nullptr){
        head=node;
        return;
    }
    FriendNode* temp=head;
    while(temp->next!=nullptr)
    temp=temp->next;
    temp->next=node;
}
int total_friends(){
    FriendNode* temp=head;
    int i=0;
    while(temp){
        temp=temp->next;
        i++;
    }
    return i;
}
};
class UserList{
UserNode* head;
public:
UserList(){
    head=nullptr;
}
void add_user(string un){
    UserNode* node=new UserNode(total_users(),un);
    if(head==nullptr){
        head=node;
        return;
    }
    UserNode* temp=head;
    while(temp->next!=nullptr)
    temp=temp->next;
    temp->next=node;
}
int total_users(){
    UserNode* temp=head;
    int i=0;
    while(temp){
        temp=temp->next;
        i++;
    }
    return i;
}
};

int main(){

}