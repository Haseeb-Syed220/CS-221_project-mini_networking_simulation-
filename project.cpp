#include <iostream>
using namespace std;
struct UserNode
{
    int id;
    string username;
    FriendList friends;
    UserNode *next;
    UserNode(int uid, string un)
    {
        id = uid;
        username = un;
        next = nullptr;
    }
};
struct FriendNode
{
    UserNode *user;
    FriendNode *next;
    FriendNode(UserNode *un)
    {
        user = un;
        next = nullptr;
    }
};
struct PostNode
{
};
class FriendList
{
    FriendNode *head;

public:
    FriendList()
    {
        head = nullptr;
    }

    bool is_friend(UserNode *u)
    {
        FriendNode *temp = head;

        while (temp != nullptr)
        {
            if (temp->user == u)
            {
                return true;
            }
            temp = temp->next;
        }

        return false;
    }
    void add_friend(UserNode *u)
    {
        if (is_friend(u))
            return;
        FriendNode *node = new FriendNode(u);
        node->next = head;
        head = node;
    }
    void remove_friend(UserNode *myfriend)
    {
        if (head == nullptr)
        {
            return;
        }
        if (!is_friend(myfriend))
            return;

        if (head->user == myfriend)
        {
            FriendNode *temp = head;
            head = head->next;
            delete temp;
            return;
        }

        FriendNode *current = head;
        while (current->next != nullptr)
        {
            if (current->next->user == myfriend)
            {
                FriendNode *temp = current->next;
                current->next = temp->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }
    int total_friends()
    {
        FriendNode *temp = head;
        int i = 0;
        while (temp)
        {
            temp = temp->next;
            i++;
        }
        return i;
    }
};
class UserList
{
    UserNode *head;

public:
    UserList()
    {
        head = nullptr;
    }
    void add_user(string un)
    {
        UserNode *node = new UserNode(total_users(), un);
        if (head == nullptr)
        {
            head = node;
            return;
        }
        UserNode *temp = head;
        while (temp->next != nullptr)
            temp = temp->next;
        temp->next = node;
    }
    int total_users()
    {
        UserNode *temp = head;
        int i = 0;
        while (temp)
        {
            temp = temp->next;
            i++;
        }
        return i;
    }
    UserNode *find_user(int uid)
    {
        UserNode *temp = head;
        while (temp != nullptr)
        {
            if (temp->id == uid)
            {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    void make_friends(int id1, int id2)
    {
        UserNode *user1 = find_user(id1);
        UserNode *user2 = find_user(id2);

        if (user1 == nullptr || user2 == nullptr)
        {
            cout << "Error: One or both users not found." << endl;
            return;
        }

        if (id1 == id2)
        {
            cout << "Error: A user cannot be friends with themselves." << endl;
            return;
        }
        if (user1->friends.is_friend(user2))
        {
            cout << user1->username << " and " << user2->username << " are already friends!" << endl;
            return;
        }

        user1->friends.add_friend(user2);
        user2->friends.add_friend(user1);

        cout << user1->username << " and " << user2->username << " are now mutual friends!" << endl;
    }
    void unfriend(int id1, int id2)
    {
        UserNode *user1 = find_user(id1);
        UserNode *user2 = find_user(id2);

        if (user1 == nullptr || user2 == nullptr)
        {
            cout << "Error: One or both users not found." << endl;
            return;
        }

        if (!user1->friends.is_friend(user2))
        {
            cout << user1->username << " and " << user2->username << " are not friends!" << endl;
            return;
        }

        user1->friends.remove_friend(user2);
        user2->friends.remove_friend(user1);

        cout << user1->username << " and " << user2->username << " are no longer friends." << endl;
    }
};

int main()
{
}