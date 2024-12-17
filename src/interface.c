// ONLY MODIFY THE "apply_move_to_tree" FUNCTION

#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

int get_random_move(GameState *gs)
{
    // This function assumes that there is at least one available move
    bool moves[gs->width];
    memset(moves, false, gs->width * sizeof(bool));
    int move_count = available_moves(gs, moves);

    return rand() % move_count;
}

int get_human_move(GameState *gs)
{
    bool moves[gs->width];
    memset(moves, 0, gs->width * sizeof(bool));
    available_moves(gs, moves);
    
    printf("Available moves: ");
    for (int i = 0; i < gs->width; i++)
    {
        if (moves[i])
            printf("%d ", i);
    }

    char c;
    printf("\nEnter your move: ");
    c = getchar();
    getchar(); // dump newline character
    int human_move = c - '0';

    // get the index of the move among the children, not the all moves
    int move_among_children = 0;

    for (int i = 0; i <= human_move; i++)
    {
        if (moves[i])
        {
            move_among_children++;
        }
    }
    
    return move_among_children - 1;
}

void apply_move_to_tree(TreeNode **root, int move, int initial_tree_depth) {
    if (root == NULL || *root == NULL) {
        printf("Error: Root is NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (move < 0 || move >= (*root)->num_children || (*root)->children[move] == NULL) {
        printf("Error: Invalid move.\n");
        printf("Error: Invalid move selected by best_move: %d\n", move);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (*root)->num_children; i++) {
        if (i != move && (*root)->children[i] != NULL) {
            free_tree((*root)->children[i]); // Free unchosen subtrees
            (*root)->children[i] = NULL;
        }
    }

    TreeNode *new_root = (*root)->children[move];
    if (!new_root) {
        GameState *new_state = make_move((*root)->game_state, move);
        new_root = init_node(new_state);
    }

    free((*root)->children);
    free(*root);

    *root = new_root;

    expand_tree(*root);

    int current_node_count = node_count(*root);
    int threshold = pow((*root)->game_state->width, initial_tree_depth - 2);

    if (current_node_count < threshold) {
        expand_tree(*root);
    }
}


void play_game(int game_width, int game_height, int tree_depth, bool play_against_human)
{
    GameState *state = init_game_state(game_width, game_height);
    TreeNode *root = init_tree(state, tree_depth);
    int move;

    while (get_game_status(root->game_state) == IN_PROGRESS)
    {   
        //fprintf(stderr,"%d" ,get_game_status(root->game_state));
        if (root->game_state->next_turn) // Player 2
        {
            if (play_against_human)
                move = get_human_move(root->game_state);
            else
                move = get_random_move(root->game_state);
        }
        else // Player 1
        {
            move = best_move(root);
            printf("Best move selected: %d\n", move);
        }

        apply_move_to_tree(&root, move, tree_depth);

        print_game_state(root->game_state);

    }
    // Declare the winner
    if (get_game_status(root->game_state) == DRAW)
        printf("It is a draw\n");
    else
        printf("Player %d won!\n", get_game_status(root->game_state) + 1);

    free_tree(root);
}
