#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>   // std::random_access_iterator_tag
#include <cstddef>     // size_t
#include <stdexcept>   // std::out_of_range
#include <type_traits> // std::is_same

template <class T>
class Vector
{
public:
    class iterator;

private:
    T *array;
    size_t _capacity, _size; // size store actual object, capacity is size + empty space

    // You may want to write a function that grows the vector
    void grow()
    {
        std::cout << "Resize called" << std::endl;

        // initialize array with 1 element so that any resize afterward can be double
        if (_capacity == 0)
        {
            std::cout << "First initialization" << std::endl;

            T *largerArray = new T[1];
            _capacity++;

            delete[] array;
            array = largerArray;
        }

        // otherwise, double capacity and create new array with updated capacity to resize
        else
        {
            _capacity = _capacity * 2;
            std::cout << "New capacity: " << _capacity << std::endl;
            T *largerArray = new T[_capacity];
            std::cout << "Size: " << _size << std::endl;

            // for loop to iterate through new array and copy over
            for (size_t i = 0; i < _size; i++)
            {
                largerArray[i] = std::move(array[i]);
                //std::cout << "Position " << i << " Copied" << std::endl;
            }
            delete[] array;
            array = largerArray;
        }
    }

public:
    Vector() noexcept
    {

        std::cout << "Default constructor called" << std::endl;

        // default 0 to do arithmatic and know that vector had no space
        _size = 0;
        _capacity = 0;
        array = nullptr;
    }

    Vector(size_t count, const T &value)
    {
        std::cout << "Parameterized constructor (2 args) called" << std::endl;

        // Make a vector full of said value
        _size = count;
        array = new T[count];
        _capacity = count;

        for (size_t i = 0; i < _size; i++)
        {
            array[i] = value;
        }
    }

    explicit Vector(size_t count)
    {
        std::cout << "Parameterized constructor (1 arg) called" << std::endl;
        _capacity = count;
        _size = count;

        //fill empty vector with "empty" default type
        array = new T[count]{};
    }

    Vector(const Vector &other)
    {
        std::cout << "Copy constructor called" << std::endl;

        _size = other._size;
        _capacity = other._capacity;
        array = new T[other._capacity];

        //don't need to delete other array since we are not "moving" it
        for (size_t i = 0; i < other._size; i++)
        {
            array[i] = other.array[i];
        }
    }

    Vector(Vector &&other) noexcept
    {
        std::cout << "Move constructor called" << std::endl;

        _size = other.size();
        _capacity = other.capacity();

        //move and deallocate other array
        array = std::move(other.array);
        other.array = nullptr;
        other._capacity = 0;
        other._size = 0;
    }

    ~Vector()
    {
        std::cout << "Destructor called" << std::endl;
        delete[] array;
        array = nullptr;
    }

    Vector &operator=(const Vector &other)
    {
        std::cout << "= operator called" << std::endl;

        //prevent self-copy
        if (this != &other)
        {
            _size = other._size;
            _capacity = other._capacity;

            // Copy everything over new array and just move pointer
            T *newArray = new T[other._capacity];

            for (size_t i = 0; i < other._size; i++)
            {
                newArray[i] = other.array[i];
            }

            delete[] array;
            array = newArray;
            return *this;
        }
    }

    Vector &operator=(Vector &&other) noexcept
    {
        std::cout << "=&& operator called" << std::endl;

        // just MOVE pointer and other attribute, prevent self-assignment error
        if (array != other.array)
        {
            _size = other.size();
            _capacity = other.capacity();

            delete[] array;
            array = other.array;
            other.array = nullptr;
        }
        return *this;
    }

    iterator begin() noexcept
    {
        return iterator(&array[0]);
    }

    iterator end() noexcept
    {
        return iterator(&array[_size]);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        if (_size == 0)
        {
            return true;
        }
        return false;
    }

    size_t size() const noexcept
    {
        return _size;
    }

    size_t capacity() const noexcept
    {
        return _capacity;
    }

    T &at(size_t pos)
    {
        if (pos >= _size)
        {
            throw std::out_of_range("Out of bound");
        }
        return array[pos];
    }

    const T &at(size_t pos) const
    {
        if (pos >= _size)
        {
            throw std::out_of_range("Out of bound");
        }
        return array[pos];
    }

    T &operator[](size_t pos)
    {
        return array[pos];
    }
    const T &operator[](size_t pos) const
    {
        return array[pos];
    }

    T &front()
    {
        return array[0];
    }

    const T &front() const
    {
        return array[0];
    }

    T &back()
    {
        return array[_size - 1];
    }

    const T &back() const
    {
        return array[_size - 1];
    }

    void push_back(const T &value)
    {
        std::cout << "Push back called" << std::endl;

        if (_size >= _capacity)
        {
            grow();
        }

        array[_size] = value;
        _size++;
    }

    void push_back(T &&value)
    {
        std::cout << "Push back && called " << std::endl;

        if (_size >= _capacity)
        {
            grow();
        }

        array[_size] = std::move(value);
        _size++;
        //std::cout << _size << " vs " << _capacity << std::endl;
    }

    void pop_back()
    {
        _size--;
    }

    iterator insert(iterator pos, const T &value)
    {
        std::cout << "Insert copy called" << std::endl;
        size_t position = pos - begin();
        
        if (_size == _capacity)
        {
            grow();
        }
        _size++;

        //use move to save memory allocation
        for (size_t i = _size; i > position; i--)
        {
            array[i] = std::move(array[i - 1]);
        }

        array[position] = value;

        return &array[position];
    }

    //same as insert copy, but use move on value insead of copying
    iterator insert(iterator pos, T &&value)
    {
        std::cout << "Insert move called" << std::endl;
        size_t position = pos - begin();

        if (_size == _capacity)
        {
            grow();
        }
        
        for (size_t i = _size; i > position; i--)
        {
            array[i] = std::move(array[i - 1]);
        }

        array[position] = std::move(value);
        _size++;

        return iterator(array + position);
    }

    //REMEMBER HOW MUCH TIME YOU WASTED BY NOT USING PTRDIFF_T!!! ALWAYS USE PTRdIFF_T FROM NOW ON!!!!
    iterator insert(iterator pos, size_t count, const T &value)
    {
        ptrdiff_t position = pos - begin();
        
        while (_capacity - _size < count)
        {
            grow();
        }

        _size+=count;

        //DUDE SAVE 3 HRS BY USING PTRDIFF PLEASE SEGMENT FAULT WILL HAPPEN IF NOT
        for (ptrdiff_t i = _size - count - 1; i >= position; i--)
        {
            array[i+count] = array[i];
        }

        for (size_t i = 0; i < count; i++)
        {
            array[position+i] = value;
        }

        return &array[position];
    }

    iterator erase(iterator pos)
    {
        std::cout << "Erase called" << std::endl;

        /*using size_t result in segmentation fault due to modifying with move, 
        so use iterator instead..

        OR JUST USE PTRDIFF BRO IT SAVES LIVES!!! NEVER USE SIZE_T AGAIN IN MY LIFE
        */
        for (iterator i = pos; i < end()-1; i++)
        {
            if (i >= pos && i < end())
            {
                *i = std::move(*(i+1));
            }
        }
        
        _size--;
        return pos;
    }

    //just moving more element than the other one
    iterator erase(iterator first, iterator last)
    {
        std::cout << "Erase line called" << std::endl;
        int diff = last - first;
        for (iterator i = first; i < end()-diff; i++)
        {
            *i = std::move(*(i + diff));
        }

        _size -= diff;
        return first;
    }

    class iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

    private:
        // Points to some element in the vector (or nullptr)
        T *_ptr;

    public:
        iterator() { _ptr = nullptr; }
        iterator(T *ptr) { _ptr = ptr; }

        // This assignment operator is done for you, please do not add more
        iterator &operator=(const iterator &) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept
        {
            return *_ptr;
        }

        [[nodiscard]] pointer operator->() const noexcept
        {
            return _ptr;
        }

        // Prefix Increment: ++a
        iterator &operator++() noexcept
        {
            this->_ptr++;
            return *this;
        }

        // Postfix Increment: a++****
        iterator operator++(int) noexcept
        {
            iterator something = *this;
            ++(_ptr);
            return something;
        }

        // Prefix Decrement: --a
        iterator &operator--() noexcept
        {
            this->_ptr--;
            return *this;
        }

        // Postfix Decrement: a--****
        iterator operator--(int) noexcept
        {
            iterator something = *this;
            --(_ptr);
            return something;
        }

        iterator &operator+=(difference_type offset) noexcept
        {
            _ptr += offset;
            return *this;
        }

        [[nodiscard]] iterator operator+(difference_type offset) const noexcept
        {
            iterator temp(_ptr + offset);
            return temp;
        }

        iterator &operator-=(difference_type offset) noexcept
        {
            _ptr -= offset;
            return *this;
        }

        [[nodiscard]] iterator operator-(difference_type offset) const noexcept
        {
            iterator temp(_ptr - offset);
            return temp;
        }

        [[nodiscard]] difference_type operator-(const iterator &rhs) const noexcept
        {
            return _ptr - rhs._ptr;
        }

        [[nodiscard]] reference operator[](difference_type offset) const noexcept
        {
            return *(_ptr + offset);
        }

        [[nodiscard]] bool operator==(const iterator &rhs) const noexcept
        {
            return _ptr == rhs._ptr;
        }

        [[nodiscard]] bool operator!=(const iterator &rhs) const noexcept
        {
            return !(_ptr == rhs._ptr);
        }

        [[nodiscard]] bool operator<(const iterator &rhs) const noexcept
        {
            return _ptr < rhs._ptr;
        }

        [[nodiscard]] bool operator>(const iterator &rhs) const noexcept
        {
            return _ptr > rhs._ptr;
        }

        [[nodiscard]] bool operator<=(const iterator &rhs) const noexcept
        {
            return _ptr <= rhs._ptr;
        }

        [[nodiscard]] bool operator>=(const iterator &rhs) const noexcept
        {
            return _ptr >= rhs._ptr;
        }
    };

    void clear() noexcept
    {
        _size = 0;
    }
};

// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace
{
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const &iterator) noexcept
{
    _Iterator copy = iterator + offset;
    return copy;
}

///////////////////////////////////////////////////////////////////////////////////////

void swap(T & a, T & b) noexcept {
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void bubble(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) {
		// Random access iterators have the same traits you defined in the Vector class
		// For instance, difference_type represents an iterator difference
		// You may delete the types you don't use to remove the compiler warnings
		using _it             = std::iterator_traits<RandomIter>;
		using difference_type = typename _it::difference_type;
		using value_type      = typename _it::value_type;
		using reference       = typename _it::reference;
		using pointer         = typename _it::pointer;

		// Starts at end and go to beginning and check/swap backward -- does not segment fault 
		for (RandomIter j = end; j > begin; j--)
		{
			for (RandomIter i = begin+1; i < j; i++)
			{
				if (!comp(*(i-1),*i))
				{
					swap(*(i-1),*i);
				}
			}
		}
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void insertion(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) 
	{
		RandomIter i = begin+1;
		while (i < end)
		{
			if (!comp(*(i-1),*i))
			{
				// Swap now to decrease comparison count
				RandomIter j = i;
				swap(*(j-1),*j);
				j--;

				// Unknown how out of place element is, use while
				while (j > begin)
				{
					if (comp(*(j-1),*j))
					{
						break;
					}
					swap(*(j-1),*j);
					j--;
				}
			}
			i++;
		}
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void selection(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) 
	{
		RandomIter smaller;
		// Outer loop "restrict" inner loop to prevent sorted element from getting sorted again/compared to
		for (RandomIter j = begin; j < end; j++)
		{
			smaller = j;
			for (RandomIter i = j+1; i < end; i++)
			{
				if (comp(*i,*smaller))
				{
					swap(*smaller, *i);
				}
			}
		}
	}

#endif