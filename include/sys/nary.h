#include <sys/defs.h>
#ifndef NARY_H
#define NARY_H

struct fs_node_entry
{
    char node_id[100];
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
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
    uint64_t size;
    uint64_t struct_address;
    int fs_type;
};

struct nary_tree_node
{
    struct fs_node_entry data;
    struct nary_tree_node* sibling;
    struct nary_tree_node* firstChild;
};

void insert_into_nary_tree(struct fs_node_entry data);
void traverse_nary_tree();
int checkIfExists(char* path);
struct nary_tree_node* findNaryNode(char* path);
struct fs_node_entry* findNaryNodeData(char* path);
int delete_nary_node(char* path);
struct nary_tree_node* findNthChild(struct nary_tree_node* parent, int N);
#endif
