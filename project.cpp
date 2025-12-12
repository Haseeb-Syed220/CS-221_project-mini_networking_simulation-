#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

struct UserNode
{
    int id;
    string username;
    FriendList friends;
    PostTree posts;
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

class PostTree
{
private:
    PostNode *root;
    int next_post_id = 0;

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
            return node;
        }

        update_height(node);

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

            cout << "    ID " << node->post_id << " | Content: \"" << node->content << "\"" << endl;

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
    void add_post(int uid, string content)
    {
        UserNode *user = find_user(uid);
        if (user == nullptr)
        {
            cout << "Error: User with ID " << uid << " not found." << endl;
            return;
        }
        user->posts.post(content);
        cout << user->username << " posted: \"" << content << "\"" << endl;
    }
};

int main()
{
}