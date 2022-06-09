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

    // 삭제 구현하는 부분에서 추가된 함수들 
    int findKey(int k); // k보다 크거나 같은 키를 갖는 인덱스를 찾기
    void deletion(int k);
    void removeFromLeaf(int idx); // 리프 노드일 때 keys[idx]를 지우는 방법 
    void removeFromNonLeaf(int idx); // 리프노드가 아닐 떄 keys[idx]를 지우는 방법
    int getPredecessor(int idx); // 전임자를 가져온다.
    int getSuccessor(int idx); // 후임자를 가져온다.
    void fill(int idx); // 현재 노드의 키를 보충
    void borrowFromPrev(int idx); // 왼쪽에서 보충
    void borrowFromNext(int idx); // 오른쪽에서 보충
    void merge(int idx); // children[idx]와 chilren[idx+1]를 병합

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
    void deletion(int k);
};

// B tree node
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
        // 크기가 꽉찼다면
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
        // 자리를 찾는다
        while (i >= 0 && keys[i] > k) {
            // 키를 뒤로 민다.
            keys[i + 1] = keys[i];
            i--;
        }

        // 키를 삽입한다.
        keys[i + 1] = k;
        n = n + 1;
    }
    // 리프 노드가 아닐 경우
    else {
        // 자리를 찾는다
        while (i >= 0 && keys[i] > k)
            i--;

        // 삽입할 자식이 꽉 찬 상태라면
        if (C[i + 1]->n == 2 * t - 1) {
            // 리프 노드 위에서도 만약에 최대치를 넘으면 분할
            splitChild(i + 1, C[i + 1]);

            // i+1에 있는 key는 새로 삽입된 키
            // 이 키보다 큰 지 확인한다.
            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k);
    }
}

// key 를 찾는 함수 
int TreeNode::findKey(int k) {
    // 내부 키 중 k를 가지고 있을 것 같은 노드 찾기
    int idx = 0;
    // 해당 노드에서 k가 있을 key위치를 찾기
    while (idx < n && keys[idx] < k)
        ++idx; // 해당 키 찾을 때까지 idx 찾을 때까지 증가.
    return idx;
}

// 삭제 수행 함수 
// 이 노드를 루트로 하는 서브트리에서 k를 삭제
void TreeNode::deletion(int k) {
    int idx = findKey(k); // k 에 해당하는 idx 찾기 

    // 만약 지워야할 인덱스가 현재 노드에 있다면,
    if (idx < n && keys[idx] == k) {
        if (leaf)
            removeFromLeaf(idx); // Leaf에서 해당 idx 삭제 수행 
        else
            removeFromNonLeaf(idx); // Leaf에서 해당 idx 삭제 수행 
    }
    else { // k에 해당하는 idx 찾지 못했을 경우 
        if (leaf) {
            cout << "키 노드 " << k << " 는(은) 트리에 존재하지 않습니다!\n";
            return;
        }

        // 리프 노드인지에 따라 다른 연산을 수행
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

// leaf에서 제거하는 함수 
void TreeNode::removeFromLeaf(int idx) {
    // 키를 앞쪽으로 민다.
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    n--; // 자료 개수 줄이기 

    return;
}

// leaf node 아닌 경우 제거 함수 
void TreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx];

    /*
        만약 k보다 작은 키를 갖는 자식노드 C[idx]가 t 키 이상을 갖고 있다면,
        C[idx]를 루트로 하는 서브 트리에서 k의 predecessor 인덱스를 구한다.
        k를 pred로 치환하고 C[idx]에서 pred를 삭제한다.
    */

    // 키 수가 최소 숫자(t)(minimum degree) 를 만족함
    if (C[idx]->n >= t) {
        int pred = getPredecessor(idx);
        keys[idx] = pred;
        C[idx]->deletion(pred);
    }

    /*
        만약 k보다 작은 키를 갖는 자식노드 C[idx]가 t 보다 적은 키를 갖고 있다면,
        C[idx+1]를 루트로 하는 서브 트리에서 k의 successor 인덱스를 구한다.
        k를 succ로 치환하고 C[idx+1]에서 succ를 삭제한다.
   */

    else if (C[idx + 1]->n >= t) {
        int succ = getSuccessor(idx);
        keys[idx] = succ;
        C[idx + 1]->deletion(succ);
    }

    /*
        C[idx]와 C[idx+1]가 t보다는 적은 키를 가지고 있다면,
        k와 C[idx+1]를 C[idx]에 병합한다.
        이제 C[idx]는 2*t-1 키를 가지고 있다.
        C[idx+1]를 지우고 C[idx]에서 k를 지운다.
    */

    else {
        merge(idx);
        C[idx]->deletion(k);
    }
    return;
}

// 자식을 분할하는 함수
void TreeNode::splitChild(int i, TreeNode* y) {
    // 노드를 분할하여 각각 할당
    TreeNode* z = new TreeNode(y->t, y->leaf);
    z->n = t - 1;

    /*
    키 = 2 x t - 1
       -> 자식 키 : t -1
       -> 본인 키 : 1
    */

    // 키 복사
    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    // 자식들 복사
    if (y->leaf == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    y->n = t - 1;

    // right를 삽입하기 위해 기존 자식들과 키를 뒤로 민다.
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    // 분리하는 자식의 가운데 키를 삽입하고 오른쪽 자식을 삽입한다.

    keys[i] = y->keys[t - 1];

    // 키 수 증가
    n = n + 1;
}

// keys[idx]를 기준으로 그 다음으로 작은 키를 
// 왼쪽 서브트리에서 탐색해온다.
int TreeNode::getPredecessor(int idx) {
    TreeNode* cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    return cur->keys[cur->n - 1];
}

// keys[idx]를 기준으로 그 다음으로 큰 키를 
// 오른쪽 서브트리에서 탐색해온다.
int TreeNode::getSuccessor(int idx) {
    TreeNode* cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    return cur->keys[0];
}

// 현재 노드의 키 수가 불충분하므로 키를 보충
void TreeNode::fill(int idx) {
    // 왼쪽이 키 수가 충분히 많다면 왼쪽에서 빌려온다.
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);

    // 오른쪽 키 수가 충분히 많다면 오른쪽에서 빌려온다.
    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);

    // 가져올 수 없는 경우 C[idx]를 형제 노드와 병합
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// C[idx-1] 에서 key를 빌려와 C[idx]에 삽입
void TreeNode::borrowFromPrev(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx - 1];

    /*
    C[idx-1] 에서 맨 오른쪽에 있는 키를 빌려와 key[idx-1]에 넣고
    기존 key[idx-1]를 C[idx]의 맨 왼쪽에 넣는 방식이다.
    형제는 키 하나를 잃고 자식은 키 하나를 얻는다
   */

   // C[idx]에 있는 키를 한 칸 민다.
    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->leaf) {

        // 자식 노드도 마찬가지
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    // child의 첫번째 키를 key[idx-1]로 치환
    child->keys[0] = keys[idx - 1];

    // 자식도 같이 움직인다.
    if (!child->leaf)
        child->C[0] = sibling->C[sibling->n];

    // sibling의 마지막 키를 key[idx-1]로 치환
    keys[idx - 1] = sibling->keys[sibling->n - 1];

    // 자식과 형제 키 수를 조정
    child->n += 1;
    sibling->n -= 1;

    return;
}

// C[idx+1] 에서 key를 빌려와 C[idx]에 삽입
void TreeNode::borrowFromNext(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx + 1];

    // C의 키를 조정한다
    child->keys[(child->n)] = keys[idx];

    if (!(child->leaf))
        // 자식 노드도 마찬가지로 조정
        child->C[(child->n) + 1] = sibling->C[0];

    // keys[idx]를 sibling의 첫번째 키로 치환한다.
    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf) {

        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    // 자식과 형제 키 수를 조정
    child->n += 1;
    sibling->n -= 1;

    return;
}

// 병합하는 함수 
// : C[idx] 키 수가 부족한 상황에 빌려오는 것도 불가능할 때에 사용
// : C[idx] 와 C[idx+1]를 합친다.
// : C[idx+1]은 연산 후에 삭제된다. 
void TreeNode::merge(int idx) {
    TreeNode* child = C[idx];
    TreeNode* sibling = C[idx + 1];

    // 키를 왼쪽 자식에 밀어 넣는다.
    child->keys[t - 1] = keys[idx];

    // 형제 키를 복사하여 넣는다.
    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    // 형제 자식노드를 복사하여 넣는다.
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    // 기존 키 하나를 뺏으므로 기존 키 배열을 조정
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    // C[idx+1]을 삭제할 것이므로 자식 노드도 움직인다.
    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    // 자식과 현재 노드의 키 수를 업데이트
    child->n += sibling->n + 1;
    n--;

    delete (sibling);

    return;
}

// Delete 함수
void BTree::deletion(int k) {
    if (!root) {
        cout << "트리는 현재 비어있습니다!\n";
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

    // 시드값을 얻기 위한 random_device 생성.
    std::random_device rd;

    // random_device 를 통해 난수 생성 엔진을 초기화 한다.
    std::mt19937 gen(rd());

    // 0 부터 9999 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
    std::uniform_int_distribution<int> dis(0, 9999);

    cout << "*** minimum_degree(t)가 " << minimum_degree << "일 경우\n" << endl;
    cout << "삽입할 데이터의 개수를 입력하세요 : ";
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
        t.insert(dis(gen));
    }

    cout << "\n" << endl;
    cout << "트리를 순회합니다. \n\n* (삽입 후) B-tree 는 : \n\n";
    t.traverse();
    cout << "\n" << endl;

    cout << "* (데이터 500개 삭제를 시작합니다.) \n\n";

    for (i = 0; i < 500; i++)
    {
        t.deletion(dis(gen)); // 데이터 500개 삭제
    }

    cout << "\n" << endl;
    cout << "트리를 순회합니다. \n\n* (삭제 후) B-tree 는 : \n\n";
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
    std::cout << "노드 삽입 및 삭제에 걸린 시간 : " 
         << ch::duration_cast<ch::microseconds> (diff).count() << " microseconds" << std::endl;

    /*
        - duration_cast 는 임의의 duration 객체를 받아서 우리가 원하는 duration 으로 캐스팅 가능
        - std::chrono::microseconds 는 <chrono> 에 미리 정의되어 있는 duration 객체

        - microseconds 로 캐스팅 한뒤에 리턴하는 count 값
            -> 해당 duration 이 몇 초 인지를 나타냄
    */
    
}
