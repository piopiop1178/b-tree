#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIN_DEGREE 3
#define MAX_KEY (MIN_DEGREE*2 - 1)

typedef struct _node {
    bool is_leaf;
    int key[MAX_KEY + 1], key_count;
    struct _node *linker[MAX_KEY + 2];
} node;

node *node_create();
void b_tree_create(node **root);
void b_tree_insert(node **root, int k);
void b_tree_delete(node *sub_root, node **root, int k);
void b_tree_search(node *sub_root, int k);
void node_insert(node *sub_root, int k);
void node_split(node *parent, int child_index);
void node_delete(node *sub_root, int k);
void move_key_right_to_left(node *left_child, node *right_child, int *parent_key);
void move_key_left_to_right(node *left_child, node *right_child, int *parent_key);
void bind_node(node *parent, node *left_child, node *right_child, int index);
void display(node *cur_node, int blanks);
void test_case(node **root);
int PRED(node *pred_child);
int SUCC(node *succ_child);

int main() {
    node *root;
    b_tree_create(&root);
    test_case(&root);
    display(root, 0);
}

void test_case(node **root) {
    int* out_arr = (int*)malloc(sizeof(int) * 1000000);
    for (int i = 0; i < 1000000; i++) {
		out_arr[i] = i;
    }
    for (int i = 0; i < 1000000; i++) 
    {
        int j = i + rand() / (RAND_MAX / (1000000 - i) + 1);
        int t = out_arr[j];
        out_arr[j] = out_arr[i];
        out_arr[i] = t;
    }
	for (int i = 0; i < 1000000; i++) {
        int r = out_arr[i];
        b_tree_insert(root, r);
    }
    for (int i = 0; i < 1000000; i++) {
        int r = out_arr[i];
        b_tree_delete(*root, root, r);
    }
}

node *node_create() {
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL) {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
    new_node->is_leaf = true;
    new_node->key_count = 0;
    return new_node;
}

void b_tree_create(node **root) {
    node *new_node = node_create();
    *root = new_node;
}

void node_split(node *parent, int child_index) {
    node *right_child = (node *)malloc(sizeof(node));
    node *left_child = parent->linker[child_index];
    right_child->is_leaf = left_child -> is_leaf;
    right_child->key_count = MIN_DEGREE - 1;
    for (int i = 1; i <= MIN_DEGREE - 1; i ++) {
        right_child->key[i] = left_child->key[i + MIN_DEGREE];
    }
    if (!left_child->is_leaf) {
        for (int i = 1; i <= MIN_DEGREE; i++) {
        right_child->linker[i] = left_child->linker[i + MIN_DEGREE];
        }
    }
    //오른쪽 자식에도 부모 추가
    right_child->linker[0] = parent;
    left_child->key_count = MIN_DEGREE - 1;
    for (int i = parent->key_count + 1; i >= child_index + 1; i--) {
        parent->linker[i + 1] = parent->linker[i];
    }
    //오른쪽 자식 넣기
    parent->linker[child_index + 1] = right_child;
    for (int i = parent->key_count; i >= child_index; i--) {
        parent->key[i + 1] = parent->key[i];
    }
    //중앙값 올리기
    parent->key[child_index] = left_child->key[MIN_DEGREE];
    parent->key_count += 1;
}

void b_tree_insert(node **root, int k) {
    node *curr_root = *root;
    if((*root)->key_count == MAX_KEY) {
        node *new_root = node_create();
        *root = new_root;
        new_root->is_leaf = false;
        new_root->linker[1] = curr_root;
        curr_root->linker[0] = new_root;
        node_split(new_root, 1);
        node_insert(new_root, k);
    }
    else {
        node_insert(curr_root, k);
    }
}

void node_insert(node *sub_root, int k) {
    int i = sub_root->key_count;
    if (sub_root->is_leaf){
        while (i >= 1 && k < sub_root->key[i]) {
            sub_root->key[i + 1] = sub_root->key[i];
            i -= 1;
        }
        sub_root->key[i + 1] = k;
        sub_root->key_count += 1;
    }
    else {
        while (i >= 1 && k < sub_root->key[i]) {
            i -= 1;
        }
        i += 1;
        if (sub_root->linker[i]->key_count == MAX_KEY) {
            node_split(sub_root, i);
            if (k > sub_root->key[i]) {
                i += 1;
            } 
        }
        node_insert(sub_root->linker[i], k);
    }
}

void b_tree_search(node *sub_root, int k){
    int i = 1;
    while(i < sub_root->key_count && k > sub_root->key[i]) {
        i = i + 1;
    }
    if (i <= sub_root->key_count && k == sub_root->key[i]) {
        printf("success find %d\n", k);
    } else if(sub_root->is_leaf == true) {
        printf("fail find %d\n", k);
    } else {
        b_tree_search(sub_root->linker[i], i);
    }
}

void b_tree_delete(node *sub_root, node **root, int k) {
    if ((*root)->key_count == 0) {
        if ((*root)->is_leaf) {
            printf("tree is empty\n");
            return;
        }
    }
    node_delete(sub_root, k);
    if ((*root)->key_count == 0) {
        if ((*root)->is_leaf) {
            printf("tree is empty\n");
            free(*root);
            b_tree_create(root);
            return;
        }
        else {
            node *old_root = *root;
            *root = (*root)->linker[1];
            free(old_root);
            return;
        }
    }
}

void node_delete(node *sub_root, int k) {
    // 리프노드일 때
    if (sub_root->is_leaf){
        int original_key_count = sub_root->key_count;
        for (int i = 1; i <= sub_root->key_count; i ++) {
            if (sub_root->key[i] == k){
                for (int j = i; j < sub_root->key_count; j ++) {
                    sub_root->key[j] = sub_root->key[j + 1];
                }
                sub_root->key_count -= 1;
            }
        }
        if (original_key_count == sub_root->key_count) {
            printf("%d not in b-tree\n", k);
        }
        return;
    } 
    // 리프노드가 아니라면, key를 순회하면서 삭제하는 값을 찾을 수 있는 위치로 이동
    int i = 1;
    while(sub_root->key[i] <  k && i <= sub_root->key_count) {
        i += 1;
    }
    // 삭제할 값을 찾았다면, PRED와 SUCC 중 하나를 찾아 교환 후 삭제 수행
    if (sub_root->key[i] == k && i <= sub_root->key_count) {
        node *left_child = sub_root->linker[i];
        node *right_child = sub_root->linker[i + 1];
        if (left_child->key_count >= MIN_DEGREE) { // 왼쪽 자식이 MIN_DEGREE 이상의 키 개수를 가지고 있으면
            int pred = PRED(left_child);
            sub_root->key[i] = pred;
            node_delete(left_child, pred);
            return;
        }
        else if (right_child->key_count >= MIN_DEGREE) { // 오른쪽 자식이 MIN_DEGREE 이상의 키 개수를 가지고 있으면
            int succ = SUCC(right_child);
            sub_root->key[i] = succ;
            node_delete(right_child, succ);
            return;
        } else {
            bind_node(sub_root, left_child, right_child, i);
            node_delete(left_child, k);
            return;
        }
        return;
    }
    // 값을 찾지 못했을 때
    if (i == sub_root->key_count + 1) { // 노드의 키 안에 k보다 큰 키가 존재하지 않아 가장 오른쪽 자식노드 검사
        node *left_child = sub_root->linker[i - 1];
        node *right_child = sub_root->linker[i];
        if (sub_root->linker[i]->key_count >= MIN_DEGREE) { // 오른쪽 자식노드의 키 개수가 t 이상일 때
            node_delete(sub_root->linker[i], k);
            return;
        }
        // 오른쪽 자식노드의 키 개수가 t 미만이어서 왼쪽 자식을 검사
        if (sub_root->linker[i - 1]->key_count >= MIN_DEGREE) { // 왼쪽 자식노드의 키 개수가 t 이상일 때 왼쪽 노드의 가장 마지막 키를 가져옴
            move_key_left_to_right(left_child, right_child, &(sub_root->key[i - 1]));
            node_delete(right_child, k);
            return;
        }
        else { // 왼쪽, 오른쪽 자식 모두의 키 개수가 t개 미만이어서 부모키를 가져와 병합 수행
            bind_node(sub_root, left_child, right_child, i - 1);
            node_delete(left_child, k);
            return; 
        }
        return;
    }
    // 가장 오른쪽 노드를 검사하는 경우가 아니라면
    else {
        node *left_child = sub_root->linker[i];
        node *right_child = sub_root->linker[i + 1];
        if (sub_root->linker[i]->key_count >= MIN_DEGREE) { // 왼쪽 자식 노드의 key 개수가 t개 이상
            node_delete(sub_root->linker[i], k);
            return;
        }
        // 왼쪽 자식 노드의 key 개수가 t개 미만이어서 오른쪽 자식을 검사
        if (sub_root->linker[i + 1]->key_count >= MIN_DEGREE) { // 오른쪽 자식노드의 키 개수가 t 이상일 때 오른쪽 노드의 가장 처음 키를 가져옴
            move_key_right_to_left(left_child, right_child, &(sub_root->key[i]));
            node_delete(left_child, k);
            return;
        }
        else { // 왼쪽, 오른쪽 자식노드의 key 개수가 t개 미만이기 때문에 부모키를 가져와 병합을 수행
            bind_node(sub_root, left_child, right_child, i);
            node_delete(sub_root->linker[i], k);
            return;
        }
        return;
    }
}

void move_key_right_to_left(node *left_child, node *right_child, int *parent_key) {
    left_child->key[MIN_DEGREE] = *parent_key;
    left_child->key_count += 1;
    *parent_key = right_child->key[1];
    right_child->key_count -= 1;
    left_child->linker[MIN_DEGREE + 1] = right_child->linker[1];
    for (int j = 1; j <= right_child->key_count; j++) {
        right_child->key[j] = right_child->key[j + 1];
    }
    if (!left_child->is_leaf) {
        for (int j = 1; j <= (right_child->key_count) + 1; j++ ) {
            right_child->linker[j] = right_child->linker[j + 1];
        }
    }
}

void move_key_left_to_right(node *left_child, node *right_child, int *parent_key) {
    for (int j = right_child->key_count; j >= 1; j--) {
        right_child->key[j + 1] = right_child->key[j];
    }
    int left_sibling_key_count = left_child->key_count;
    if (!right_child->is_leaf){
        for (int j = (right_child->key_count) + 1; j >= 1; j--) {
            right_child->linker[j + 1] = right_child->linker[j];
        }
        right_child->linker[1] = left_child->linker[left_sibling_key_count + 1];
    }
    right_child->key_count += 1;
    right_child->key[1] = *parent_key;
    *parent_key = left_child->key[left_sibling_key_count];
    left_child->key_count -= 1;
}

void bind_node(node *parent, node *left_child, node *right_child, int index) {
    left_child->key[left_child->key_count + 1] = parent->key[index];
    for (int j = index; j < parent->key_count; j++) {
        parent->key[j] = parent->key[j + 1];
    }
    for (int j = index + 1; j <= parent->key_count; j++) {
        parent->linker[j] = parent->linker[j + 1];
    }
    parent->key_count -= 1;
    for (int j = 1; j <= right_child->key_count; j++) {
        left_child->key[MIN_DEGREE + j] = right_child->key[j];
    }
    if (!left_child->is_leaf) {
        for (int j = 1; j <= (right_child->key_count) + 1; j++) {
            left_child->linker[MIN_DEGREE + j] = right_child->linker[j];
        }
    } 
    left_child->key_count += (right_child->key_count + 1);
    free(right_child);
}

int PRED (node *pred_child) {
    if (pred_child->is_leaf) {
        return pred_child->key[pred_child->key_count];
    } else {
        return PRED(pred_child->linker[(pred_child->key_count) + 1]);
    }
}

int SUCC (node *succ_child) {
    if (succ_child->is_leaf) {
        return succ_child->key[1];
    } else {
        return SUCC(succ_child->linker[1]);
    }
}

void display(node *cur_node, int blanks) {
    int i;
    if (cur_node->key_count == 0) {
        printf("tree is empty\n");
        return;
    }
    if (cur_node->is_leaf) {
        for (i = 1; i <= cur_node->key_count; i++) {
            for (int j = 1; j <= blanks; j ++)
                printf("---!");
            printf("%d\n", cur_node->key[i]);
        }
        return;
    }
    for (i = 1; i <= cur_node->key_count; i++) {
        display(cur_node->linker[i], blanks + 1);
        for (int j = 1; j <= blanks; j ++)
            printf("---!");
        printf("%d\n", cur_node->key[i]);
    }
    display(cur_node->linker[i], blanks + 1);
    return;
}