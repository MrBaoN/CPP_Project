#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"



template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    size_type _bucket_count;    // Array size, number of linked list total
    HashNode **_buckets;    // Actual array containing all the "Linked list"

    HashNode * _head;   // Points to first NODE, usually in the array index 0
    size_type _size;    // Number of NODE total

    Hash _hash;     // Function that "convert" key into number called hash_code
    key_equal _equal;   //Function that check equivalence

    // This function find the index where the hash_code should be
    static size_type _range_hash(size_type hash_code, size_type bucket_count) 
    {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;  // Points to the Unordered map itself
        HashNode * _ptr;    // Points to current node

        explicit basic_iterator(UnorderedMap const * map, HashNode *ptr) noexcept {_map = map; _ptr = ptr;}

    public:
        basic_iterator() {_map = nullptr; _ptr = nullptr;};

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;

        // "Dereferencing" or get content out, in this case value val is the content
        reference operator*() const {return _ptr -> val;}

        //Getting access to val to access its private member function
        pointer operator->() const {return &(_ptr -> val);}     
        
        // If next is null then find the next non-null node to return, via traversing the array
        basic_iterator &operator++() 
        {
            if (_ptr -> next)
            {_ptr = _ptr -> next;}

            else
            {
                size_type index = _map->_bucket(_ptr->val)+1;

                while(!_map -> _buckets[index] && index < _map->_bucket_count)
                    {index++;}

                // Set to null in case there are "junk" in the memory right after array
                if(index == _map->_bucket_count)
                    {_ptr = nullptr;}

                else
                    {_ptr = _map -> _buckets[index];}
            }
            return *this;
        }

        // just call the other operator, but keep copy of old position to return
        basic_iterator operator++(int) 
        {
            basic_iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const basic_iterator &other) const noexcept {return _ptr == other._ptr;}
        bool operator!=(const basic_iterator &other) const noexcept {return _ptr != other._ptr;}
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;

            explicit local_iterator( HashNode * node ) noexcept {_node = node;}

        public:
            local_iterator() {_node = nullptr;}

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            
            // "Dereferencing" or get val out again
            reference operator*() const 
            {return _node->val;}

            // Return access to val to access private function and member
            pointer operator->() const 
            {return &(_node->val);}

            // Does not care if next is null or not, as local iterator is only traversing in ONE linked list
            local_iterator & operator++() 
            {
                _node = _node -> next; 
                return *this;
            }

            // Same as before
            local_iterator operator++(int) 
            {
                local_iterator temp = *this;
                ++(*this);
                return temp;
            }

            bool operator==(const local_iterator &other) const noexcept {return _node == other._node;}
            bool operator!=(const local_iterator &other) const noexcept {return _node != other._node;}
    };

private:

    size_type _bucket(size_t code) const {return _range_hash(code, _bucket_count);}
    size_type _bucket(const Key & key) const {return _bucket(_hash(key));}
    size_type _bucket(const value_type & val) const {return _bucket(_hash(val.first));}

    // Member function of Unordered map, therefore have access to _buckets. Not sure why code is in the argument when index bucket is given
    HashNode*& _find(size_type code, size_type bucket, const Key & key) 
    {
        HashNode ** node = &_buckets[bucket];

        while (*node && !_equal((*node) -> val.first,key))
        {
            node = &((*node) -> next);
        }

        return *node;
    }
    
    // call first find 
    HashNode*& _find(const Key & key) 
    {return _find(_hash(key), _bucket(key), key);}

    // Add new node with move semantic, take care of cases where insert at _head or head is uninitialized
    HashNode * _insert_into_bucket(size_type bucket, value_type && value) 
    {
        HashNode * temp = new HashNode(std::move(value));

        temp -> next = _buckets[_bucket(value)];
        _buckets[_bucket(value)] = temp;

        if (_head == nullptr || _bucket(value.first) <= _bucket(_head -> val.first))
        {
            _head = temp;
        }
        _size++;
        return temp;
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) {
        
        // Transfer everything to dst
        dst._bucket_count = src._bucket_count;
        dst._buckets = src._buckets;
        dst._head = src._head;
        dst._size = src._size;
        dst._hash = std::move(src._hash);
        dst._equal = std::move(src._equal);

        // Clear src data
        src._buckets = new HashNode*[src._bucket_count]();
        src._head = nullptr;
        src._size = 0;
    }

public:

    // Default Constructor - creates prime number of empty linkedlist array
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { }, const key_equal & equal = key_equal { }) 
    {
        _bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode * [_bucket_count]();
        _size = 0;
        _head = nullptr;
        _hash = hash;
        _equal = equal;
    }

    ~UnorderedMap() 
    {
        clear();
        delete[] _buckets;
        _buckets = nullptr;
        _bucket_count = 0;
        _size = 0;
    }

    // Copy constructor - copy everything, and traverse array, 
    // inserting elements into linkedlist without caring about order
    UnorderedMap(const UnorderedMap & other) 
    {
        _hash = other._hash;
        _equal = other._equal;
        _size = 0;
        _head = nullptr;
        _bucket_count = other._bucket_count;
        _buckets = new HashNode * [_bucket_count]();
        
        for (size_type i = 0; i < _bucket_count; i++){
            HashNode * traverse = other._buckets[i];

            while (traverse){
                insert(traverse -> val);
                traverse = traverse ->next;
            }
        }
    }

    UnorderedMap(UnorderedMap && other) 
    {_move_content(other, *this);}

    // Operator copy - For some reason must do it manually, can't clear() and call copy constructor
    UnorderedMap & operator=(const UnorderedMap & other) 
    {
        if (other._buckets == _buckets)
        {return *this;}

        clear();
        delete [] _buckets;

        _hash = other._hash;
        _equal = other._equal;
        _size = 0;
        _head = nullptr;
        _bucket_count = other._bucket_count;
        _buckets = new HashNode * [_bucket_count]();
        
        for (size_type i = 0; i < _bucket_count; i++){
            HashNode * traverse = other._buckets[i];

            while (traverse){
                insert(traverse -> val);
                traverse = traverse ->next;
            }
        }
    }

    // Operator Move - call destructor and _move_content
    UnorderedMap & operator=(UnorderedMap && other) 
    {
        if (other._buckets == _buckets)
            {return *this;}
        this -> ~UnorderedMap();
        _move_content(other, *this);
        
    }

    // Check each linked list for nonempty then traverse and delete
    void clear() noexcept 
    {
        if (_size <= 0)
            {return;}

        iterator traverse = iterator(this,_head);
        
        while(traverse != this -> end())
        {
            basic_iterator temp = traverse;
            traverse++;
            delete temp._ptr;
            _size--;
        }
    }

    size_type size() const noexcept {return _size;}

    bool empty() const noexcept {return _size == 0;}

    size_type bucket_count() const noexcept {return _bucket_count;}

    iterator begin() {return iterator(this,_head);}
    iterator end() {return iterator(this,nullptr);}

    const_iterator cbegin() const {return iterator(this,_head);}
    const_iterator cend() const {return iterator(this,nullptr);}

    local_iterator begin(size_type n) {return local_iterator(_buckets[n]);}
    local_iterator end(size_type n) {return local_iterator(nullptr);}

    size_type bucket_size(size_type n) 
    {
        HashNode * traverse = _buckets[n];
        size_type count = 0;

        while(traverse)
        {
            count++;
            traverse = traverse -> next;
        }
        return count;
    }

    float load_factor() const {return float(_size) / _bucket_count;}

    size_type bucket(const Key & key) const {return _bucket(key);}

    // return pair with iterator and true or false if inserted or not
    std::pair<iterator, bool> insert(value_type && value) 
    {
        HashNode * exist = _find(value.first);

        if(exist)
        {
            return {iterator(this,exist),false};
        }
        
        return {iterator(this,_insert_into_bucket(_bucket(value), std::move(value))),true};
    }

    // Same but with copy sematic so have to do manually since helper insert is move semantic only
    std::pair<iterator, bool> insert(const value_type & value) 
    {
        HashNode * exist = _find(value.first);
        size_type index = _bucket(value);
        
        if (exist)
        {
            return {iterator(this,exist),false};
        }
        
        exist = new HashNode(value);
        exist -> next = _buckets[index];
        _buckets[index] = exist;
        _size++;

        if(!_head || _bucket(value) <= _bucket(_head -> val))
        {
            _head = exist;
        }

        return {iterator(this,exist),true};
    }

    iterator find(const Key & key) 
        {return iterator(this,_find(key));}

    // T() or T{} to get element on the right of assignment operator in : int x = something
    T& operator[](const Key & key) 
    {
        HashNode * node = _find(key);

        if (node)
            {return node->val.second;}

        node = new HashNode(std::make_pair(key,T{}));
        node -> next = _buckets[_bucket(key)];
        _buckets[_bucket(key)] = node;

        if (!_head || _bucket(key) <= _bucket(_head -> val.first))
            {_head = node;}

        _size++;

        return node->val.second;
    }

    iterator erase(iterator pos) 
    {
        if (pos == end()){
            return end();
        }

        HashNode * to_be_erase = pos._ptr;
        iterator it = iterator(this, to_be_erase); // Get iterator next ahead of time to avoid conflict
        it++;

        // Case one where the element to be erase is the head of the linked list, also check if its head
        if (_buckets[_bucket(to_be_erase -> val)] == to_be_erase)
        {

            _buckets[_bucket(to_be_erase -> val)] = to_be_erase -> next;

            if(_head == to_be_erase){
                _head = it._ptr;
            }

            delete to_be_erase;
            _size--;
            return it;
        }

        // Standard case, no need to worry about head or nullptr next;
        HashNode * erase_prev = _buckets[_bucket(pos._ptr -> val)];

        while(erase_prev -> next != to_be_erase)
        {
            erase_prev = erase_prev -> next;
        }
        
        erase_prev -> next = to_be_erase -> next;
        delete to_be_erase;
        _size--;

        return it;
    }

    size_type erase(const Key & key) 
    {
        HashNode * node = _find(key);

        if (!node)
            {return 0;}

        node = _buckets[_bucket(key)];
        
        // Case 1: eraseing node at beginning of linked list
        if (_equal(node->val.first,key))
        {
            iterator it = iterator(this,_buckets[_bucket(key)]);
            _buckets[_bucket(key)]=node->next;

            if(_head == node)
            {
                it++;

                _head = it._ptr;
            }

            delete node;
            _size--;
            return 1;
        }
        
        // Standard case
        while (!_equal((node->next)->val.first, key))
            {node = node -> next;}

        HashNode * erase_this = node->next;
        node -> next = erase_this -> next;

        delete erase_this;
        _size--;
        return 1;
        
    }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
