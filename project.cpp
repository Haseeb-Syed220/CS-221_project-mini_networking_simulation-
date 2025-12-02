#include<iostream>
using namespace std;
class UserNode{
    int id;
    string username;
    FriendNode* friends;
    public:
     UserNode* next;
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
class UserList{
UserNode* head;
static int count;
public:
UserList(){
    head=nullptr;
}
void add_user(string un){
    UserNode* node=new UserNode(count,un);
    if(head==nullptr){
        head=node;
        return;
    }
    UserNode* temp=head;
    while(temp->next!=nullptr)
    temp=temp->next;
    temp->next=node;
    count++;
}
};
int UserList::count=0;
int main(){

}