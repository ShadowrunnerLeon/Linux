#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

static bool RootAddFlag = 1;
static pthread_mutex_t pmutex_tree_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t pmutex_node = PTHREAD_MUTEX_INITIALIZER;

struct timespec tm = {1, 0};

struct tree {
    char *key;
    void *value;
    struct tree *left;
    struct tree *right;
    pthread_mutex_t pmutex;
};

struct pthread_arg {
    pthread_t id;
    struct tree *node;
    char *key;
    void **value;
} pthread[8];

struct tree *initialize() {

    if (pthread_mutex_lock(&pmutex_tree_init))
        err_msg("pthread_mutex_lock");

    struct tree *node = (struct tree*)malloc(sizeof(struct tree));
    if (pthread_mutex_init(&node->pmutex, NULL))
        err_msg("pthread_mutex_init");
    node->left = NULL;
    node->right = NULL;
    
    if (pthread_mutex_unlock(&pmutex_tree_init))
        err_msg("pthread_mutex_unlock");
    
    return node;

};

void *add(void *arg) {

    struct pthread_arg *pth_arg = (struct pthread_arg*)arg;
    struct tree *node = pth_arg->node;
    char *key = pth_arg->key;
    void *value = (void*)pth_arg->value;

    if (RootAddFlag) {

        if (pthread_mutex_lock(&node->pmutex))
            err_msg("pthread_mutex_lock");

        node->key = key;
        node->value = value;
        RootAddFlag = 0;

        if (pthread_mutex_unlock(&node->pmutex))
            err_msg("pthread_mutex_unlock");

    }

    else {

        struct tree *new_node = initialize();

        while(1) {

            if ((strcmp(key, node->key) > 0) || (strcmp(key, node->key) == 0 && value >= node->value)) {

                if (pthread_mutex_lock(&new_node->pmutex))
                    err_msg("pthread_mutex_lock");

                if (node->right==NULL) {
                
                    node->right = new_node;
                    node->right->key = key;
                    node->right->value = value;

                    if (pthread_mutex_unlock(&new_node->pmutex))
                        err_msg("pthread_mutex_unlock");
                
                    break;

                }
                
                else 
                    node = node->right;

                if (pthread_mutex_unlock(&new_node->pmutex))
                    err_msg("pthread_mutex_unlock");
            }

            else if ((strcmp(key, node->key) < 0) || (strcmp(key, node->key) == 0 && value < node->value)) {
            
                if (pthread_mutex_lock(&new_node->pmutex))
                    err_msg("pthread_mutex_lock");

                if (node->left==NULL) {

                    node->left = new_node;
                    node->left->key = key;
                    node->left->value = value;

                    if (pthread_mutex_unlock(&new_node->pmutex))
                        err_msg("pthread_mutex_unlock");
                    
                    break;

                }
                
                else 
                    node = node->left;

                if (pthread_mutex_unlock(&new_node->pmutex))
                    err_msg("pthread_mutex_unlock");

            };
        
        };

    };

};


//Последние слева и справа  +
//Слева                     +
//Справа                    +
//Корень                    +

bool lookup(void *arg) {

    struct pthread_arg *pth_arg = (struct pthread_arg*)arg;
    struct tree *node = pth_arg->node;
    char *key = pth_arg->key;
    void **value = pth_arg->value; 
    
    while (node!=NULL) {

        if ((strcmp(key, node->key) > 0) || (strcmp(key, node->key) == 0 && value > (void**)node->value))
            node = node->right;
        else if ((strcmp(key, node->key) < 0) || (strcmp(key, node->key) == 0 && value < (void**)node->value))
            node = node->left;
        else 
            return true;
            
    };

    return false;
};

void treeprint(struct tree *node) {

    if (node!=NULL) {
        treeprint(node->left);
        printf("%s, %d\n", node->key, (int)node->value);
        treeprint(node->right);
    };

};

//Последние слева и справа  +
//Слева                     +
//Справа                    +
//Корень                    +

struct tree *delete(void *arg) {
    
    struct pthread_arg *pth_arg = (struct pthread_arg*)arg;
    char *key = pth_arg->key;

    bool RootDeleteFlag = 1, RootRightLeft = 0;  //Right - 0, Left - 1
    struct tree *prevnode;


    if (pthread_mutex_lock(&pmutex_node))
        err_msg("pthread_mutex_lock");

    struct tree *node = pth_arg->node;

    while (node!=NULL) {

        if (strcmp(key, node->key) > 0) {

            RootRightLeft = 0;
            prevnode = node;
            node = node->right;

        }

        else if (strcmp(key, node->key) < 0) {

            RootRightLeft = 1;
            prevnode = node;
            node = node->left;

        }

        else {
            
            struct tree *limit_tree = node->right;

            if (RootRightLeft && !RootDeleteFlag && node->right==NULL)
                prevnode->left = node->left;
            else if (RootRightLeft && !RootDeleteFlag)
                prevnode->left = node->right;
            else if (!RootRightLeft && !RootDeleteFlag)
                prevnode->right = node->right;

            if (limit_tree!=NULL) {
                while (limit_tree->left!=NULL)
                    limit_tree = limit_tree->left;
                limit_tree->left = node->left;
            };

            if (RootDeleteFlag) 
                pth_arg->node = node->right;    
                
            if (pthread_mutex_destroy(&node->pmutex))
                err_msg("pthread_mutex_destroy");

            free(node);

            if (pthread_mutex_unlock(&pmutex_node))
                err_msg("pthread_mutex_unlock");    

            return (struct tree*)0;
            
        };

        RootDeleteFlag = 0;
        
    };

    if (pthread_mutex_unlock(&pmutex_node))
        err_msg("pthread_mutex_unlock");

    return (struct tree*)-1;
};

int main() {
    
    struct tree *root = initialize();
    
    char *str[8] = {"aab", "abc", "aaa", "bcc", "bac", "bba", "cab", "aaa"};

    for (int i=0; i<8; i++) {
        pthread[i].node = root;
        pthread[i].key = str[i];
        pthread[i].value = (void**)(i+100);
        if (pthread_create(&pthread[i].id, NULL, add, (void*)&pthread[i]))
            err_msg("pthread_create");
    };

    for (int i=0; i<8; i++)
        if (pthread_join(pthread[i].id, NULL))
            err_msg("pthread_join");

    for (int i=4; i<8; i++)
        if (pthread_create(&pthread[i].id, NULL, delete, (void*)&pthread[i]))
            err_msg("pthread_create");

    struct tree *retval;
    for (int i=4; i<8; i++) {
        if (pthread_join(pthread[i].id, (void**)&retval))
            err_msg("pthread_join");
        if (retval==(struct tree*)-1)
            printf("Node for delete not found\n");
    };

    treeprint(root);

}