#ifndef NARY_H
#define NARY_H

struct nary_tree_node
{
    char* data;
    struct nary_tree_node* sibling;
    struct nary_tree_node* firstChild;
};

struct fs_node_entry
{
    char node_id[100];
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};

void insert(struct nary_tree_node** root, char* path);
void traverse(struct nary_tree_node* root, int tab);

#endif