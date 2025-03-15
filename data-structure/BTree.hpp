#pragma once
#include <bits/stdc++.h>

namespace Atok
{
    template <typename T = int, int b = 2>
    struct BTreeNode
    {
    private:
        T *keys;
        BTreeNode<T, b> **C;
        int sz;
        bool leaf;

    public:
        BTreeNode(bool _leaf);

        void insertNonFull(T k);

        void splitChild(int i, BTreeNode *y);

        void remove(T k);

        void removeFromLeaf(int idx);

        void removeFromNonLeaf(int idx);

        T getPredecessor(int idx);

        T getSuccessor(int idx);

        void fill(int idx);

        void borrowFromPrev(int idx);

        void borrowFromNext(int idx);

        void merge(int idx);

        void traverse();

        // return key's pointer.
        BTreeNode *find(T k);

        // return index.
        int find_key(T k);

        template <typename S, int c>
        friend class BTree;
    };

    template <typename T, int b = 2>
    struct BTree
    {
    private:
        BTreeNode<T, b> *root;

    public:
        BTree() { root = nullptr; }

        void traverse()
        {
            if (root)
                root->traverse();
        }

        BTreeNode<T, b> *find(T k)
        {
            return (!root) ? nullptr : root->find(k);
        }

        void insert(T k);

        void remove(T k);
    };

    template <typename T, int b>
    BTreeNode<T, b>::BTreeNode(bool leaf1)
    {
        leaf = leaf1;

        keys = new T[2 * b - 1];
        C = new BTreeNode<T, b> *[2 * b];

        sz = 0;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::traverse()
    {
        int i;
        for (i = 0; i < sz; ++i)
        {
            if (!leaf)
            {
                C[i]->traverse();
            }
            std::cout << " " << keys[i];
        }

        if (!leaf)
        {
            C[i]->traverse();
        }
    }

    template <typename T, int b>
    BTreeNode<T, b> *BTreeNode<T, b>::find(T k)
    {
        int i = 0;
        while (i < sz && k > keys[i])
            i++;

        if (keys[i] == k)
            return this;

        if (leaf)
            return nullptr;

        return C[i]->find(k);
    }

    template <typename T, int b>
    int BTreeNode<T, b>::find_key(T k)
    {
        int idx = 0;
        while (idx < sz && keys[idx] < k)
            ++idx;
        return idx;
    }

    template <typename T, int b>
    void BTree<T, b>::insert(T k)
    {
        if (!root)
        {
            root = new BTreeNode<T, b>(true);
            root->keys[0] = k;
            root->sz = 1;
        }
        else
        {
            if (root->sz == 2 * b - 1)
            {
                BTreeNode<T, b> *s = new BTreeNode<T, b>(false);

                s->C[0] = root;

                s->splitChild(0, root);

                int i = 0;
                if (s->keys[0] < k)
                    i++;
                s->C[i]->insertNonFull(k);

                root = s;
            }
            else
            {
                root->insertNonFull(k);
            }
        }
    }

    template <typename T, int b>
    void BTree<T, b>::remove(T k)
    {
        if (!root)
        {
            return;
        }

        root->remove(k);

        if (root->sz == 0)
        {
            BTreeNode<T, b> *tmp = root;
            if (root->leaf)
            {
                root = nullptr;
            }
            else
            {
                root = root->C[0];
            }

            delete tmp;
        }

        return;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::insertNonFull(T k)
    {
        int i = sz - 1;

        if (leaf)
        {
            while (i >= 0 && keys[i] > k)
            {
                keys[i + 1] = keys[i];
                i--;
            }

            keys[i + 1] = k;
            sz = sz + 1;
        }
        else
        {
            while (i >= 0 && keys[i] > k)
                i--;

            if (C[i + 1]->sz == 2 * b - 1)
            {
                splitChild(i + 1, C[i + 1]);

                if (keys[i + 1] < k)
                    i++;
            }

            C[i + 1]->insertNonFull(k);
        }
    }

    template <typename T, int b>
    void BTreeNode<T, b>::splitChild(int i, BTreeNode<T, b> *y)
    {
        BTreeNode *z = new BTreeNode<T, b>(y->leaf);
        z->sz = b - 1;

        for (int j = 0; j < b - 1; ++j)
        {
            z->keys[j] = y->keys[j + b];
        }
        if (!(y->leaf))
        {
            for (int j = 0; j < b; ++j)
            {
                z->C[j] = y->C[j + b];
            }
        }

        y->sz = b - 1;

        for (int j = sz; j >= i + 1; --j)
        {
            C[j + 1] = C[j];
        }

        C[i + 1] = z;

        for (int j = sz - 1; j >= i; --j)
        {
            keys[j + 1] = keys[j];
        }

        keys[i] = y->keys[b - 1];

        sz = sz + 1;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::remove(T k)
    {
        int idx = find_key(k);

        if (idx < sz && keys[idx] == k)
        {
            if (leaf)
            {
                removeFromLeaf(idx);
            }
            else
            {
                removeFromNonLeaf(idx);
            }
        }
        else
        {
            if (leaf)
                return;

            bool flag = (idx == sz);

            if (C[idx]->sz < b)
            {
                fill(idx);
            }

            if (flag && idx > sz)
            {
                C[idx - 1]->remove(k);
            }
            else
            {
                C[idx]->remove(k);
            }

            return;
        }
    }

    template <typename T, int b>
    void BTreeNode<T, b>::removeFromLeaf(int idx)
    {
        for (int i = idx + 1; i < sz; ++i)
        {
            keys[i - 1] = keys[i];
        }
        sz--;
        return;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::removeFromNonLeaf(int idx)
    {
        int k = keys[idx];

        if (C[idx]->sz >= b)
        {
            T pred = getPredecessor(idx);
            keys[idx] = pred;
            C[idx]->remove(pred);
        }
        else if (C[idx + 1]->sz >= b)
        {
            T succ = getSuccessor(idx);
            keys[idx] = succ;
            C[idx + 1]->remove(succ);
        }
        else
        {
            merge(idx);
            C[idx]->remove(k);
        }

        return;
    }

    template <typename T, int b>
    T BTreeNode<T, b>::getPredecessor(int idx)
    {
        BTreeNode<T, b> *cur = C[idx];
        while (!cur->leaf)
        {
            cur = cur->C[cur->sz];
        }

        return cur->keys[cur->sz - 1];
    }

    template <typename T, int b>
    T BTreeNode<T, b>::getSuccessor(int idx)
    {
        BTreeNode<T, b> *cur = C[idx + 1];
        while (!cur->leaf)
        {
            cur = cur->C[0];
        }

        return cur->keys[0];
    }

    template <typename T, int b>
    void BTreeNode<T, b>::fill(int idx)
    {
        if (idx != 0 && C[idx - 1]->sz >= b)
        {
            borrowFromPrev(idx);
        }
        else if (idx != sz && C[idx + 1]->sz >= b)
        {
            borrowFromNext(idx);
        }
        else
        {
            if (idx != sz)
            {
                merge(idx);
            }
            else
            {
                merge(idx - 1);
            }

            return;
        }
    }

    template <typename T, int b>
    void BTreeNode<T, b>::borrowFromPrev(int idx)
    {
        BTreeNode<T, b> *child = C[idx];
        BTreeNode<T, b> *sibling = C[idx - 1];

        for (int i = child->sz - 1; i >= 0; --i)
        {
            child->keys[i + 1] = child->keys[i];
        }

        if (!child->leaf)
        {
            for (int i = child->sz; i >= 0; --i)
            {
                child->C[i + 1] = child->C[i];
            }
        }

        child->keys[0] = keys[idx - 1];

        if (!child->leaf)
        {
            child->C[0] = sibling->C[sibling->sz];
        }

        keys[idx - 1] = sibling->keys[sibling->sz - 1];

        child->sz++;
        sibling->sz--;

        return;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::borrowFromNext(int idx)
    {
        BTreeNode<T, b> *child = C[idx];
        BTreeNode<T, b> *sibling = C[idx + 1];

        child->keys[child->sz] = keys[idx];
        if (!child->leaf)
        {
            child->C[child->sz + 1] = sibling->C[0];
        }

        keys[idx] = sibling->keys[0];

        for (int i = 1; i < sibling->sz; ++i)
        {
            sibling->keys[i - 1] = sibling->keys[i];
        }

        if (!sibling->leaf)
        {
            for (int i = 1; i <= sibling->sz; ++i)
            {
                sibling->C[i - 1] = sibling->C[i];
            }
        }

        child->sz++;
        sibling->sz--;

        return;
    }

    template <typename T, int b>
    void BTreeNode<T, b>::merge(int idx)
    {
        BTreeNode<T, b> *child = C[idx];
        BTreeNode<T, b> *sibling = C[idx + 1];

        child->keys[b - 1] = keys[idx];

        for (int i = 0; i < sibling->sz; ++i)
        {
            child->keys[i + b] = sibling->keys[i];
        }

        if (!child->leaf)
        {
            for (int i = 0; i <= sibling->sz; ++i)
            {
                child->C[i + b] = sibling->C[i];
            }
        }

        for (int i = idx + 1; i < sz; ++i)
        {
            keys[i - 1] = keys[i];
        }

        for (int i = idx + 2; i <= sz; ++i)
        {
            C[i - 1] = C[i];
        }

        child->sz += sibling->sz + 1;
        sz--;

        delete (sibling);
        return;
    }
}