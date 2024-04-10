#pragma once

#include <functional> // std::less
#include <iostream>
#include <queue> // std::queue
#include <utility> // std::pair

template <typename K, typename V, typename Comparator = std::less<K>>
class BinarySearchTree
{
  public:
    using key_type        = K;
    using value_type      = V;
    using key_compare     = Comparator;
    using pair            = std::pair<key_type, value_type>;
    using pointer         = pair*;
    using const_pointer   = const pair*;
    using reference       = pair&;
    using const_reference = const pair&;
    using difference_type = ptrdiff_t;
    using size_type       = size_t;

  private:
    struct BinaryNode
    {
        pair element;
        BinaryNode *left;
        BinaryNode *right;

        BinaryNode( const_reference theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ theElement }, left{ lt }, right{ rt } { }
        
        BinaryNode( pair && theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ std::move( theElement ) }, left{ lt }, right{ rt } { }
    };

    using node           = BinaryNode;
    using node_ptr       = node*;
    using const_node_ptr = const node*;

    node_ptr _root;
    size_type _size;
    key_compare comp;

  public:
    BinarySearchTree() {
        // Default Constructor
        _root = nullptr;
        _size = 0;

    }
    BinarySearchTree( const BinarySearchTree & rhs ) {
        // Copy Constructor
        // just call clone cuz why work harder than you have to

        _root = clone(rhs._root);
        
        _size = rhs._size;
            
    }
    BinarySearchTree( BinarySearchTree && rhs ) {
        // Move Constructor
        // Just do pointer manipulation

            _root = (rhs._root);
            _size = (rhs._size);

            rhs._root = nullptr;
            rhs._size = 0;
            
    }
    ~BinarySearchTree() {
        // Destructor
        // Call clear() since it will take care of everything

        clear();
    }

    const_reference min() const { return min( _root )->element; }
    const_reference max() const { return max( _root )->element; }
    const_reference root() const {
        return const_reference(*_root);
    }
    bool contains( const key_type & x ) const { return contains( x, _root ); }
    value_type & find( const key_type & key ) { return find( key, _root )->element.second; }
    const value_type & find( const key_type & key ) const { return find( key, _root )->element.second; }
    bool empty() const {
        return size() == 0;
    }
    size_type size() const {
        return _size;
    }

    void clear() {
        // Call clear on root since already implemented node clear below
        // delete this and its child node

        clear( _root );
        _size = 0;
    }
    void insert( const_reference x ) { insert( x, _root ); }
    void insert( pair && x ) { insert( std::move( x ), _root ); }
    void erase( const key_type & x ) { erase(x, _root); }

    BinarySearchTree & operator=( const BinarySearchTree & rhs ) {
        // Operator Copy
        // check self-assignment
        if(rhs._root != _root){
            clear();
            _root = clone(rhs._root);
            _size = (rhs._size);
        }
        return *this;
    }
    BinarySearchTree & operator=( BinarySearchTree && rhs ) {
        // Operator Move
        // Pointer manipulation
        if(&rhs == this){
            return *this;
        }
        clear();
        _root = rhs._root;
        _size = rhs._size;
        rhs._root=nullptr;
        rhs._size = 0;
        return *this;
    }

  private:
    void insert( const_reference x, node_ptr & t ) {
        // Insert copy
        // find where it should be inserting
        // already there: replace
        // not there: add new node
        // NEVER WILL BE IN-BETWEEN NODE
        // const_reference to compare same type

        if(t == nullptr){
            t = new BinaryNode(x,nullptr, nullptr);
            _size++;
        }

        else if(const_reference(t->element).first == x.first){
            t->element = x;
        }

        else if(comp(x.first, const_reference(t->element).first)){
            insert(x, t->left);
        }

        else if(comp(const_reference(t->element).first, x.first)){
            insert(x, t->right);
        }
        
    }
    void insert( pair && x, node_ptr & t ) {
        // Insert Move
        // No need for const_reference since already same type
        // Call std::move inside constructor for move constructor

        if(t == nullptr){
            t = new BinaryNode(std::move(x),nullptr, nullptr);
            _size++;
        }

        else if(t->element.first == x.first){
            t->element = std::move(x);
        }

        else if(comp(x.first, const_reference(t->element).first)){
            insert(std::move(x), t->left);
        }

        else if(comp(const_reference(t->element).first, x.first)){
            insert(std::move(x), t->right);
        }
        
    }

    void erase( const key_type & x, node_ptr & t ) {
        // try to find key first half, when find, 3 cases: no child, one child, two child
        // node t = node t -> next to reassign pointers to next and cut t from binary tree
        // then delete t after by creating temp node_ptr = t and delete temp
        // temp = nullptr avoid double free

        if (t == nullptr)
        {
            return;
        }

        if(comp(x, const_reference(t->element).first))
        {
            erase(x,t->left);
        }

        else if (comp(const_reference(t->element).first, x))
        {
            erase(x,t->right);
        }

        else if (x == const_reference(t->element).first)
        {
            if (t -> left == nullptr && t -> right == nullptr)
            {
                delete t;
                t = nullptr;
                _size--;
            }

            else if (t -> left == nullptr)
            {
                node_ptr temp = t;
                t = t->right;
                delete temp;
                temp = nullptr;
                _size--;
            }

            else if (t -> right == nullptr)
            {
                node_ptr temp = t;
                t = t->left;
                delete temp;
                temp = nullptr;
                _size--;
            }
        
            else
            {
                t -> element = max(t->left) -> element;
                erase(t->element.first, t->left);
            }

        }
    }

    const_node_ptr min( const_node_ptr t ) const {
        // keep going left until can't, don't go right since only element bigger than node

        if(t->left){
            return min(t->left);
        }
    
        return t;
    }
    const_node_ptr max( const_node_ptr t ) const {
        // keep going right until can't, don't go left as only element lower than node

        if(t->right){
            return max(t->right);
        }
        
        return t;
    }

    bool contains( const key_type & x, const_node_ptr t ) const {
        // Coulda call find and check if nullptr or not but why not do it manually to understand?

        if(t==nullptr){
            return false;
        }

        else if(comp(x, const_reference(t->element).first)){
            return contains(x, t->left);
        }

        else if(comp(const_reference(t->element).first, x)){
            return contains(x, t->right);
        }

        return true;
    }
    node_ptr find( const key_type & key, node_ptr t ) {
        // Compare first because gradescope want to? also don't have to check ==

        if(t==nullptr){
            return nullptr;
        }

        else if(comp(key, const_reference(t->element).first)){
            return find(key, t->left);
        }

        else if(comp(const_reference(t->element).first, key)){
            return find(key, t->right);
        }

        return t;
    }
    const_node_ptr find( const key_type & key, const_node_ptr t ) const {
        // Same as other one???

        if(t==nullptr){
            return nullptr;
        }

        else if(comp(key, const_reference(t->element).first)){
            return find(key, t->left);
        }

        else if(comp(const_reference(t->element).first, key)){
            return find(key, t->right);
        }

        return t;
    }

    void clear( node_ptr & t ) {
        // Call recursively on left and right before delete to get all node, reassign to nullptr to avoid double free
        
        if(t == nullptr){
            return;
        }

        if(t -> left){
            clear(t->left);
        }

        if(t -> right){
            clear(t->right);
        }

        delete t;
        t=nullptr;
    }
    
    node_ptr clone ( const_node_ptr t ) const {
        // clone left and right inside constructor to save time assigning pointers
        if (t == nullptr){
            return nullptr;
        }
        node_ptr temp = new BinaryNode(t->element, clone(t->left), clone(t->right));
        return temp;
    }

  public:
    template <typename KK, typename VV, typename CC>
    friend void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend std::ostream& printNode(std::ostream& o, const typename BinarySearchTree<KK, VV, CC>::node& bn);

    template <typename KK, typename VV, typename CC>
    friend void printTree( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
        std::ostream & out,
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev
    );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        const BinarySearchTree<KK, VV, CC> & bst, 
        std::ostream & out
    );
};

template <typename KK, typename VV, typename CC>
std::ostream& printNode(std::ostream & o, const typename BinarySearchTree<KK, VV, CC>::node & bn) {
    return o << '(' << bn.element.first << ", " << bn.element.second << ')';
}

template <typename KK, typename VV, typename CC>
void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out = std::cout ) {
    using node = typename BinarySearchTree<KK, VV, CC>::node;
    using node_ptr = typename BinarySearchTree<KK, VV, CC>::node_ptr;
    using const_node_ptr = typename BinarySearchTree<KK, VV, CC>::const_node_ptr;
    
    // Push root on queue, then recursively push left, right
    // Change size at end of for loop to update parameter since for compare const parameter

    node_ptr temp;
    std::queue<node_ptr> q;
    std::stringstream str;
    int size;
    bool isNull;

    q.push(bst._root);
    size = q.size();

    while (true)
    {
        isNull = true;

        for (int i = 0; i < size; i++)
        {
            temp = q.front();
            q.pop();

            if (temp == nullptr)
            {
                str << "null";
                q.push(nullptr);
                q.push(nullptr);
            }
            else
            {
                str << "("<<temp->element.first<<","<<temp->element.second<<")";
                isNull = false;
                q.push(temp->left);
                q.push(temp->right);
            }

        }
        if (isNull == true)
        {
            return;
        }

        size = q.size();
        out << str.str() << '\n';
        str.str("");
    }
}

template <typename KK, typename VV, typename CC>
void printTree( const BinarySearchTree<KK, VV, CC> & bst, std::ostream & out = std::cout ) { printTree<KK, VV, CC>(bst._root, out ); }

template <typename KK, typename VV, typename CC>
void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth = 0 ) {
    if (t != nullptr) {
        printTree<KK, VV, CC>(t->right, out, depth + 1);
        for (unsigned i = 0; i < depth; ++i)
            out << '\t';
        printNode<KK, VV, CC>(out, *t) << '\n';
        printTree<KK, VV, CC>(t->left, out, depth + 1);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
    std::ostream & out,
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev = nullptr
) {
    if(node) {
        std::hash<KK> khash{};

        out << "\t" "node_" << (uint32_t) khash(node->element.first)
            << "[label=\"" << node->element.first 
            << " [" << node->element.second << "]\"];" << std::endl;
        
        if(prev)
            out << "\tnode_" << (uint32_t) khash(prev->element.first) <<  " -> ";
        else
            out << "\t";
        
        out << "node_" << (uint32_t) khash(node->element.first) << ";" << std::endl;
    
        vizTree<KK, VV, CC>(node->left, out, node);
        vizTree<KK, VV, CC>(node->right, out, node);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    const BinarySearchTree<KK, VV, CC> & bst, 
    std::ostream & out = std::cout
) {
    out << "digraph Tree {" << std::endl;
    vizTree<KK, VV, CC>(bst._root, out);
    out << "}" << std::endl;
}


