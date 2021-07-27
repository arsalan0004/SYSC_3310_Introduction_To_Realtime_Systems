# SYSC 2100 Summer 2021 Assignment 1

# ADT Deque (Implementation using a singly-linked list.)

from typing import Any


class LinkedDeque:
    """An implementation of a deque (double-ended queue).

    The methods for adding items to the left and right sides of a deque are O(1).
    The method for removing an item from the left side of a deque is O(1).
    The method for removing an item from the right side of a deque is O(n).
    """

    class Node:
        """A node in a singly-linked list"""

        def __init__(self, node_data: Any) -> None:
            """Initialize this node with payload node_data.
            The node's link is initialized with the end-of-list marker (None).
            """
            self.data = node_data
            self.next = None

        def __str__(self) -> str:
            """Return a string representation of the payload in this Node."""
            return str(self.data)

        def __repr__(self) -> str:
            """Return a string representation of this Node."""
            return 'Node({0})'.format(repr(self.data))

    # The data structure underlying this implementation of ADT Deque is a
    # circular singly-linked list.
    #
    # A LinkedDeque object has two instance variables:
    #
    #    _tail refers to the last (tail) node in the linked list, which
    #    corresponds to the right side of the deque.
    #    Because the linked list is circular, this node refers to the
    #    first (head) node in the linked list, which corresponds to the
    #    left side of the deque.
    #
    #    _size is the the number of items in the deque;
    #    i.e., the number of nodes in the linked list.

    def __init__(self, iterable=None) -> None:
        """Initialize this LinkedDeque with the contents of iterable.

        Items are appended to the right side of the deque in the order they are
        returned from the iterable. If iterable isn't provided, the new deque
        is empty.

        >>> dq = LinkedDeque()
        >>> str(dq)
        'None'

        >>> dq = LinkedDeque([10, 20, 30])
        >>> str(dq)
        [10, 20, 30]

        # Another way to create the same deque.
        >>> dq = LinkedDeque()
        >>> dq.append(10)
        >>> dq.append(20)
        >>> dq.append(30)
        >>> str(dq)
        [10, 20, 30]
        """
        self._tail = None
        self._size = 0
        if iterable is not None:
            for elem in iterable:
                # append will update _tail, increment _size
                self.append(elem)

    def __str__(self) -> str:
        """Return a string representation of this deque, in the format:
        '[value_1, value_2, value_3, ... value_n]'

        value_1 is the value at the left side of the deque.
        value_n is the value at the right side of the deque.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> str(dq)
        '[10, 20, 30]'

        >>> dq = LinkedDeque()
        >>> str(dq)
        'None'

        >>> dq.append(10)
        >>> str(dq)
        '[10]'
        >>> dq.append(20)
        >>> str(dq)
        '[10, 20]'
        >>> dq.append(30)
        >>> str(dq)
        '[10, 20, 30]'
        """
        if self._tail is None:
            return '[]'

        # Grab the reference to the leftmost node in the deque (the head node
        # of the linked list).
        node = self._tail.next

        # Traverse the linked list from the head node to the node immediately
        # before the tail node, collecting the data in the nodes as a list of
        # strings.
        items = []
        while node is not self._tail:
            items.append(repr(node.data))
            node = node.next

        # Now get the data from the rightmost node in the deque.
        items.append(node.data)

        # Concatenate the strings in items, with each pair separated by " -> "
        return str(items)

    def __repr__(self) -> str:
        """Return a string representation of this deque.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> repr(dq)
        'LinkedDeque([10, 20, 30])'
        >>> dq
        LinkedDeque([10, 20, 30])

        >>> dq = LinkedDeque()  # or, dq = LinkedDeque([])
        >>> dq
        LinkedDeque([])
        """
        items = []
        if self._tail is not None:
            # Grab the reference to the leftmost node in the deque (the head node
            # of the linked list).
            node = self._tail.next

            # Traverse the linked list from the head node to the node immediately
            # before the tail node, collecting the data in the nodes as a list of
            # strings.
            while node is not self._tail:
                items.append(node.data)
                node = node.next

            # Now get the data from the rightmost node in the deque.
            items.append(node.data)

        return '{0}({1})'.format(self.__class__.__name__, items)

    def __len__(self) -> int:
        """Return the number of items in this deque.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> len(dq)
        3
        """
        return self._size

    def append(self, item: Any) -> None:
        """Add item to the right side of this deque.

        If the deque is used as a queue, append enqueues item at the
        rear of the queue.

        >>> dq = LinkedDeque()
        >>> dq.append(10)
        >>> dq.append(20)
        >>> dq.append(30)
        >>> str(dq)
        [10, 20, 30]
        # 10 (the first item added) is at the left side of the deque,
        # 30 (the most recent item added) is at the right side.
        """
        
        #new list, therefore, just add a tail
        if(len(self) == 0):
            self._tail = LinkedDeque.Node(item)
            self._tail.next = self._tail
            self._size +=1
            return
            
        
        #create a new node
        newNode = LinkedDeque.Node(item)
        
        #get a pointer to the head of the existing list
        tempHead = self._tail.next
        
        #set 'next' of newNode as the head (since newNode is new tail)
        newNode.next = tempHead
        
        #have old tail point to new nail
        self._tail.next = newNode 
        
        #set tail as the newNode
        self._tail = newNode
        
        #increment size
        self._size +=1
        
        return
    

    def append_left(self, item: Any) -> None:
        """Add item to the left side of this deque.

        If the deque is used as a stack, append_left pushes an item on the top
        of the stack.

        >>> dq = LinkedDeque()
        >>> dq.append_left(10)
        >>> dq.append_left(20)
        >>> dq.append_left(30)
        >>> str(dq)
        '30 -> 20 -> 10'
        # 30 (the most recently-added item) is at the left side of the deque,
        # 10 (the first item added) is at the right side.
        """
        raise NotImplementedError("append_left hasn't been implemented.")

    def pop(self) -> Any:
        """Remove and return the item from the right side of this deque.

        Raise IndexError with the message, "pop: empty deque"
        if the deque is empty.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> dq.pop()
        30
        >>> dq.pop()
        20
        >>> dq.pop()
        10
        """
        #check to see if the deque is empty
        if(len(self) == 0):
            raise IndexError('pop:empty deque')
        
        #if only one item left in dq
        if(len(self) == 1):
            tempTail = self._tail
            self._tail = None 
            self._size -=1
            
            return tempTail
        
        #get pointer to current tail (which will be returned shortly)
        oldTail = self._tail
        
        #get pointer to node adjascent to the tail (right before the tail)
        i= len(self) - 1
        walkingPtr = self._tail
        while(i != 0):
            walkingPtr = walkingPtr.next
            i-= 1
        #this node is now the new tail 
        newTail = walkingPtr
        newTail.next = oldTail.next 
        self._tail = newTail
        
        #resize list
        self._size -=1
        
        return oldTail.data
        

    def pop_left(self) -> Any:
        """Remove and return the item from the left side of this deque.

        Raise IndexError with the message, "pop_left: empty deque"
        if the deque is empty.

        If the deque is used as a queue, pop_left dequeues an item from
        the front of the queue.
        If the deque is used as a stack, pop_left pops an item from the
        top of the stack.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> dq.pop_left()
        10
        >>> dq.pop_left()
        20
        >>> dq.pop_left()
        30
        """
        #raise an exception if the DQ is empty
        if(len(self) == 0 ):
            raise IndexError("pop_left: empty deque")
        
        #if one item left we need to set the _tail to point to None
        if(len(self) == 1):
            tempHead = self._tail
            self._tail = None
            self._size -=1
            
            return tempHead
        
        #grab reference to the old head (which will be returned shortly)
        oldHead = self._tail.next
        
        #the node under the head is now the new head, so set pointer accordingly
        self._tail.next = oldHead.next
        
        #update the size of the list
        self._size -=1
        
        return oldHead
        

    def peek(self) -> Any:
        """Return (but don't remove) the item from the right side of this deque.

        Raise IndexError with the message, "peek: empty deque"
        if the deque is empty.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> dq.peek()
        30
        """
        return self._tail.data

    def peek_left(self) -> Any:
        """Return (but don't remove) the item from the left side of this deque.

        Raise IndexError with the message, "peek_left: empty deque"
        if the deque is empty.

        >>> dq = LinkedDeque([10, 20, 30])
        >>> dq.peek_left()
        10
        """
        return self._tail.next.data 

#for debugging
if __name__ == "__main__":
    #testing append()
    dq = LinkedDeque([33,44,55,66,77,12])
    print('current size of dq is: ',len(dq))
    dq.append(33)
    print(dq)
    dq.append(-10)
    print(dq)
    dq.append(999)
    print(dq)
    print('current size of dq is: ',len(dq))
    
    #testing peek()
    tail = dq.peek()
    print(tail)
    
    #testing peek_left()
    head = dq.peek_left()
    print(head)
    
    #testing pop_left()
    print('starting pop_left()')
    for i in range (len(dq)):
        print(dq.pop_left())
    
    #testing pop()
    print('starting pop()')
    dq = LinkedDeque([33,44,55,66,77,12])
    for i in range(len(dq)):
        print(dq.pop())