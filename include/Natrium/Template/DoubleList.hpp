#if !defined(NA_LINKED_LIST_HPP)
#define NA_LINKED_LIST_HPP

#include "./ListNode.hpp"
#include "./DoubleListIterator.hpp"

namespace Na {
	template<typename T>
	class DoubleList {
	public:
		using Node = DoubleList_Node<DoubleList>;
		using iterator = DoubleList_Iterator<DoubleList>;
		using const_iterator = DoubleList_ConstIterator<DoubleList>;
		using reverse_iterator = DoubleList_ReverseIterator<DoubleList>;
		using const_reverse_iterator = DoubleList_ConstReverseIterator<DoubleList>;
		using T_t = T;
	public:
		DoubleList(void) = default;
		~DoubleList(void) { this->clear(); }

		void clear(void) { while (this->pop_back()); }

		template<typename t_Iterator>
		DoubleList(const t_Iterator& begin, const t_Iterator& end)
		{
			for (t_Iterator it = begin; it != end; it++)
				this->emplace_back(*it);
		}

		DoubleList(const T* buffer, u64 size)
		{
			while (m_Size < size)
				this->emplace_back(buffer[m_Size]);
		}

		DoubleList(T* buffer, u64 size)
		{
			while (m_Size < size)
				this->emplace_back(std::move(buffer[m_Size]));
		}

		DoubleList(const std::initializer_list<T>& list)
		: DoubleList(list.begin(), list.size()) {}

		DoubleList(const DoubleList& other)
		: DoubleList(other.begin(), other.end())
		{}

		DoubleList& operator=(const DoubleList& other)
		{
			if (this == &other)
				return *this;
			
			this->clear();

			for (const T& data : other)
				this->emplace_back(data);

			return *this;
		}

		DoubleList(DoubleList&& other)
		: m_Head(std::exchange(other.m_Head, nullptr)),
		m_Tail(std::exchange(other.m_Tail, nullptr)),
		m_Size(std::exchange(other.m_Size, 0))
		{}

		DoubleList& operator=(DoubleList&& other)
		{
			if (this == &other)
				return *this;

			this->clear();

			m_Head = std::exchange(other.m_Head, nullptr);
			m_Tail = std::exchange(other.m_Tail, nullptr);
			m_Size = std::exchange(other.m_Size, 0);
			
			return *this;
		}

		template<typename... t_Args>
		T* emplace_back(t_Args&&... __args)
		{
			if (!m_Size++)
				return &(this->_emplace_empty(std::forward<t_Args>(__args)...)->data);

			m_Tail = new Node(nullptr, m_Tail, std::forward<t_Args>(__args)...);
			return &((m_Tail->previous->next = m_Tail)->data);
		}

		template<typename... t_Args>
		T* emplace_front(t_Args&&... __args)
		{
			if (!m_Size++)
				return &(this->_emplace_empty(std::forward<t_Args>(__args)...)->data);

			m_Head = new Node(m_Head, nullptr, std::forward<t_Args>(__args)...);
			return &((m_Head->next->previous = m_Head)->data);
		}

		template<typename... t_Args>
		T* emplace_at(u64 index, t_Args&&... __args)
		{
			if (!m_Size++)
				return &(this->_emplace_empty(std::forward<t_Args>(__args)...)->data);

			if (index >= m_Size)
			{
				m_Tail = new Node(nullptr, m_Tail, std::forward<t_Args>(__args)...);
				return &((m_Tail->previous->next = m_Tail)->data);
			}

			Node* node = this->at(index).node;
			node = new Node(node, node->previous, std::forward<t_Args>(__args)...);
			if (node->previous)
				node->previous->next = node;
			return &((node->next->previous = node)->data);
		}

		bool pop_back(void)
		{
			if (!m_Head)
				return false;

			m_Size--;
			if ((m_Tail = m_Tail->previous))
			{
				delete m_Tail->next;
				m_Tail->next = nullptr;
			} else
			{
				delete m_Head;
				m_Head = nullptr;
			}
			return true;
		}

		bool pop_front(void)
		{
			if (!m_Head)
				return false;

			m_Size--;
			if ((m_Head = m_Head->next))
			{
				delete m_Head->previous;
				m_Head->previous = nullptr;
			} else
			{
				delete m_Tail;
				m_Tail = nullptr;
			}
			return true;
		}

		bool pop_at(u64 index)
		{
			if (!m_Head)
				return false;

			if (!--m_Size && index)
				return false;

			if (index > m_Size)
				return false;

			Node* node = this->at(index).node;

			if (!node->is_tail())
				node->next->previous = node->previous;
			else
				m_Tail = node->previous;

			if (!node->is_head())
				node->previous->next = node->next;
			else
				m_Head = node->next;

			delete node;
			return true;
		}

		bool pop(T* data)
		{
			if (!find(data))
				return false;

			Node* node = (Node*)((Byte*)data - offsetof(Node, data));
			if (!node->is_tail())
				node->next->previous = node->previous;
			else
				m_Tail = node->previous;

			if (!node->is_head())
				node->previous->next = node->next;
			else
				m_Head = node->next;

			m_Size--;
			delete node;
			return true;
		}

		[[nodiscard]] bool find(const T* data) const
		{
			if (!data) return false;
			for (const T& it : *this)
				if (&it == data)
					return true;
			return false;
		}

		bool swap(Node* node1, Node* node2)
		{
			if (!node1 || !node2 || node1 == node2)
				return false;

			Node* temp = node1->next;

			if ((node1->next = node2->next))
				node1->next->previous = node1;
			else
				m_Tail = node1;

			if ((node2->next = temp))
				node2->next->previous = node2;
			else
				m_Tail = node2;

			temp = node1->previous;

			if ((node1->previous = node2->previous))
				node1->previous->next = node1;
			else
				m_Head = node1;

			if ((node2->previous = temp))
				node2->previous->next = node2;
			else
				m_Head = node2;

			return true;
		}
		inline bool swap(u64 index1, u64 index2) { return this->swap(this->at(index1).node, this->at(index2).node); }
		inline bool swap(iterator it1, iterator it2) { return this->swap(it1.node, it2.node); }

		[[nodiscard]] inline iterator begin(void) { return m_Head; }
		[[nodiscard]] inline const_iterator begin(void) const { return m_Head; }
		[[nodiscard]] inline const_iterator cbegin(void) const { return m_Head; }

		[[nodiscard]] inline iterator end(void) { return nullptr; }
		[[nodiscard]] inline const_iterator end(void) const { return nullptr; }
		[[nodiscard]] inline const_iterator cend(void) const { return nullptr; }

		[[nodiscard]] inline reverse_iterator rbegin(void) { return m_Tail; }
		[[nodiscard]] inline const_reverse_iterator rbegin(void) const { return m_Tail; }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return m_Tail; }

		[[nodiscard]] inline reverse_iterator rend(void) { return nullptr; }
		[[nodiscard]] inline const_reverse_iterator rend(void) const { return nullptr; }
		[[nodiscard]] inline const_reverse_iterator crend(void) const { return nullptr; }

		[[nodiscard]] iterator at(u64 index)
		{
			if (index > m_Size * 0.5)
				return iterator(m_Tail, m_Size - index - 1, backwards);
			return iterator(m_Head, index, forwards);
		}
		[[nodiscard]] const_iterator at(u64 index) const
		{
			if (index > m_Size * 0.5)
				return const_iterator(m_Tail, m_Size - index - 1, backwards);
			return const_iterator(m_Head, index, forwards);
		}

		[[nodiscard]] inline T& operator[](u64 index) { return *(this->at(index)); }
		[[nodiscard]] inline const T& operator[](u64 index) const { return *(this->at(index)); }

		[[nodiscard]] inline T& head(void) { return *m_Head; }
		[[nodiscard]] inline const T& head(void) const { return *m_Head; }

		[[nodiscard]] inline T& tail(void) { return *m_Tail; }
		[[nodiscard]] inline const T& tail(void) const { return *m_Tail; }

		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline bool empty(void) const { return !m_Size; }
	private:
		template<typename... t_Args>
		Node* _emplace_empty(t_Args&&... __args)
		{
			m_Head = new Node(nullptr, nullptr, std::forward<t_Args>(__args)...);
			return m_Tail = m_Head;
		}
	private:
		Node* m_Head = nullptr;
		Node* m_Tail = nullptr;
		u64 m_Size = 0;
	};
} // namespace Neo

#endif // NA_LINKED_LIST_HPP
