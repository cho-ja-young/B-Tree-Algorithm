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

    // ���� �����ϴ� �κп��� �߰��� �Լ��� 
    int findKey(int k); // k���� ũ�ų� ���� Ű�� ���� �ε����� ã��
    void deletion(int k);
    void removeFromLeaf(int idx); // ���� ����� �� keys[idx]�� ����� ��� 
    void removeFromNonLeaf(int idx); // ������尡 �ƴ� �� keys[idx]�� ����� ���
    int getPredecessor(int idx); // �����ڸ� �����´�.
    int getSuccessor(int idx); // �����ڸ� �����´�.
    void fill(int idx); // ���� ����� Ű�� ����
    void borrowFromPrev(int idx); // ���ʿ��� ����
    void borrowFromNext(int idx); // �����ʿ��� ����
    void merge(int idx); // children[idx]�� chilren[idx+1]�� ����

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
    void deletion(int k);
};

// B tree node
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
        // ũ�Ⱑ ��á�ٸ�
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
        // �ڸ��� ã�´�
        while (i >= 0 && keys[i] > k) {
            // Ű�� �ڷ� �δ�.
            keys[i + 1] = keys[i];
            i--;
        }

        // Ű�� �����Ѵ�.
        keys[i + 1] = k;
        n = n + 1;
    }
    // ���� ��尡 �ƴ� ���
    else {
        // �ڸ��� ã�´�
        while (i >= 0 && keys[i] > k)
            i--;

        // ������ �ڽ��� �� �� ���¶��
        if (C[i + 1]->n == 2 * t - 1) {
            // ���� ��� �������� ���࿡ �ִ�ġ�� ������ ����
            splitChild(i + 1, C[i + 1]);

            // i+1�� �ִ� key�� ���� ���Ե� Ű
            // �� Ű���� ū �� Ȯ���Ѵ�.
            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k);
    }
}

// key �� ã�� �Լ� 
int TreeNode::findKey(int k) {
    // ���� Ű �� k�� ������ ���� �� ���� ��� ã��
    int idx = 0;
    // �ش� ��忡�� k�� ���� key��ġ�� ã��
    while (idx < n && keys[idx] < k)
        ++idx; // �ش� Ű ã�� ������ idx ã�� ������ ����.
    return idx;
}

// ���� ���� �Լ� 
// �� ��带 ��Ʈ�� �ϴ� ����Ʈ������ k�� ����
void TreeNode::deletion(int k) {
    int idx = findKey(k); // k �� �ش��ϴ� idx ã�� 

    // ���� �������� �ε����� ���� ��忡 �ִٸ�,
    if (idx < n && keys[idx] == k) {
        if (leaf)
            removeFromLeaf(idx); // Leaf���� �ش� idx ���� ���� 
        else
            removeFromNonLeaf(idx); // Leaf���� �ش� idx ���� ���� 
    }
    else { // k�� �ش��ϴ� idx ã�� ������ ��� 
        if (leaf) {
            cout << "Ű ��� " << k << " ��(��) Ʈ���� �������� �ʽ��ϴ�!\n";
            return;
        }

        // ���� ��������� ���� �ٸ� ������ ����
        bool flag = ((idx == n) ? true : false);

        if (C[idx]->n < t)
            fill(idx);

        if (flag && idx > n)
            C[idx - 1]->deletion(k);
        else
            C[idx]->deletion(k);
    }
    return;
}

// leaf���� �����ϴ� �Լ� 
void TreeNode::removeFromLeaf(int idx) {
    // Ű�� �������� �δ�.
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    n--; // �ڷ� ���� ���̱� 

    return;
}

// leaf node �ƴ� ��� ���� �Լ� 
void TreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx];

    /*
        ���� k���� ���� Ű�� ���� �ڽĳ�� C[idx]�� t Ű �̻��� ���� �ִٸ�,
        C[idx]�� ��Ʈ�� �ϴ� ���� Ʈ������ k�� predecessor �ε����� ���Ѵ�.
        k�� pred�� ġȯ�ϰ� C[idx]���� pred�� �����Ѵ�.
    */

    // Ű ���� �ּ� ����(t)(minimum degree) �� ������
    if (C[idx]->n >= t) {
        int pred = getPredecessor(idx);
        keys[idx] = pred;
        C[idx]->deletion(pred);
    }

    /*
        ���� k���� ���� Ű�� ���� �ڽĳ�� C[idx]�� t ���� ���� Ű�� ���� �ִٸ�,
        C[idx+1]�� ��Ʈ�� �ϴ� ���� Ʈ������ k�� successor �ε����� ���Ѵ�.
        k�� succ�� ġȯ�ϰ� C[idx+1]���� succ�� �����Ѵ�.
   */

    else if (C[idx + 1]->n >= t) {
        int succ = getSuccessor(idx);
        keys[idx] = succ;
        C[idx + 1]->deletion(succ);
    }

    /*
        C[idx]�� C[idx+1]�� t���ٴ� ���� Ű�� ������ �ִٸ�,
        k�� C[idx+1]�� C[idx]�� �����Ѵ�.
        ���� C[idx]�� 2*t-1 Ű�� ������ �ִ�.
        C[idx+1]�� ����� C[idx]���� k�� �����.
    */

    else {
        merge(idx);
        C[idx]->deletion(k);
    }
    return;
}

// �ڽ��� �����ϴ� �Լ�
void TreeNode::splitChild(int i, TreeNode* y) {
    // ��带 �����Ͽ� ���� �Ҵ�
    TreeNode* z = new TreeNode(y->t, y->leaf);
    z->n = t - 1;

    /*
    Ű = 2 x t - 1
       -> �ڽ� Ű : t -1
       -> ���� Ű : 1
    */

    // Ű ����
    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    // �ڽĵ� ����
    if (y->leaf == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    y->n = t - 1;

    // right�� �����ϱ� ���� ���� �ڽĵ�� Ű�� �ڷ� �δ�.
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    // �и��ϴ� �ڽ��� ��� Ű�� �����ϰ� ������ �ڽ��� �����Ѵ�.

    keys[i] = y->keys[t - 1];

    // Ű �� ����
    n = n + 1;
}

// keys[idx]�� �������� �� �������� ���� Ű�� 
// ���� ����Ʈ������ Ž���ؿ´�.
int TreeNode::getPredecessor(int idx) {
    TreeNode* cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    return cur->keys[cur->n - 1];
}

// keys[idx]�� �������� �� �������� ū Ű�� 
// ������ ����Ʈ������ Ž���ؿ´�.
int TreeNode::getSuccessor(int idx) {
    TreeNode* cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    return cur->keys[0];
}

// ���� ����� Ű ���� ������ϹǷ� Ű�� ����
void TreeNode::fill(int idx) {
    // ������ Ű ���� ����� ���ٸ� ���ʿ��� �����´�.
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);

    // ������ Ű ���� ����� ���ٸ� �����ʿ��� �����´�.
    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);

    // ������ �� ���� ��� C[idx]�� ���� ���� ����
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// C[idx-1] ���� key�� ������ C[idx]�� ����
void TreeNode::borrowFromPrev(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx - 1];

    /*
    C[idx-1] ���� �� �����ʿ� �ִ� Ű�� ������ key[idx-1]�� �ְ�
    ���� key[idx-1]�� C[idx]�� �� ���ʿ� �ִ� ����̴�.
    ������ Ű �ϳ��� �Ұ� �ڽ��� Ű �ϳ��� ��´�
   */

   // C[idx]�� �ִ� Ű�� �� ĭ �δ�.
    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->leaf) {

        // �ڽ� ��嵵 ��������
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    // child�� ù��° Ű�� key[idx-1]�� ġȯ
    child->keys[0] = keys[idx - 1];

    // �ڽĵ� ���� �����δ�.
    if (!child->leaf)
        child->C[0] = sibling->C[sibling->n];

    // sibling�� ������ Ű�� key[idx-1]�� ġȯ
    keys[idx - 1] = sibling->keys[sibling->n - 1];

    // �ڽİ� ���� Ű ���� ����
    child->n += 1;
    sibling->n -= 1;

    return;
}

// C[idx+1] ���� key�� ������ C[idx]�� ����
void TreeNode::borrowFromNext(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx + 1];

    // C�� Ű�� �����Ѵ�
    child->keys[(child->n)] = keys[idx];

    if (!(child->leaf))
        // �ڽ� ��嵵 ���������� ����
        child->C[(child->n) + 1] = sibling->C[0];

    // keys[idx]�� sibling�� ù��° Ű�� ġȯ�Ѵ�.
    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf) {

        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    // �ڽİ� ���� Ű ���� ����
    child->n += 1;
    sibling->n -= 1;

    return;
}

// �����ϴ� �Լ� 
// : C[idx] Ű ���� ������ ��Ȳ�� �������� �͵� �Ұ����� ���� ���
// : C[idx] �� C[idx+1]�� ��ģ��.
// : C[idx+1]�� ���� �Ŀ� �����ȴ�. 
void TreeNode::merge(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx + 1];

    // Ű�� ���� �ڽĿ� �о� �ִ´�.
    child->keys[t - 1] = keys[idx];

    // ���� Ű�� �����Ͽ� �ִ´�.
    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    // ���� �ڽĳ�带 �����Ͽ� �ִ´�.
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    // ���� Ű �ϳ��� �����Ƿ� ���� Ű �迭�� ����
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    // C[idx+1]�� ������ ���̹Ƿ� �ڽ� ��嵵 �����δ�.
    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    // �ڽİ� ���� ����� Ű ���� ������Ʈ
    child->n += sibling->n + 1;
    n--;

    delete (sibling);

    return;
}

// Delete �Լ�
void BTree::deletion(int k) {
    if (!root) {
        cout << "Ʈ���� ���� ����ֽ��ϴ�!\n";
        return;
    }

    root->deletion(k);

    if (root->n == 0) {
        TreeNode* tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->C[0];

        delete tmp;
    }
    return;
}

int main() {

    int i, n;
    int minimum_degree = 5; // Minimum degree t = 5, 7, 9 
    BTree t(minimum_degree); 

    // �õ尪�� ��� ���� random_device ����.
    std::random_device rd;

    // random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
    std::mt19937 gen(rd());

    // 0 ���� 9999 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
    std::uniform_int_distribution<int> dis(0, 9999);

    cout << "*** minimum_degree(t)�� " << minimum_degree << "�� ���\n" << endl;
    cout << "������ �������� ������ �Է��ϼ��� : ";
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
        t.insert(dis(gen));
    }

    cout << "\n" << endl;
    cout << "Ʈ���� ��ȸ�մϴ�. \n\n* (���� ��) B-tree �� : \n\n";
    t.traverse();
    cout << "\n" << endl;

    cout << "* (������ 500�� ������ �����մϴ�.) \n\n";

    for (i = 0; i < 500; i++)
    {
        t.deletion(dis(gen)); // ������ 500�� ����
    }

    cout << "\n" << endl;
    cout << "Ʈ���� ��ȸ�մϴ�. \n\n* (���� ��) B-tree �� : \n\n";
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
    std::cout << "��� ���� �� ������ �ɸ� �ð� : " 
         << ch::duration_cast<ch::microseconds> (diff).count() << " microseconds" << std::endl;

    /*
        - duration_cast �� ������ duration ��ü�� �޾Ƽ� �츮�� ���ϴ� duration ���� ĳ���� ����
        - std::chrono::microseconds �� <chrono> �� �̸� ���ǵǾ� �ִ� duration ��ü

        - microseconds �� ĳ���� �ѵڿ� �����ϴ� count ��
            -> �ش� duration �� �� �� ������ ��Ÿ��
    */
    
}
