#pragma once

#include <cstddef> // size_t
#include <iterator> // std::bidirectional_iterator_tag
#include <type_traits> // std::is_same, std::enable_if

template <class T>
class List {
    private:
    struct Node {
        Node *next, *prev;
        T data;
        explicit Node(Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev} {}
        explicit Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{data} {}
        explicit Node(T&& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{std::move(data)} {}
    };

    template <typename pointer_type, typename reference_type>
    class basic_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = pointer_type;
        using reference         = reference_type;
    private:
        friend class List<value_type>;
        using Node = typename List<value_type>::Node;

        Node* node;

        explicit basic_iterator(Node* ptr) noexcept : node{ptr} {}
        explicit basic_iterator(const Node* ptr) noexcept : node{const_cast<Node*>(ptr)} {}

    public:
        basic_iterator() {node = nullptr;};
        basic_iterator(const basic_iterator&) = default;
        basic_iterator(basic_iterator&&) = default;
        ~basic_iterator() = default;
        basic_iterator& operator=(const basic_iterator&) = default;
        basic_iterator& operator=(basic_iterator&&) = default;

        reference operator*() const {
            return node -> data;
        }
        pointer operator->() const {
            return &(node -> data);
        }

        // Prefix Increment: ++a
        basic_iterator& operator++() {
            node = node -> next;
            return *this;
        }
        // Postfix Increment: a++
        basic_iterator operator++(int) {
            node = node -> next;
            
            return basic_iterator(node -> prev);
        }
        // Prefix Decrement: --a
        basic_iterator& operator--() {
            node = node -> prev;
            return *this;
        }
        // Postfix Decrement: a--
        basic_iterator operator--(int) {
            node = node -> prev;
            return basic_iterator(node -> next);
        }

        bool operator==(const basic_iterator& other) const noexcept {
            return node == other.node;
        }
        bool operator!=(const basic_iterator& other) const noexcept {
            return node != other.node;
        }
    };

public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = basic_iterator<pointer, reference>;
    using const_iterator  = basic_iterator<const_pointer, const_reference>;

private:
    Node head, tail;
    size_type _size;

public:
    List() {
        std::cout << "Default Constructor called"<< std::endl;
        
        head.next = &tail;
        tail.prev = &head;
        _size = 0;
    }

    List(size_type count, const T& value ) {
        std::cout << "Insert Constructor with Value called" << std::endl;
        _size = 0;
        Node * traverse = &head;
        for (int i = 0; i< count; i++){
            Node * temp = new Node(value);
            temp -> prev = traverse;
            traverse -> next = temp;
            traverse = traverse -> next;
            _size++;
        }
        traverse -> next = &tail;
        tail.prev = traverse;

    }

    explicit List( size_type count ) {
        std::cout << "Default Insert Constructor called" <<std::endl;
        _size = 0;
        Node* traverse = &head;
        for (size_type i = 0; i < count; i++){
            
            Node* temp = new Node(T{},nullptr,nullptr);
            

            temp -> prev = traverse;
            traverse -> next = temp;
            traverse = traverse -> next;
            _size++;
        }
        traverse -> next = &tail;
        tail.prev = traverse;
    }

    List( const List& other ) {
        std::cout << "Copy constructor called" << std::endl;

        
        Node* traverseThis = &head;
        basic_iterator it = other.begin();
        _size=0;
    
        while (it != other.end()){
            
            Node * temp = new Node (*it,nullptr,nullptr);
            
            temp -> prev = traverseThis;
            
            traverseThis -> next = temp;
            
            traverseThis = traverseThis -> next;

            it++;
            _size++;
        }
        traverseThis -> next = &tail;
        tail.prev = traverseThis;

    }

    List( List&& other ){
        std::cout << "Move Constructor called" << std::endl;

        head.next = other.head.next;
        head.next->prev = &head;

        tail.prev=other.tail.prev;
        tail.prev->next = &tail;
        
        other.head.next=&other.tail;
        
        other.tail.prev=&other.head;
        
        _size = other._size;
        other._size=0;
    }

    ~List() {
        clear();
    }
    List& operator=( const List& other ) {
        std::cout << "Copy operator called" << std::endl;

        if (this->begin() != other.begin()){

            
            clear();
    
            basic_iterator it = basic_iterator(other.begin());
            
            while (it != other.end()){
                Node * temp = new Node (*it,tail.prev,&tail);

                temp -> prev -> next = temp;
                tail.prev = temp;

                it++;
                _size++;
        
            }
        }
        
        return *this;
    }
    List& operator=( List&& other ) noexcept {
        std::cout << "Operator Move called" << std::endl;

        if (this->begin() == other.begin()){
            return *this;
        }
        
        clear();
        head.next = other.head.next;
        head.next->prev = &head;

        tail.prev=other.tail.prev;
        tail.prev->next = &tail;
        
        other.head.next=&other.tail;
        
        other.tail.prev=&other.head;
        
        _size = other._size;
        other._size=0;
        return *this;
    }

    reference front() {
        return head.next -> data;
    }
    const_reference front() const {
        return head.next -> data;
    }
	
    reference back() {
        return tail.prev -> data;
    }
    const_reference back() const {
        return tail.prev -> data;
    }
	
    iterator begin() noexcept {
        Node* temp = head.next;
        return iterator(temp);
    }
    const_iterator begin() const noexcept {
        return const_iterator(head.next);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(head.next);
    }

    iterator end() noexcept {
        return iterator(&tail);
    }
    const_iterator end() const noexcept {
        return const_iterator(&tail);
    }
    const_iterator cend() const noexcept {
        return const_iterator(&tail);
    }

    bool empty() const noexcept {
        return begin() == end();
    }

    size_type size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        Node * traverse = &head;
        Node * ahead = head.next;
        while(traverse -> next != &tail){
            traverse = traverse -> next;
            delete traverse;
            traverse = ahead;
            ahead = ahead -> next;
            _size--;
        }
        head.next = &tail;
        tail.prev = &head;
    }

    iterator insert( const_iterator pos, const T& value ) {

        Node *something = new Node(value,pos.node->prev, pos.node);
        something -> next -> prev = something;
        something -> prev -> next = something;
        _size++;
        
        return iterator(something);
         
    }
    iterator insert( const_iterator pos, T&& value ) {
        Node *something = new Node(std::move(value),pos.node->prev, pos.node);
        something -> next -> prev = something;
        something -> prev -> next = something;
        _size++;
        
        return iterator(something);
    }

    iterator erase( const_iterator pos ) {
        pos.node -> prev -> next = pos.node -> next;
        Node * returnThis = pos.node -> next;
        pos.node -> next -> prev = pos.node -> prev;
        delete pos.node;
        _size--;

        return iterator(returnThis);
    }

    void push_back( const T& value ) {
        std::cout << "Regular Pushback called" << std::endl;
        Node * endd = new Node(value, tail.prev, &tail);

        tail.prev -> next = endd;
        tail.prev = endd;
        _size++;
    }

    void push_back( T&& value ) {
        std::cout << "Move Pushback called" << std::endl;
        Node * endd = new Node(std::move(value), tail.prev, &tail);

        tail.prev -> next = endd;
        tail.prev = endd;
        _size++;
    }

    void pop_back() {
        std::cout << "Popback called" << std::endl;

        Node * temp = tail.prev -> prev;
        temp -> next = &tail;
        delete tail.prev;
        tail.prev = temp;
        _size--;
        
    }
	
    void push_front( const T& value ) {
        Node * newFirst = new Node(value,&head,head.next);
        head.next = newFirst;
        newFirst -> next -> prev = newFirst;
        _size++;
    }
	void push_front( T&& value ) {
        Node * newFirst = new Node(std::move(value),&head,head.next);
        head.next = newFirst;
        newFirst -> next -> prev = newFirst;
        _size++;
    }

    void pop_front() {
        Node * temp = head.next;
        head.next = temp -> next;
        temp -> next -> prev = &head;
        delete temp;

        _size--;
    }

    /*
      You do not need to modify these methods!
      
      These method provide the non-const complement 
      for the const_iterator methods provided above.
    */
    iterator insert( iterator pos, const T & value) { 
        return insert((const_iterator &) (pos), value);
    }

    iterator insert( iterator pos, T && value ) {
        return insert((const_iterator &) (pos), std::move(value));
    }

    iterator erase( iterator pos ) {
        return erase((const_iterator&)(pos));
    }
};


/*
    You do not need to modify these methods!

    These method provide a overload to compare const and 
    non-const iterators safely.
*/
 
namespace {
    template<typename Iter, typename ConstIter, typename T>
    using enable_for_list_iters = typename std::enable_if<
        std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::iterator, 
            Iter
        >{} && std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::const_iterator,
            ConstIter
        >{}, T>::type;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) == rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) == lhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) != rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) != lhs;
}