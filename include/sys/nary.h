#ifndef NARY_H
#define NARY_H

struct node
{
    void* data;
    struct node* sibling;
    struct node* firstChild;
};

void insert(struct node** root, char* path);
void traverse(struct node* root, int tab);

#endif