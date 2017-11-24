#ifndef NARY_H
#define NARY_H

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
    int fs_type;
};

struct nary_tree_node
{
    struct fs_node_entry data;
    struct nary_tree_node* sibling;
    struct nary_tree_node* firstChild;
};

void insert(struct nary_tree_node** root, struct fs_node_entry data);
void traverse(struct nary_tree_node* root, int tab);
int checkIfExists(struct nary_tree_node* root, char* path);
#endif