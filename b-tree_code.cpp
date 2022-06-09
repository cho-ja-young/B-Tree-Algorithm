/*
  ���α׷� ���� : C++�� B-Tree ����
*/

#include <chrono>
#include <iomanip>
#include <iostream> 
#include <random>
#include <vector>

using namespace std;
namespace ch = std::chrono;

// BTree�� ��� ����ü�� ����
class TreeNode {
    int* keys; // ��忡 �� �ڷ��� �迭
    int t;  // Minimum degree
    TreeNode** C; // ��� ����Ʈ�� �迭
    int n; // �ڷ��� ������ �ǹ�
    bool leaf; // ���� ������� Ȯ��

public:
    TreeNode(int temp, bool bool_leaf); // ��带 �ʱ�ȭ�ϴ� �Լ�

    void insertNonFull(int k); // ���Ҹ� �����ϴ� �Լ�
    void splitChild(int i, TreeNode* y); // �ڽ��� �����ϴ� �Լ�
    void traverse(); // ���� Ʈ���� �����ִ� ��ȸ �Լ�

    friend class BTree;
};

class BTree { 
    TreeNode* root;
    int t;

public:
    BTree(int temp) {
        root = NULL;
        t = temp;
    }

    void traverse() { // ���� Ʈ���� �����ִ� ��ȸ �Լ�
        if (root != NULL)
            root->traverse();
    }

    void insert(int k);
};

TreeNode::TreeNode(int t1, bool leaf1) { // ��带 �ʱ�ȭ�ϴ� �Լ�
    t = t1; // minimum degree
    leaf = leaf1; // �⺻������ ���� ���

    keys = new int[2 * t - 1]; // ������ �Ҵ�
    C = new TreeNode * [2 * t]; // ��ü�� �Ҵ�

    n = 0; // �ڷ��� ������ 0��
}

void TreeNode::traverse() { // ���� Ʈ���� �����ִ� ��ȸ �Լ�
    int i;
    // ù��° �ڽ� ������ ���� ������ �ڽ� ������ ���� ������ ��������.
    for (i = 0; i < n; i++) {
        if (leaf == false)
            C[i]->traverse();
        // �����͸� ���
        cout << " " << keys[i];
    }
    // ���� ��尡 �ƴ϶�� �� ������ ��������.
    if (leaf == false)
        C[i]->traverse();
}

// k��� ���Ҹ� �����ϴ� �Լ�
void BTree::insert(int k) {
    // ���࿡ ��Ʈ ��尡 NULL�̶�� 
    if (root == NULL) {
        root = new TreeNode(t, true);
        root->keys[0] = k;
        root->n = 1;
    }
    // ��Ʈ ��尡 NULL�� �ƴ϶�� 
    else {
        // ���� ��������̰� ũ�Ⱑ ��á�ٸ�
        if (root->n == 2 * t - 1) {
            TreeNode* s = new TreeNode(t, false);

            s->C[0] = root;

            s->splitChild(0, root);

            int i = 0;
            // ���� �̵��ϸ� ���Ե� �ڸ��� ã�µ�.
            if (s->keys[0] < k)
                i++;
            s->C[i]->insertNonFull(k);

            root = s;
        }
        // ������尡 �ƴ� ��
        else
            root->insertNonFull(k);
    }
}
// ���� ������ ���� ��, ���� �Լ� 
void TreeNode::insertNonFull(int k) {
    int i = n - 1;

    // ���� ���� ��� �� ��
    if (leaf == true) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i--;
        }

        keys[i + 1] = k;
        n = n + 1;
    }
    else {
        while (i >= 0 && keys[i] > k)
            i--;

        if (C[i + 1]->n == 2 * t - 1) {
            // ���� ��� �������� ���࿡ �ִ�ġ�� ������ ����
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k);
    }
}

// �ڽ��� �����ϴ� �Լ�
void TreeNode::splitChild(int i, TreeNode* y) {
    // ��带 �����Ͽ� ���� �Ҵ�
    TreeNode* z = new TreeNode(y->t, y->leaf);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    if (y->leaf == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    y->n = t - 1;
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[t - 1];
    n = n + 1;
}

int main() {

    int i, n;
    int minimum_degree = 7; // Minimum degree t = 5, 7, 9 
    BTree t(minimum_degree); 

    // �õ尪�� ��� ���� random_device ����.
    std::random_device rd;

    // random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
    std::mt19937 gen(rd());

    // 0 ���� 9999 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
    std::uniform_int_distribution<int> dis(0, 9999);

    cout << "*** minimum_degree(t)�� " << minimum_degree << "�� ���\n" << endl;
    cout << "������ ������ ������ �Է��ϼ��� : ";
    cin >> n;

    // system_clock �� 1 �ʿ� 1 tick
    // high_resolution_clock �� ��� 0.00000001 �� ���� 1 tick 

    /*
    std::chrono::time_point<std::chrono::high_resolution_clock> start =
        std::chrono::high_resolution_clock::now();
    */
    std::chrono::time_point<std::chrono::system_clock> start =
        std::chrono::system_clock::now();

    for (i = 0; i < n; i++)
    {
        // cout << "���Ҹ� �Է��ϼ��� : ";
        // cin >> t;

        // std::cout << "Ramdom Num : " << dis(gen) << std::endl;

        t.insert(dis(gen));
    }

    cout << "\n" << endl;
    cout << "Ʈ���� ��ȸ�մϴ�. \n\n* B-tree �� : \n\n";
    t.traverse();

    /*
    std::chrono::time_point<std::chrono::high_resolution_clock> end =
        std::chrono::high_resolution_clock::now();
    */
    std::chrono::time_point<std::chrono::system_clock> end =
        std::chrono::system_clock::now();

    auto diff = end - start;

    // �Ҽ��� �Ʒ� 10�ڸ����� ��Ÿ���� 
    cout << fixed;
    cout.precision(10);
    cout << "\n" << endl;
    cout << "�ɸ� �ð� : " << (diff.count()) * 0.00000001 << "��" << endl;
    
}
