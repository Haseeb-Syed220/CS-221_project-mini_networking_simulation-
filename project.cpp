#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

class FriendList;
class PostTree;
class AdjacencyMatrix;
class MessageList;

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

struct MessageNode
{
    int message_id;
    int sender_id;
    int receiver_id;
    string content;
    MessageNode *next;

    MessageNode(int mid, int sid, int rid, string c)
    {
        message_id = mid;
        sender_id = sid;
        receiver_id = rid;
        content = c;
        next = nullptr;
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

            cout << "Post number: " << node->post_id << " | Content: \"" << node->content << "\"" << endl;

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
struct HashNode
{
    int key;
    UserNode *value;
    HashNode *next;

    HashNode(int k, UserNode *v)
    {
        key = k;
        value = v;
        next = nullptr;
    }
};

class UserHashMap
{
private:
    static const int TABLE_SIZE = 100;
    HashNode *table[TABLE_SIZE];

    int hash_function(int key)
    {
        return key % TABLE_SIZE;
    }

public:
    UserHashMap()
    {
        int i = 0;
        while (i < TABLE_SIZE)
        {
            table[i] = nullptr;
            i++;
        }
    }

    void insert(int key, UserNode *value)
    {
        int index = hash_function(key);
        HashNode *new_node = new HashNode(key, value);

        if (table[index] == nullptr)
        {
            table[index] = new_node;
        }
        else
        {
            new_node->next = table[index];
            table[index] = new_node;
        }
    }

    UserNode *get(int key)
    {
        int index = hash_function(key);
        HashNode *temp = table[index];

        while (temp != nullptr)
        {
            if (temp->key == key)
            {
                return temp->value;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    ~UserHashMap()
    {
        int i = 0;
        while (i < TABLE_SIZE)
        {
            HashNode *temp = table[i];
            while (temp != nullptr)
            {
                HashNode *to_delete = temp;
                temp = temp->next;
                delete to_delete;
            }
            i++;
        }
    }
};
class AdjacencyMatrix
{
private:
    vector<vector<bool>> matrix;
    int max_users;

public:
    AdjacencyMatrix(int size) : max_users(size)
    {
        matrix.resize(size, vector<bool>(size, false));
    }

    void add_friendship(int user1_id, int user2_id)
    {
        if (user1_id >= 0 && user1_id < max_users && user2_id >= 0 && user2_id < max_users)
        {
            matrix[user1_id][user2_id] = true;
            matrix[user2_id][user1_id] = true;
        }
    }

    void remove_friendship(int user1_id, int user2_id)
    {
        if (user1_id >= 0 && user1_id < max_users && user2_id >= 0 && user2_id < max_users)
        {
            matrix[user1_id][user2_id] = false;
            matrix[user2_id][user1_id] = false;
        }
    }

    bool are_friends(int user1_id, int user2_id)
    {
        if (user1_id >= 0 && user1_id < max_users && user2_id >= 0 && user2_id < max_users)
        {
            return matrix[user1_id][user2_id];
        }
        return false;
    }

    void expand_matrix(int new_size)
    {
        if (new_size > max_users)
        {
            int old_size = max_users;
            max_users = new_size;
            matrix.resize(new_size);
            for (int i = 0; i < new_size; i++)
            {
                matrix[i].resize(new_size, false);
            }
        }
    }

    void display_matrix(int total_users)
    {
        cout << "\n  --- Friendship Adjacency Matrix ---" << endl;
        cout << "     ";
        for (int i = 0; i < total_users; i++)
        {
            cout << i << " ";
        }
        cout << endl;
        for (int i = 0; i < total_users; i++)
        {
            cout << "  " << i << " ";
            for (int j = 0; j < total_users; j++)
            {
                cout << (matrix[i][j] ? "1" : "0") << " ";
            }
            cout << endl;
        }
        cout << "  ------------------------------------" << endl;
    }
};

class MessageList
{
private:
    MessageNode *head;
    int next_message_id;

public:
    MessageList() : head(nullptr), next_message_id(0) {}

    ~MessageList()
    {
        MessageNode *current = head;
        while (current != nullptr)
        {
            MessageNode *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void add_message(int sender_id, int receiver_id, string content)
    {
        MessageNode *new_message = new MessageNode(next_message_id++, sender_id, receiver_id, content);
        new_message->next = head;
        head = new_message;
    }

    void display_messages_between(int user1_id, int user2_id)
    {
        MessageNode *current = head;
        bool found = false;
        cout << "\n  --- Messages between User " << user1_id << " and User " << user2_id << " ---" << endl;
        while (current != nullptr)
        {
            if ((current->sender_id == user1_id && current->receiver_id == user2_id) ||
                (current->sender_id == user2_id && current->receiver_id == user1_id))
            {
                cout << "  Message ID: " << current->message_id << " | From User " << current->sender_id
                     << " to User " << current->receiver_id << ": \"" << current->content << "\"" << endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            cout << "    No messages found." << endl;
        }
        cout << "  ------------------------------------------------" << endl;
    }

    void display_user_messages(int user_id)
    {
        MessageNode *current = head;
        bool found = false;
        cout << "\n  --- Messages for User " << user_id << " ---" << endl;
        while (current != nullptr)
        {
            if (current->receiver_id == user_id || current->sender_id == user_id)
            {
                cout << "  Message ID: " << current->message_id << " | From User " << current->sender_id
                     << " to User " << current->receiver_id << ": \"" << current->content << "\"" << endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            cout << "    No messages found." << endl;
        }
        cout << "  ------------------------------------" << endl;
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

    void sort_by_username()
    {
        if (head == nullptr || head->next == nullptr)
        {
            return; // Empty or single node, no need to sort
        }

        // Collect all FriendNodes into a vector
        vector<FriendNode *> nodes;
        FriendNode *temp = head;
        while (temp != nullptr)
        {
            nodes.push_back(temp);
            temp = temp->next;
        }

        // Sort by username using bubble sort
        int n = nodes.size();
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = 0; j < n - i - 1; j++)
            {
                if (nodes[j]->user != nullptr && nodes[j + 1]->user != nullptr)
                {
                    if (nodes[j]->user->username > nodes[j + 1]->user->username)
                    {
                        FriendNode *temp = nodes[j];
                        nodes[j] = nodes[j + 1];
                        nodes[j + 1] = temp;
                    }
                }
            }
        }

        // Rebuild the linked list in sorted order
        head = nodes[0];
        for (size_t i = 0; i < nodes.size() - 1; i++)
        {
            nodes[i]->next = nodes[i + 1];
        }
        nodes[nodes.size() - 1]->next = nullptr;
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
struct QueueNode
{
    UserNode *user;
    QueueNode *next;
    QueueNode(UserNode *u)
    {
        user = u;
        next = nullptr;
    }
};

class UserQueue
{
private:
    QueueNode *front;
    QueueNode *rear;

public:
    UserQueue()
    {
        front = nullptr;
        rear = nullptr;
    }
    bool empty()
    {
        if (front == nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    void push(UserNode *u)
    {
        QueueNode *node = new QueueNode(u);
        if (empty())
        {
            front = node;
            rear = node;
        }
        else
        {
            rear->next = node;
            rear = node;
        }
    }
    void pop()
    {
        if (empty())
        {
            return;
        }
        QueueNode *temp = front;
        front = front->next;
        delete temp;
    }
    UserNode *get_front()
    {
        if (empty())
        {
            return nullptr;
        }
        return front->user;
    }
};
class UserList
{
    UserNode *head;
    AdjacencyMatrix *adjacency_matrix;
    MessageList *messages;

public:
    UserList()
    {
        head = nullptr;
        adjacency_matrix = new AdjacencyMatrix(1000); // Initialize with capacity for 1000 users
        messages = new MessageList();
    }

    ~UserList()
    {
        if (adjacency_matrix != nullptr)
            delete adjacency_matrix;
        if (messages != nullptr)
            delete messages;
    }
    void add_user(string un, string pwd)
    {
        int new_user_count = total_users() + 1;
        // Expand adjacency matrix if needed
        adjacency_matrix->expand_matrix(new_user_count);

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

        // Update adjacency matrix
        adjacency_matrix->add_friendship(id1, id2);

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

        // Update adjacency matrix
        adjacency_matrix->remove_friendship(id1, id2);

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

    void display_adjacency_matrix()
    {
        int total = total_users();
        if (total == 0)
        {
            cout << "No users available to display adjacency matrix." << endl;
            return;
        }
        adjacency_matrix->display_matrix(total);
    }

    void send_message(int sender_id, int receiver_id, string content)
    {
        UserNode *sender = find_user(sender_id);
        UserNode *receiver = find_user(receiver_id);

        if (sender == nullptr || receiver == nullptr)
        {
            cout << "Error: One or both users not found." << endl;
            return;
        }

        if (sender_id == receiver_id)
        {
            cout << "Error: A user cannot send a message to themselves." << endl;
            return;
        }

        // Check if users are friends using adjacency matrix
        if (!adjacency_matrix->are_friends(sender_id, receiver_id))
        {
            cout << "Error: Users must be friends to send messages. " << sender->username
                 << " and " << receiver->username << " are not friends." << endl;
            return;
        }

        messages->add_message(sender_id, receiver_id, content);
        cout << "Message sent from " << sender->username << " (ID: " << sender_id
             << ") to " << receiver->username << " (ID: " << receiver_id << ")." << endl;
    }

    void view_messages_between(int user1_id, int user2_id)
    {
        UserNode *user1 = find_user(user1_id);
        UserNode *user2 = find_user(user2_id);

        if (user1 == nullptr || user2 == nullptr)
        {
            cout << "Error: One or both users not found." << endl;
            return;
        }

        messages->display_messages_between(user1_id, user2_id);
    }

    void view_user_messages(int user_id)
    {
        UserNode *user = find_user(user_id);
        if (user == nullptr)
        {
            cout << "Error: User with ID " << user_id << " not found." << endl;
            return;
        }

        messages->display_user_messages(user_id);
    }
    vector<int> find_shortest_path(int src_id, int dest_id)
    {
        vector<int> path;

        UserNode *src = find_user(src_id);
        UserNode *dest = find_user(dest_id);

        if (src == nullptr || dest == nullptr)
            return path;

        if (src_id == dest_id)
        {
            path.push_back(src_id);
            return path;
        }

        int total = total_users();

        vector<bool> visited(total, false);
        UserQueue q;
        UserHashMap parent; // child_id → parent UserNode*

        visited[src_id] = true;
        parent.insert(src_id, nullptr);
        q.push(src);

        bool found = false;

        while (!q.empty() && !found)
        {
            UserNode *current = q.get_front();
            q.pop();

            int u = current->id;

            for (int v = 0; v < total; v++)
            {
                if (!visited[v] && adjacency_matrix->are_friends(u, v))
                {
                    UserNode *neighbor = find_user(v);
                    if (neighbor == nullptr)
                        continue;

                    visited[v] = true;
                    parent.insert(v, current);
                    q.push(neighbor);

                    if (v == dest_id)
                    {
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found)
            return path;

        // Reconstruct path (dest → src)
        int curr = dest_id;
        while (true)
        {
            path.push_back(curr);
            UserNode *p = parent.get(curr);
            if (p == nullptr)
                break;
            curr = p->id;
        }

        reverse(path.begin(), path.end());
        return path;
    }
    void print_shortest_path(int src_id, int dest_id)
    {
        vector<int> path = find_shortest_path(src_id, dest_id);

        if (path.empty())
        {
            cout << "No connection path found between user "
                 << src_id << " and user " << dest_id << ".\n";
            return;
        }

        cout << "Shortest path (" << path.size() - 1 << " hop(s)):\n";
        for (int i = 0; i < path.size(); i++)
        {
            UserNode *u = find_user(path[i]);
            if (u)
                cout << "[" << u->id << "] " << u->username;
            else
                cout << "[" << path[i] << "] Unknown";

            if (i + 1 < path.size())
                cout << " -> ";
        }
        cout << endl;
    }
};

void log_action(const string &action)
{
    // open the file named exactly "text_file" in append mode
    ofstream log_file("text_file", ios::app);
    if (!log_file)
    {
        cerr << "Warning: could not open log file 'text_file' for writing.\n";
        return;
    }

    time_t now = time(0);
    char *dt = ctime(&now);
    string timestamp(dt);
    if (!timestamp.empty() && timestamp.back() == '\n')
        timestamp.pop_back(); // remove newline from ctime

    log_file << "[" << timestamp << "] " << action << endl;
    log_file.flush(); // ensure it's written immediately
    // file closed automatically when log_file goes out of scope
}

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
    // ensure log file exists (creates the file if it doesn't)
    ofstream("text_file", ios::app).close();
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
    if (status_choice == 1)
        log_action("Status selected: CEO of the social networking system");
    else
        log_action("Status selected: Simple user");

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
            cout << "5. Display friendship adjacency matrix\n";
            cout << "6. Change the status\n";
            cout << "7. Find shortest path between two users\n";
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
            cout << "6. Send message to friend\n";
            cout << "7. View messages with a friend\n";
            cout << "8. View all my messages\n";
            cout << "9. Change the status\n";
            cout << "10. Find shortest path between two users\n";
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
                log_action("CEO: Added user \"" + username + "\" with ID " + to_string(assigned_id));
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
                log_action("CEO: Viewed all users (Total: " + to_string(tot) + ")");
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
                u->friends->sort_by_username();
                cout << u->username << "'s friends:\n";
                for (int i = 0; i < n; ++i)
                {
                    FriendNode *fn = u->friends->get_friend_node(i);
                    if (fn && fn->user)
                    {
                        cout << "  ID: " << fn->user->id << " | Username: " << fn->user->username << '\n';
                    }
                }
                log_action("CEO: Viewed friends of user ID " + to_string(uid) + " (" + u->username + ")");
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
                log_action("CEO: Viewed posts of user ID " + to_string(uid) + " (" + u->username + ")");
            }
            else if (choice == 5)
            {
                users.display_adjacency_matrix();
                log_action("CEO: Displayed friendship adjacency matrix");
            }
            else if (choice == 6)
            {
                status_choice = 2;
                log_action("CEO: Changed status to Simple user");
            }
            else if (choice == 7) // <<-- ADDED: CEO shortest-path handler
            {
                cout << "Enter source user ID: ";
                int src_id;
                if (!(cin >> src_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                cout << "Enter destination user ID: ";
                int dest_id;
                if (!(cin >> dest_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                users.print_shortest_path(src_id, dest_id);
                log_action("CEO: Found shortest path between user ID " + to_string(src_id) + " and user ID " + to_string(dest_id));
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
                UserNode *u2 = users.find_user(b);
                if (u2 != nullptr)
                    log_action("User: User ID " + to_string(a) + " made friends with User ID " + to_string(b));
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
                UserNode *u2 = users.find_user(b);
                if (u2 != nullptr)
                    log_action("User: User ID " + to_string(a) + " unfriended User ID " + to_string(b));
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
                log_action("User: User ID " + to_string(u->id) + " (" + u->username + ") added a post");
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
                log_action("User: User ID " + to_string(u->id) + " (" + u->username + ") deleted post ID " + to_string(pid));
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
                if (u2 != nullptr)
                    log_action("User: User ID " + to_string(u->id) + " viewed posts of friend ID " + to_string(friend_id));
            }
            else if (choice == 6)
            {
                UserNode *sender = login_user(users);
                if (sender == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                cout << "Enter receiver user ID: ";
                int receiver_id;
                if (!(cin >> receiver_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                cout << "Enter message content: ";
                string message_content;
                getline(cin, message_content);
                if (message_content.empty())
                {
                    cout << "Empty message not allowed.\n";
                    continue;
                }
                users.send_message(sender->id, receiver_id, message_content);
                log_action("User: User ID " + to_string(sender->id) + " sent a message to User ID " + to_string(receiver_id));
            }
            else if (choice == 7)
            {
                UserNode *u1 = login_user(users);
                if (u1 == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                cout << "Enter friend's user ID: ";
                int friend_id;
                if (!(cin >> friend_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                users.view_messages_between(u1->id, friend_id);
                log_action("User: User ID " + to_string(u1->id) + " viewed messages with User ID " + to_string(friend_id));
            }
            else if (choice == 8)
            {
                UserNode *u = login_user(users);
                if (u == nullptr)
                {
                    cout << "The account does not exist.\n";
                    continue;
                }
                users.view_user_messages(u->id);
            }
            else if (choice == 9)
            {
                status_choice = 1;
            }
            else if (choice == 10) // <<-- ADDED: Simple-user shortest-path handler
            {
                cout << "Enter source user ID: ";
                int src_id;
                if (!(cin >> src_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                cout << "Enter destination user ID: ";
                int dest_id;
                if (!(cin >> dest_id))
                {
                    cin.clear();
                    getline(cin, line);
                    cout << "Invalid ID.\n";
                    continue;
                }
                getline(cin, line);
                users.print_shortest_path(src_id, dest_id);
            }
            else
            {
                cout << "Unknown option. Try again.\n";
            }
        }
    }
    return 0;
}
