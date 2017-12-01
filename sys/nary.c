#include <sys/alloc.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/string.h>

struct nary_tree_node* nary_root = NULL;

struct nary_tree_node*
createNode(struct fs_node_entry data)
{
    struct nary_tree_node* nary_node = kmalloc(sizeof(struct nary_tree_node));
    strcpy((nary_node->data).node_id, data.node_id);
    strcpy((nary_node->data).name, data.name);
    strcpy((nary_node->data).mode, data.mode);
    strcpy((nary_node->data).uid, data.uid);
    strcpy((nary_node->data).gid, data.gid);
    strcpy((nary_node->data).mtime, data.mtime);
    strcpy((nary_node->data).checksum, data.checksum);
    strcpy((nary_node->data).typeflag, data.typeflag);
    strcpy((nary_node->data).linkname, data.linkname);
    strcpy((nary_node->data).magic, data.magic);
    strcpy((nary_node->data).version, data.version);
    strcpy((nary_node->data).uname, data.uname);
    strcpy((nary_node->data).gname, data.gname);
    strcpy((nary_node->data).devmajor, data.devmajor);
    strcpy((nary_node->data).devminor, data.devminor);
    strcpy((nary_node->data).prefix, data.prefix);
    strcpy((nary_node->data).pad, data.pad);
    (nary_node->data).struct_address = data.struct_address;
    (nary_node->data).fs_type = data.fs_type;
    (nary_node->data).size = data.size;
    nary_node->sibling = NULL;
    nary_node->firstChild = NULL;
    return nary_node;
}

void
calcPaths(char* path, char** subPath, char** remPath)
{
    char* slashPointOnwards = strchr(path, '/');
    if (slashPointOnwards != 0) {
        int subPathLength = (int)(strlen(path) - strlen(slashPointOnwards));
        *subPath = kmalloc(subPathLength + 1);
        strncpy(*subPath, path, subPathLength);
        *remPath = kmalloc(strlen(slashPointOnwards));
        *remPath = slashPointOnwards + 1;
    } else {
        *subPath = kmalloc(strlen(path));
        strcpy(*subPath, path);
        *remPath = NULL;
    }
}

void
insertInPath(struct nary_tree_node* root, struct fs_node_entry data)
{
    /*kprintf("Insert %s <-> %s\n", data.name, data.node_id);*/
    char* subPath = NULL;
    char* remPath = NULL;
    char* path = data.name;
    while (1) {
        calcPaths(path, &subPath, &remPath);
        while (1) {
            if (strcmp((root->data).node_id, subPath) == 0) {
                if (root->firstChild == NULL) {
                    root->firstChild = createNode(data);
                    /*kprintf("%s -> firstChild = %s\n", (root->data).node_id,
                            data.node_id);*/
                    return;
                }
                root = root->firstChild;
                path = remPath;
                break;
            }
            if (root->sibling == NULL) {
                root->sibling = createNode(data);
                /*kprintf("%s -> Sibling = %s\n", (root->data).node_id,
                        data.node_id);*/
                return;
            }
            root = root->sibling;
        }
    }
}

struct nary_tree_node*
findNaryNode(char* path)
{
    struct nary_tree_node* root = nary_root;
    if (root == NULL) {
        return NULL;
    }
    if ((root->data).node_id[0] != path[0]) {
        return NULL;
    }
    ++path;
    if (path[strlen(path) - 1] == '/') {
        path[strlen(path) - 1] = '\0';
    }
    char* subPath = NULL;
    char* remPath = NULL;
    while (root->firstChild != NULL) {
        calcPaths(path, &subPath, &remPath);
        if (strcmp(((root->firstChild)->data).node_id, subPath) == 0) {
            if (remPath == NULL) {
                return root->firstChild;
            }
            root = root->firstChild;
            path = remPath;
        } else {
            root = root->firstChild;
            int flag = 1;
            while (root->sibling != NULL) {
                if (strcmp(((root->sibling)->data).node_id, subPath) == 0) {
                    if (remPath == NULL) {
                        return root->sibling;
                    }
                    root = root->sibling;
                    path = remPath;
                    flag = 0;
                    break;
                }
                root = root->sibling;
            }
            if (flag == 1)
                return NULL;
        }
    }
    return NULL;
}

struct fs_node_entry*
findNaryNodeData(char* path)
{
    /*
    struct nary_tree_node* root = nary_root;
    if (root == NULL) {
        return NULL;
    }
    if ((root->data).node_id[0] != path[0]) {
        return NULL;
    }
    ++path;
    char* subPath = NULL;
    char* remPath = NULL;
    while (root->firstChild != NULL) {
        calcPaths(path, &subPath, &remPath);
        if (strcmp(((root->firstChild)->data).node_id, subPath) == 0) {
            if (remPath == NULL) {
                return &((root->firstChild)->data);
            }
            root = root->firstChild;
            path = remPath;
        } else {
            root = root->firstChild;
            int flag = 1;
            while (root->sibling != NULL) {
                if (strcmp(((root->sibling)->data).node_id, subPath) == 0) {
                    if (remPath == NULL) {
                        return &((root->sibling)->data);
                    }
                    root = root->sibling;
                    path = remPath;
                    flag = 0;
                    break;
                }
                root = root->sibling;
            }
            if (flag == 1)
                return NULL;
        }
    }
    return NULL;
    */
    struct nary_tree_node* nary_node = findNaryNode(path);
    return &(nary_node->data);
}

struct nary_tree_node*
findNthChild(struct nary_tree_node* parent, int N)
{
    struct nary_tree_node* cur = parent;
    if (cur == NULL) {
        return NULL;
    }
    if (N == 1) {
        return cur->firstChild;
    }
    if (N > 1) {
        cur = cur->firstChild;
        --N;
        while (N > 0 && cur != NULL) {
            cur = cur->sibling;
            N--;
        }
        return cur;
    }
    return NULL;
}

int
checkIfExists(char* path)
{
    struct nary_tree_node* root = nary_root;
    if (root == NULL) {
        return -1;
    }
    if ((root->data).node_id[0] != path[0]) {
        return -1;
    }
    ++path;
    char* subPath = NULL;
    char* remPath = NULL;
    while (root->firstChild != NULL) {
        calcPaths(path, &subPath, &remPath);
        if (strcmp(((root->firstChild)->data).node_id, subPath) == 0) {
            if (remPath == NULL) {
                return 0;
            }
            root = root->firstChild;
            path = remPath;
        } else {
            root = root->firstChild;
            int flag = 1;
            while (root->sibling != NULL) {
                if (strcmp(((root->sibling)->data).node_id, subPath) == 0) {
                    if (remPath == NULL) {
                        return 0;
                    }
                    root = root->sibling;
                    path = remPath;
                    flag = 0;
                    break;
                }
                root = root->sibling;
            }
            if (flag == 1)
                return -1;
        }
    }
    return -1;
}

int
delete_nary_node(char* path)
{
    struct nary_tree_node* root = nary_root;
    if (root == NULL)
        return -1;
    if ((root->data).node_id[0] != path[0]) {
        return -1;
    }
    ++path;
    char* subPath = NULL;
    char* remPath = NULL;
    while (root->firstChild != NULL) {
        calcPaths(path, &subPath, &remPath);
        if (strcmp(((root->firstChild)->data).node_id, subPath) == 0) {
            if (remPath == NULL) {
                struct nary_tree_node* temp = root->firstChild;
                root->firstChild = root->firstChild->sibling;
                if (root->firstChild == NULL &&
                    strcmp((root->data).node_id, "/") == 0) {
                    kfree(root);
                    nary_root = NULL;
                }
                kfree(temp);
                return 0;
            }
            root = root->firstChild;
            path = remPath;
        } else {
            root = root->firstChild;
            int flag = 1;
            while (root->sibling != NULL) {
                if (strcmp(((root->sibling)->data).node_id, subPath) == 0) {
                    if (remPath == NULL) {
                        struct nary_tree_node* temp = root->sibling;
                        root->sibling = (root->sibling)->sibling;
                        kfree(temp);
                        return 0;
                    }
                    root = root->sibling;
                    path = remPath;
                    flag = 0;
                    break;
                }
                root = root->sibling;
            }
            if (flag == 1)
                return -1;
        }
    }
    return -1;
}

void
traverse(struct nary_tree_node* root, int tab)
{
    int td = tab;
    if (root != NULL) {
        while (td > 0) {
            kprintf("    ");
            td--;
        }
        kprintf("%s\n", (root->data).node_id);
        traverse(root->firstChild, tab + 1);
        traverse(root->sibling, tab);
    }
}

void
traverse_nary_tree()
{
    traverse(nary_root, 0);
}

void
insert_into_nary_tree(struct fs_node_entry data)
{
    if (nary_root == NULL) {
        struct fs_node_entry mother;
        strcpy(mother.node_id, "/");
        nary_root = createNode(mother);

        nary_root->firstChild = createNode(data);
        /*kprintf("%s -> firstChild = %s\n", ((*root)->data).node_id,
                data.node_id);*/
        return;
    }
    insertInPath(nary_root->firstChild, data);
}
