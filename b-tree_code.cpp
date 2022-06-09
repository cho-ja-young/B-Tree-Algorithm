/*
  프로그램 설명 : C++로 B-Tree 구현
*/

#include <chrono>
#include <iomanip>
#include <iostream> 
#include <random>
#include <vector>

using namespace std;
namespace ch = std::chrono;

// BTree의 노드 구조체를 선언
class TreeNode {
    int* keys; // 노드에 들어갈 자료의 배열
    int t;  // Minimum degree
    TreeNode** C; // 노드 포인트의 배열
    int n; // 자료의 개수를 의미
    bool leaf; // 리프 노드인지 확인

public:
    TreeNode(int temp, bool bool_leaf); // 노드를 초기화하는 함수

    void insertNonFull(int k); // 원소를 삽입하는 함수
    void splitChild(int i, TreeNode* y); // 자식을 분할하는 함수
    void traverse(); // 현재 트리를 보여주는 순회 함수

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

    void traverse() { // 현재 트리를 보여주는 순회 함수
        if (root != NULL)
            root->traverse();
    }

    void insert(int k);
};

TreeNode::TreeNode(int t1, bool leaf1) { // 노드를 초기화하는 함수
    t = t1; // minimum degree
    leaf = leaf1; // 기본적으로 리프 노드

    keys = new int[2 * t - 1]; // 데이터 할당
    C = new TreeNode * [2 * t]; // 객체를 할당

    n = 0; // 자료의 개수는 0개
}

void TreeNode::traverse() { // 현재 트리를 보여주는 순회 함수
    int i;
    // 첫번째 자식 노드부터 가장 마지막 자식 노드까지 전부 밑으로 내려간다.
    for (i = 0; i < n; i++) {
        if (leaf == false)
            C[i]->traverse();
        // 데이터를 출력
        cout << " " << keys[i];
    }
    // 리프 노드가 아니라면 더 밑으로 내려간다.
    if (leaf == false)
        C[i]->traverse();
}

// k라는 원소를 삽입하는 함수
void BTree::insert(int k) {
    // 만약에 루트 노드가 NULL이라면 
    if (root == NULL) {
        root = new TreeNode(t, true);
        root->keys[0] = k;
        root->n = 1;
    }
    // 루트 노드가 NULL이 아니라면 
    else {
        // 현재 리프노드이고 크기가 꽉찼다면
        if (root->n == 2 * t - 1) {
            TreeNode* s = new TreeNode(t, false);

            s->C[0] = root;

            s->splitChild(0, root);

            int i = 0;
            // 값을 이동하며 삽입될 자리를 찾는디.
            if (s->keys[0] < k)
                i++;
            s->C[i]->insertNonFull(k);

            root = s;
        }
        // 리프노드가 아닐 때
        else
            root->insertNonFull(k);
    }
}
// 가득 차있지 않을 때, 삽입 함수 
void TreeNode::insertNonFull(int k) {
    int i = n - 1;

    // 현재 리프 노드 일 때
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
            // 리프 노드 위에서도 만약에 최대치를 넘으면 분할
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k);
    }
}

// 자식을 분할하는 함수
void TreeNode::splitChild(int i, TreeNode* y) {
    // 노드를 분할하여 각각 할당
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

    // 시드값을 얻기 위한 random_device 생성.
    std::random_device rd;

    // random_device 를 통해 난수 생성 엔진을 초기화 한다.
    std::mt19937 gen(rd());

    // 0 부터 9999 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
    std::uniform_int_distribution<int> dis(0, 9999);

    cout << "*** minimum_degree(t)가 " << minimum_degree << "일 경우\n" << endl;
    cout << "삽입할 원소의 개수를 입력하세요 : ";
    cin >> n;

    // system_clock 이 1 초에 1 tick
    // high_resolution_clock 의 경우 0.00000001 초 마다 1 tick 

    /*
    std::chrono::time_point<std::chrono::high_resolution_clock> start =
        std::chrono::high_resolution_clock::now();
    */
    std::chrono::time_point<std::chrono::system_clock> start =
        std::chrono::system_clock::now();

    for (i = 0; i < n; i++)
    {
        // cout << "원소를 입력하세요 : ";
        // cin >> t;

        // std::cout << "Ramdom Num : " << dis(gen) << std::endl;

        t.insert(dis(gen));
    }

    cout << "\n" << endl;
    cout << "트리를 순회합니다. \n\n* B-tree 는 : \n\n";
    t.traverse();

    /*
    std::chrono::time_point<std::chrono::high_resolution_clock> end =
        std::chrono::high_resolution_clock::now();
    */
    std::chrono::time_point<std::chrono::system_clock> end =
        std::chrono::system_clock::now();

    auto diff = end - start;

    // 소수점 아래 10자리까지 나타내기 
    cout << fixed;
    cout.precision(10);
    cout << "\n" << endl;
    cout << "걸린 시간 : " << (diff.count()) * 0.00000001 << "초" << endl;
    
}
