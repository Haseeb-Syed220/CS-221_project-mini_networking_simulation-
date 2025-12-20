#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

class FriendList;
class PostTree;

struct UserNode
{
    int id;
    string username;
    string password;
    FriendList *friends;
    PostTree *posts;
    UserNode *next;
    UserNode(int uid, string un, string pwd)
    {
        id = uid;
        username = un;
        password = pwd;
        next = nullptr;
        friends = nullptr;
        posts = nullptr;
    }

    void ensure_friends_initialized();
    void ensure_posts_initialized();

    ~UserNode()
    {
        if (friends != nullptr)
            delete friends;
        if (posts != nullptr)
            delete posts;
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
    int post_id;
    string content;
    int height;
    PostNode *left;
    PostNode *right;

    PostNode(int id, string c)
    {
        post_id = id;
        content = c;
        height = 1;
        left = nullptr;
        right = nullptr;
    }
};

struct QueueNode {
    UserNode* user;
    QueueNode* next;
    QueueNode(UserNode* u) {
        user = u;
        next = nullptr;
    }
};

class UserQueue {
private:
    QueueNode* front;
    QueueNode* rear;
public:
    UserQueue() {
        front = nullptr;
        rear = nullptr;
    }
    bool empty() {
        if (front == nullptr) { return true; }
        else { return false; }
    }
    void push(UserNode* u) {
        QueueNode* node = new QueueNode(u);
        if (empty()) {
            front = node;
            rear = node;
        } else {
            rear->next = node;
            rear = node;
        }
    }
    void pop() {
        if (empty()) { return; }
        QueueNode* temp = front;
        front = front->next;
        delete temp;
    }
    UserNode* get_front() {
        if (empty()) { return nullptr; }
        return front->user;
    }
};
class PostTree
{
private:
    PostNode *root;
    int next_post_id = 0; //

    void delete_tree(PostNode *node)
    {
        if (node != nullptr)
        {
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }

    int get_height(PostNode *node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        else
        {
            return node->height;
        }
    }

    void update_height(PostNode *node)
    {
        if (node != nullptr)
        {
            node->height = 1 + max(get_height(node->left), get_height(node->right));
        }
    }

    int get_balance_factor(PostNode *node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        else
        {
            return get_height(node->left) - get_height(node->right);
        }
    }
    PostNode *find_min_value_node(PostNode *node)
    {
        PostNode *temp = node;
        while (temp->left != nullptr)
        {
            temp = temp->left;
        }
        return temp;
    }

    PostNode *rotate_right(PostNode *y)
    {
        PostNode *x = y->left;
        PostNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        update_height(y);
        update_height(x);

        return x;
    }

    PostNode *rotate_left(PostNode *x)
    {
        PostNode *y = x->right;
        PostNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        update_height(x);
        update_height(y);

        return y;
    }

    PostNode *insert_post(PostNode *node, PostNode *new_post)
    {
        if (node == nullptr)
        {
            return new_post;
        }

        if (new_post->post_id < node->post_id)
        {
            node->left = insert_post(node->left, new_post);
        }
        else if (new_post->post_id > node->post_id)
        {
            node->right = insert_post(node->right, new_post);
        }
        else
        {
            return node; // In the case, the current root node matches the node to be inserted, the node itself is returned and there is no change to the tree overall.
        }

        update_height(node); // The height of the node that just got a child (directly) is updated first and then the heights of its parents consecutively.

        int balance = get_balance_factor(node);

        if (balance > 1)
        {
            if (new_post->post_id < node->left->post_id)
            {
                return rotate_right(node);
            }
            else
            {
                node->left = rotate_left(node->left);
                return rotate_right(node);
            }
        }

        if (balance < -1)
        {
            if (new_post->post_id > node->right->post_id)
            {
                return rotate_left(node);
            }
            else
            {
                node->right = rotate_right(node->right);
                return rotate_left(node);
            }
        }

        return node;
    }
    PostNode *remove_post(PostNode *node, int target_id)
    {
        if (node == nullptr)
        {
            return node;
        }

        if (target_id < node->post_id)
        {
            node->left = remove_post(node->left, target_id);
        }
        else if (target_id > node->post_id)
        {
            node->right = remove_post(node->right, target_id);
        }
        else
        {

            if (node->left == nullptr || node->right == nullptr)
            {
                PostNode *temp_node;
                if (node->left != nullptr)
                {
                    temp_node = node->left;
                }
                else
                {
                    temp_node = node->right;
                }

                if (temp_node == nullptr)
                {

                    temp_node = node;
                    node = nullptr;
                }
                else
                {

                    *node = *temp_node;
                }
                delete temp_node;
            }
            else
            {

                PostNode *temp_node = find_min_value_node(node->right);

                node->post_id = temp_node->post_id;
                node->content = temp_node->content;

                node->right = remove_post(node->right, temp_node->post_id);
            }
        }

        if (node == nullptr)
        {
            return node;
        }

        update_height(node);
        int balance = get_balance_factor(node);

        if (balance > 1)
        {
            if (get_balance_factor(node->left) >= 0)
            {
                return rotate_right(node);
            }
            else
            {
                node->left = rotate_left(node->left);
                return rotate_right(node);
            }
        }

        if (balance < -1)
        {
            if (get_balance_factor(node->right) <= 0)
            {
                return rotate_left(node);
            }
            else
            {
                node->right = rotate_right(node->right);
                return rotate_left(node);
            }
        }

        return node;
    }

    void display_timeline(PostNode *node)
    {
        if (node != nullptr)
        {
            display_timeline(node->left);

            cout << "ID: " << node->post_id << " | Content: \"" << node->content << "\"" << endl;

            display_timeline(node->right);
        }
    }
    PostNode *searchp(PostNode *node, int target_id)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (target_id == node->post_id)
        {
            return node;
        }

        if (target_id < node->post_id)
        {
            return searchp(node->left, target_id);
        }
        else
        {
            return searchp(node->right, target_id);
        }
    }

public:
    PostTree() : root(nullptr) {}

    ~PostTree()
    {
        delete_tree(root);
    }

    void post(string content)
    {
        PostNode *new_post = new PostNode(next_post_id++, content);
        root = insert_post(root, new_post);
    }

    void display_user_posts()
    {
        cout << "\n  --- User Timeline (Sequential ID Order) ---" << endl;
        if (root == nullptr)
        {
            cout << "    No posts available." << endl;
        }
        else
        {
            display_timeline(root);
        }
        cout << "  -------------------------------------------" << endl;
    }
    PostNode *search_post(int post_id)
    {
        return searchp(root, post_id);
    }
    bool delete_post(int post_id)
    {
        PostNode *initial_search = search_post(post_id);

        if (initial_search == nullptr)
        {
            return false;
        }

        root = remove_post(root, post_id);

        PostNode *final_search = search_post(post_id);

        if (final_search == nullptr)
        {
            return true;
        }
        else
        {
            cout << "The value was not deleted at all, even after using the remove_post() function.\n";
            return false;
        }
    }
};
class FriendList
{
    FriendNode *head;

public:
    FriendList()
    {
        head = nullptr;
    }

    ~FriendList()
    {
        FriendNode *current = head;
        while (current != nullptr)
        {
            FriendNode *temp = current;
            current = current->next;
            delete temp;
        }
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
        {
            cout << "The user is already a friend.\n";
            return;
        }
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

    FriendNode *get_friend_node(int index)
    {
        FriendNode *temp = head;
        int i = 0;
        while (temp != nullptr)
        {
            if (i == index)
            {
                return temp;
            }
            temp = temp->next;
            i++;
        }
        return nullptr;
    }

    int get_friend_index_by_id(int friend_id)
    {
        FriendNode *temp = head;
        int index = 0;
        while (temp != nullptr)
        {
            if (temp->user != nullptr && temp->user->id == friend_id)
            {
                return index;
            }
            temp = temp->next;
            index++;
        }
        return -1;
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

// These are the outline functions that are used to manifest that the FriendList linked list and the Post AVL tree have been initialized.
void UserNode::ensure_friends_initialized()
{
    if (friends == nullptr)
    {
        friends = new FriendList();
        cout << "The FriendList was just created\n";
    }
    else
    {
        cout << "The FriendList was already created\n";
    }
}

void UserNode::ensure_posts_initialized()
{
    if (posts == nullptr)
    {
        posts = new PostTree();
        cout << "The PostTree was just created\n";
    }
    else
    {
        cout << "The PostTree was already created\n";
    }
}

class UserList
{
    UserNode *head;

public:
    UserList()
    {
        head = nullptr;
    }
    void add_user(string un, string pwd)
    {
        UserNode *node = new UserNode(total_users(), un, pwd);
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

    UserNode *authenticate(int uid, const string &pw)
    {
        UserNode *user = find_user(uid);
        if (user == nullptr)
        {
            return nullptr;
        }
        if (user->password == pw)
        {
            return user;
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
        user1->ensure_friends_initialized();
        user2->ensure_friends_initialized();
        if (user1->friends->is_friend(user2))
        {
            cout << user1->username << " and " << user2->username << " are already friends!" << endl;
            return;
        }

        user1->friends->add_friend(user2);
        user2->friends->add_friend(user1);

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

        user1->ensure_friends_initialized();
        user2->ensure_friends_initialized();
        if (!user1->friends->is_friend(user2))
        {
            cout << user1->username << " and " << user2->username << " are not friends!" << endl;
            return;
        }

        user1->friends->remove_friend(user2);
        user2->friends->remove_friend(user1);

        cout << user1->username << " and " << user2->username << " are no longer friends." << endl;
    }
    void add_post(int uid, string content)
    {
        UserNode *user = find_user(uid);
        if (user == nullptr)
        {
            cout << "Error: User with ID " << uid << " not found." << endl;
            return;
        }
        user->ensure_posts_initialized();
        user->posts->post(content);
        cout << user->username << " posted: \"" << content << "\"" << endl;
    }
    void delete_user_post(int uid, int post_id)
    {
        UserNode *user = find_user(uid);

        if (user == nullptr)
        {
            cout << "Error: User with ID " << uid << " not found." << endl;
            return;
        }

        user->ensure_posts_initialized();
        bool success = user->posts->delete_post(post_id);

        if (success)
        {
            cout << user->username << " successfully deleted post ID " << post_id << "." << endl;
        }
        else
        {
            cout << "Error: Post with ID " << post_id << " not found or failed to delete for user " << user->username << "." << endl;
        }
    }
};

UserNode *login_user(UserList &users)
{
    int uid;
    string line;
    cout << "Enter your user ID: ";
    if (!(cin >> uid))
    {
        cin.clear();
        getline(cin, line);
        cout << "Invalid ID.\n";
        return nullptr;
    }
    getline(cin, line); // consume newline
    cout << "Enter your password: ";
    string pw;
    getline(cin, pw);

    UserNode *u = users.authenticate(uid, pw);
    if (u == nullptr)
    {
        cout << "Authentication failed. Invalid ID or password.\n";
    }
    else
    {
        return u;
    }
}

int main()
{
    UserList users;

    int status_choice = -1;
    string line;

    cout << "What is your status?\n";
    cout << "1) CEO of the social networking system\n";
    cout << "2) Simple user\n";
    cout << "Enter your choice: ";

    while (!(cin >> status_choice) || (status_choice != 1 && status_choice != 2))
    {
        cin.clear();
        getline(cin, line);
        cout << "Invalid choice. Please enter 1 or 2: ";
    }
    getline(cin, line); // consume leftover newline

    int choice = -1;

    while (true)
    {
        if (status_choice == 1) // CEO menu
        {
            cout << "\n--- CEO Menu ---\n";
            cout << "1. Add user\n";
            cout << "2. Show all users\n";
            cout << "3. Show user's friends\n";
            cout << "4. Show user's posts\n";
            cout << "5. Change the status\n";
            cout << "0. Exit\n";
            cout << "Choose an option: ";
        }
        else // Simple user menu
        {
            cout << "\n--- Mini Social Network ---\n";
            cout << "1. Make friends\n";
            cout << "2. Unfriend\n";
            cout << "3. Add post\n";
            cout << "4. Delete user's post\n";
            cout << "5. Show friends' posts\n";
            cout << "6. Change the status\n";
            cout << "0. Exit\n";
            cout << "Choose an option: ";
        }

        if (!(cin >> choice)) // If by mistake a string is sent as input, the choice parameter will fail to receive it.
        {
            cin.clear();        // This will clear the buffer for the string that was unintenionally entered in the place of integer.
            getline(cin, line); // This receive the string
            cout << "Invalid input. Try again.\n";
            continue; // Skip the currently on-going loop to give the user another change to make a choice
        }
        getline(cin, line);

        if (choice == 0)
        {
            cout << "Exiting the premisis....\n";
            break;
        }

        if (status_choice == 1) // CEO choices
        {
            if (choice == 1)
            {
                cout << "Enter username: ";
                string username;
                getline(cin, username);
                cout << "Set a password for this user: ";
                string password;
                getline(cin, password);
                if (username.empty())
                {
                    cout << "Username cannot be empty.\n";
                    continue;
                }
                if (password.empty())
                {
                    cout << "Password cannot be empty.\n";
                    continue;
                }
                users.add_user(username, password);
                int assigned_id = users.total_users() - 1;
                cout << "User \"" << username << "\" added with ID " << assigned_id << ".\n";
            }
            else if (choice == 2)
            {
                int tot = users.total_users();
                if (tot == 0)
                {
                    cout << "No users yet.\n";
                    continue;
                }
                else
                {
                    cout << "All users:\n";
                    for (int i = 0; i < tot; ++i)
                    {
                        UserNode *u = users.find_user(i);
                        if (u != nullptr)
                        {
                            cout << "  ID: " << u->id << " | Username: " << u->username << '\n';
                        }
                    }
                }
            }
            else if (choice == 3)
            {
                cout << "Enter user ID to view friends: ";
                int uid;
                if (!(cin >> uid))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                UserNode *u = users.find_user(uid);
                if (u == nullptr)
                {
                    cout << "User not found.\n";
                    continue;
                }
                u->ensure_friends_initialized();
                int n = u->friends->total_friends();
                if (n == 0)
                {
                    cout << u->username << " has no friends.\n";
                    continue;
                }
                cout << u->username << "'s friends:\n";
                for (int i = 0; i < n; ++i)
                {
                    FriendNode *fn = u->friends->get_friend_node(i);
                    if (fn && fn->user)
                    {
                        cout << "  ID: " << fn->user->id << " | Username: " << fn->user->username << '\n';
                    }
                }
            }
            else if (choice == 4)
            {
                cout << "Enter user ID to view posts: ";
                int uid;
                if (!(cin >> uid))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                UserNode *u = users.find_user(uid);
                if (u == nullptr)
                {
                    cout << "User not found.\n";
                    continue;
                }
                u->ensure_posts_initialized();
                u->posts->display_user_posts();
            }
            else if (choice == 5)
            {
                status_choice = 2;
            }
            else
            {
                cout << "Unknown option. Try again.\n";
            }
        }
        else // Simple user options
        {
            if (choice == 1)
            {
                UserNode *u1 = login_user(users);
                if (u1 == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                int a = u1->id;
                cout << "Enter second user ID to befriend: ";
                int b;
                if (!(cin >> b))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                users.make_friends(a, b);
            }
            else if (choice == 2)
            {
                UserNode *u1 = login_user(users);
                if (u1 == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                int a = u1->id;
                cout << "Enter second user ID to unfriend: ";
                int b;
                if (!(cin >> b))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                users.unfriend(a, b);
            }
            else if (choice == 3)
            {
                UserNode *u = login_user(users);
                if (u == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                cout << "Enter post content: ";
                string content;
                getline(cin, content);
                if (content.empty())
                {
                    cout << "Empty post not allowed.\n";
                    continue;
                }
                users.add_post(u->id, content);
            }
            else if (choice == 4)
            {
                UserNode *u = login_user(users);
                if (u == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                cout << "Enter post number to delete: ";
                int pid;
                if (!(cin >> pid))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid post ID.\n";
                    continue;
                }
                getline(cin, line);
                users.delete_user_post(u->id, pid);
            }
            else if (choice == 5)
            {
                UserNode *u = login_user(users);
                if (u == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                u->ensure_friends_initialized();
                int n = u->friends->total_friends();
                if (n == 0)
                {
                    cout << u->username << " has no friends.\n";
                    continue;
                }
                int friend_id;
                cout << "Enter the ID of the friend whose post you want to view: ";
                cin >> friend_id;
                UserNode *u2 = u->friends->get_friend_node(u->friends->get_friend_index_by_id(friend_id))->user;
                cout << "Posts from " << u->username << "'s friends:\n";
                u2->ensure_posts_initialized();
                u2->posts->display_user_posts();
            }
            else if (choice == 6)
            {
                status_choice = 1;
            }
            else
            {
                cout << "Unknown option. Try again.\n";
            }
        }
    }
    return 0;
}