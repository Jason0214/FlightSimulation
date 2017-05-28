#pragma once

template <class T>
class ListNode<T>{
public:
	ListNode(T item):item(T),next(NULL),pre(NULL){}
	ListNode(const ListNode<T> & node){
		this->item = node->item;
		this->next = node->next;
	}
	~ListNode(){};
	T item;
	ListNode<T>* pre;
	ListNode<T>* next;
};

template <class T>
class LinkList<T>{
public:
	LinkList():data(NULL),len(0),index_updated(false),index_len(0),index(NULL){}
	~LinkList(){
		this->Free();
		this->len = 0;
	}
	void Append(T item){
		new_node = new ListNode<T>(item); 
		if(this->index_updated) this->index_updated = false;		
		if(this->data){
			new_node->next = this->data->next;
			this->data->pre = new_node;
		}
		// add the new node to the head of linked list
		this->data = new_node;
		this->len++;
	}
	void Remove(ListNode<T>* node_ptr){
		if(node_ptr->pre){
			if(node_ptr->next){
				node_ptr->next->pre = node_ptr->pre;
			}
			node_ptr->pre->next = node_ptr->next;
		}
		else{
			if(node_ptr->next){
				node_ptr->next->pre = NULL;
			}
			this->data = node_ptr->next;
		}
		delete node_ptr;
		this->len--;
	}
	void Remove(const T item){
		ListNode<T>* ptr = this->data;
		while(ptr){
			if(item == ptr->item){
				this->Remove(ptr);
				break;
			}
			ptr = ptr->next;
		}
	}
	void join(const LinkList<T> & list){
		ListNode<T>* ptr = list.Begin();
		while(ptr){
			this->Append(ptr->item);
			ptr = ptr->next;
			this->len++;
		}
	}
	ListNode<T>* Find(const T item){
		ListNode<T>* ptr = this->data;
		while(ptr){
			if(ptr->item == item){
				break;
			}
		}
		return ptr;
	}
	ListNode<T>* Begin(){
		return this->data;
	}
	ListNode<T>* operator[](int index){
		if(!this->index_updated){
			// re index
			if(this->index_len != this->len){
				delete[] this->index;
				this->index = new ListNode<T>[len];
				this->index_len = this->len;
			}
			ListNode<T>* ptr = this->data;
			// iterate in reverse order since we add new node to the head of list
			for(int i = this->len-1; i >= 0; i--){
				this->index[i] = ptr;
				ptr = ptr->next;
			}
			this->index_updated = true;
		}
		return this->index[i];
	}
	void Free(){
		ListNode<T> *p = this->data;
		while(p){
			this->data = p->next;
			delete p;
			p = this->data;
		}
		this->data = NULL;
		this->len = 0;
	}
private:
	// temporarily deny
	LinkList(const LinkList<T> &);
	LinkList<T> & operator=(const LinkList<T> &);

	int len;
	ListNode<T>* data;
	// a smart way of implementing index to link list
	bool index_updated;
	int index_len;
	ListNode<T>** index;
};

template<K,V>
class HeapNode{
public:
	HeapNode(K key,V value):key(key),value(value){}
	HeapNode(K key):key(key){}
	~HeapNode(){}
	K key;
	V value;
};

template<K,V> //XXX  fib heap is a better choice
class MinHeap{
public:
	MinHeap(int size):len(0),capacity(size),data(NULL){
		this->data = new HeapNode<K,V>*[size+1];
		this->data[0] = new HeapNode<K,V>(-0xfffffff);
	}
	~MinHeap(){
		for(int i = 0;i <= this->len; i++){
			delete this->data[i];
		}
		delete[] this->data;
		this->data = NULL;
		this->len = 0;
		this->capacity = 0;
	}
	int Push(K key, V value){
		this->data[++this->len] = new HeapNode<K,V>(key,value);
		return this->PercolateUp(this->len);
	}
	V Pop(K key, V value){
		V ret = this->data[1]->value;
		delete this->data[1];
		this->data[1] = this->data[this->len--];
		this->PercolateDown(1);
		return ret;
	}
	int Update(int index, K new_key){
		if(this->data[index]->key > new_key){
			return this->PercolateDown(index);
		}
		else{
			return this->PercolateUp(index);
		}
	}
private:
	int PercolateUp(int index){
		HeapNode<K,V>* temp = this->data[index];
		for(;index > 1 && temp->key < this->data[index/2]->key; index /= 2){
			this->data[index] = this->data[index/2];
		}
		this->data[index] = temp;
		return index;
	}
	int PercolateDown(int index){
		HeapNode<K,V>* temp = this->data[index];
		int child;
		for(;index * 2 <= this->len; index = child){
			child = 2 * index;
			if(child+1 <= this->len && this->data[child]->key > this->data[child+1]->key){
				child++;
			}
			if(temp->key > this->data[child]->key){
				this->data[index] = this->data[child];
			}
			else{
				break;
			}
		}
		this->data[index] = temp;
		return index;
	}
	int len;
	int capacity;
	T* data;
};