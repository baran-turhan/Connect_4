#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void expand(TreeNode *node, int current_depth, int depth) {
    if (current_depth >= depth) {
        return;
    }

    if (get_game_status(node->game_state) != IN_PROGRESS) {
        node->num_children = -1;
        return;
    } 
    bool *moves = (bool *)calloc(node->game_state->width, sizeof(bool)); // malloc switched with calloc
    if (!moves) {
        fprintf(stderr, "Memory allocation for moves failed.\n");
        exit(EXIT_FAILURE);
    }
    // printf("width : %d\n", node->game_state->width);
    int num_moves = available_moves(node->game_state, moves);
    // printf("num_moves : %d\n", num_moves);

    node->num_children = num_moves;
    node->children = (TreeNode **)malloc(num_moves * sizeof(TreeNode *));
    if (!node->children) {
        fprintf(stderr, "Memory allocation for children failed.\n");
        free(node->children);
        free(moves);
        exit(EXIT_FAILURE);
    }
    int j = 0;
    for (int i = 0; i < node->game_state->width; i++) {
        if (moves[i]) {
            // printf( "i : %d, j : %d\n", i, j);
            GameState *new_state = make_move(node->game_state, i);
            node->children[j] = init_node(new_state);
            // printf("node->children[%d]\n", j);
            expand(node->children[j], current_depth + 1, depth);
            j++;
        }
    }
    if (j != num_moves) {
        fprintf(stderr, "Error: j (%d) != num_moves (%d)\n", j, num_moves);
        exit(EXIT_FAILURE);
    }

    free(moves);
}

TreeNode *init_node(GameState *gs){
    TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
    if(!node){
        printf("Memory allocation for node failed.\n");
        return NULL;
    }
    node->game_state = gs;
    node->num_children = -1; // not 0
    node->children = NULL;

    return node;
}

TreeNode *init_tree(GameState *gs, int depth){
    TreeNode* root = init_node(gs);
    if (!root){
        printf("Error couldn't init_tree.\n");
        return NULL;
    }

    expand(root, 1, depth);
    return root;
}

void free_tree(TreeNode *root){
    if(!root){
        printf( "Attempted to free an empty tree.\n");
        return;
    }
    for (int i = 0; i < root->num_children; i++) {
        if (root->children[i] != NULL){
            free_tree(root->children[i]);
        }
    }
    free(root->children);
    root->children = NULL;
    free(root->game_state);
    root->game_state = NULL;
    free(root);
}

void expand_tree(TreeNode *root){
    if (!root){
        printf("Attempted to expand an empty tree.\n");
        return;
    }
    for(int i = 0; i < root->num_children; i++){
        if(root->children[i] && root->children[i]->num_children == -1){
            expand(root->children[i], 1, 2);
        }else if(root->children[i]){
            expand_tree(root->children[i]);
        }
    }

}

int node_count(TreeNode *root){
    if(!root) return 0;
    int count = 1;
    for(int i = 0; i < root->num_children; i++){
        if(root->children[i] != NULL){
            count += node_count(root->children[i]);
        }
    }
    return count;
}

void print_tree(TreeNode *root, int level){
    if(!root){
        printf("There is no root in print_tree function.\n");
        return;
    }

    printf("%*sLevel : %d, Evulation : %d, Children : %d\n", 
        level * 2, "", level, root->game_state->evaluation, root->num_children);

    for(int i = 0; i < root->num_children; i++){
        print_tree(root->children[i], level + 1);
    }
}