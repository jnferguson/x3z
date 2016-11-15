#ifndef HAVE_HUFFMAN_T_HPP
#define HAVE_HUFFMAN_T_HPP

/*
 * This class is potentially, loosely a derrived work based on the code sample available at 
 * http://www.codeproject.com/Articles/25088/Huffman-compression-class-in-C
 * by Chesnokov Yuriy. Essentially it was used as a skeleton code base reference and then cleaned up and modified to fit my needs, only some general
 * structure of the original remains, but for clarity and safety, it is necessary to state that it is potentially deemed as a derrived work and thus
 * subject to the original license of Mr. Yuriy's work, and thus licensed under the GPL v3.
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <limits>
#include <list>
#include <queue>
#include <stdexcept>
#include <functional>

#include "exception.hpp"
#include "util.hpp"

#define HUFFMAN_HEADER_MAGIC 0x69654945

#pragma pack(push, 1)
struct huffman_header_t
{
	uint32_t		magic;
	uint16_t		symbol_number;
	uint16_t		sfreq_size;
	uint64_t		uncompressed_size;
	uint64_t		compressed_size;

	huffman_header_t(uint8_t sn = 0, uint8_t ss = 0, uint64_t us = 0, uint64_t cs = 0) 
		: magic(HUFFMAN_HEADER_MAGIC), symbol_number(sn), sfreq_size(ss), 
		uncompressed_size(us), compressed_size(0)
	{
		return;
	}
	~huffman_header_t(void)
	{
		symbol_number		= 0;
		sfreq_size			= 0;
		uncompressed_size	= 0;
		compressed_size		= 0;

		return;
	}
};

struct huffman_symbol_byte_t
{
	uint8_t symbol;
	uint8_t frequency;

	huffman_symbol_byte_t(uint8_t s = 0, uint8_t f = 0) : symbol(s), frequency(f) { }
	~huffman_symbol_byte_t(void) { symbol = 0; frequency = 0; return; }
};

struct huffman_symbol_word_t
{
	uint8_t		symbol;
	uint16_t	frequency;

	huffman_symbol_word_t(uint8_t s = 0, uint16_t f = 0) : symbol(s), frequency(f) { }
	~huffman_symbol_word_t(void) { symbol = 0; frequency = 0; return; }

};

struct huffman_symbol_dword_t
{
	uint8_t		symbol;
	uint32_t	frequency;

	huffman_symbol_dword_t(uint8_t s = 0, uint32_t f = 0, uint8_t m = 0) : symbol(s), frequency(f) { }
	~huffman_symbol_dword_t(void) { symbol = 0; frequency = 0; return; }

};

struct huffman_symbol_qword_t
{
	uint8_t		symbol;
	uint64_t	frequency;	

	huffman_symbol_qword_t(uint8_t s = 0, uint64_t f = 0, uint8_t m = 0) : symbol(s), frequency(f) { }
	~huffman_symbol_qword_t(void) { symbol = 0; frequency = 0; return; }

};

struct huffman_symbol_t
{
	uint8_t		symbol;
	uint64_t	frequency;

	huffman_symbol_t(uint8_t s = 0, uint64_t f = 0) : symbol(s), frequency(f) { }
	~huffman_symbol_t(void) { symbol = 0; frequency = 0; return; }
};

#pragma pack(pop)

class inode_t
{
	private:
	protected:
		const uint64_t	m_weight;

		inode_t(const uint64_t weight) : m_weight(weight) { }
	public:
		virtual ~inode_t(void) { }
		virtual const uint64_t weight(void) const { return m_weight; }
};

class internal_node_t : public inode_t
{
	private:
		const inode_t*	m_left;
		const inode_t*	m_right;

	protected:
	public:
		internal_node_t(inode_t* left, inode_t* right)
			: inode_t(left->weight() + right->weight()), m_left(left), m_right(right)
		{
		}

		internal_node_t(const inode_t* left, const inode_t* right)
			: inode_t(left->weight() + right->weight()), m_left(left), m_right(right)
		{
		}

		~internal_node_t(void)
		{
			delete m_left;
			delete m_right;
			m_left = nullptr;
			m_right = nullptr;
		}

		const inode_t* left(void) const { return m_left; }
		const inode_t* right(void) const { return m_right; }
};

template< typename T, typename std::enable_if< std::is_integral< T >::value, bool >::type = true >
class leaf_t : public inode_t
{
	private:
	protected:
		const T		m_data;

	public:
		leaf_t(const uint64_t weight, const T data) : inode_t(weight), m_data(data)
		{
			/* It would be nice if there was a variant of std::is_integral
 			 * That only worked on 'integer' et al types; sadly not possible
			 * because of the way bool is 'special'.
			 */
			if ( typeid( bool ) == typeid( T ) )
				throw std::exception("leaf_t::leaf_t(): Instantiated with 'bool' type, "
										 "which is probably not what you wanted.");

			return;
		}

		virtual ~leaf_t(void) { }
		virtual const T data(void) const { return m_data; }
};

typedef std::vector< bool > hct_t;

struct symbol_t
{
	std::vector< bool >	vector;

	symbol_t(void) { }
	symbol_t(std::vector< bool >& v) : vector(v) { }
	~symbol_t(void) { vector.clear(); return; }
};

typedef std::map< uint8_t, symbol_t > hmap_t;

class htree_t
{
	private:
		htree_t(const htree_t&) = delete;
		const htree_t& operator=(const htree_t&) = delete;

	protected:
		inode_t*	m_tree;
		hmap_t		m_codes;
		uint8_t		m_bits;
		uint8_t		m_byte;
		uint16_t	m_sfl;
		uint64_t	m_fsz;
		uint16_t	m_sn;

		virtual inode_t* build_tree(std::vector< uint8_t >& dst, std::vector< uint8_t >& src);
		virtual void generate_codes(const inode_t* node, hmap_t& map, hct_t& prefix);
		virtual inode_t* read_tree(std::vector< uint8_t >& src);

		static inline uint8_t byte_count(uint64_t val);
		static inline std::size_t symbol_struct_size(std::size_t bc);

		inline bool write_bit(uint8_t bit);
		inline void write_symbol(std::vector< uint8_t >& dst, std::size_t c);
		inline uint8_t read_bit(std::vector< uint8_t >::iterator& sitr, std::vector< uint8_t >::iterator& eitr);
		inline uint8_t read_symbol(std::vector< uint8_t >::iterator& sitr, std::vector< uint8_t >::iterator& eitr);

	public:
		htree_t(inode_t* t = nullptr);
		virtual ~htree_t(void);

		virtual void reset(void);

		virtual bool encode(std::vector< uint8_t >& dst, std::vector< uint8_t >& src);
		virtual bool decode(std::vector< uint8_t >& dst, std::vector< uint8_t >& src);
};

using compare_ptr_t = bool(*)( const inode_t*, const inode_t* );

class sorted_queue_t
{
	private:
		std::vector< inode_t* >		m_container;
		compare_ptr_t				m_sort;

	protected:
		static bool iascsort(const inode_t* lhs, const inode_t* rhs)
		{
			if ( lhs->weight() == rhs->weight() ) {
				const leaf_t< uint8_t >* left(dynamic_cast<const leaf_t< uint8_t >*>( lhs ));
				const leaf_t< uint8_t >* right(dynamic_cast<const leaf_t< uint8_t >*>( rhs ));

				/*
				 * I *believe* this makes the dual sort strict weak ordering
				 * compliant; we return false if one node or the other is not
				 * a leaf or if their lexographic value is equal (which shouldnt
				 * be possible in the first place), otherwise we compare the
				 * values.
				 */
				if ( nullptr == left && nullptr != right )
					return false;
				else  if ( nullptr != left && nullptr == right )
					return true;
				else if ( nullptr == left && nullptr == right )
					return false;

				/*if (	nullptr == left && nullptr == right ||
						nullptr != left && nullptr == right || 
						nullptr == left && nullptr != right )
					return false;*/
				else if ( left->data() == right->data() )
					return false;

				return left->data() > right->data();
			}

		return lhs->weight() > rhs->weight();
	}

	public:
	sorted_queue_t(void) : m_sort(&sorted_queue_t::iascsort) {}

	sorted_queue_t(const std::vector< inode_t* >& container)
		: m_container(container) , m_sort(&sorted_queue_t::iascsort)
	{}

	void set_sort(compare_ptr_t& s)
	{
		m_sort = s;
		return;
	}

	sorted_queue_t&
	operator=(const sorted_queue_t& h)
	{
		if ( &h != this )
			m_container = h.m_container;

		return *this;
	}

	void
	sort(std::function< bool(const inode_t*, const inode_t*)>& comp)
	{
		std::sort(m_container.begin(), m_container.end(), comp);
		return;
	}

	void
	sort(void)
	{
		std::sort(m_container.begin(), m_container.end(), m_sort);
		return;
	}

	void
	rsort(void)
	{
		sort();
		std::reverse(m_container.begin(), m_container.end());
		return;
	}

	void
	push(inode_t* x, bool auto_sort = true)
	{
		m_container.push_back(x);

		if ( true == auto_sort )
			sort();

		return;
	}

	void
	pop(void)
	{
		if ( true == m_container.empty() )
			return;

		if (1 != m_container.size())
			m_container.erase(m_container.begin(), m_container.begin() + 1);
		else
			m_container.pop_back();

		return;
	}

	inode_t*
	pop_top(void)
	{
		inode_t* ret(nullptr);
	
		if ( 0 == m_container.size() )
			throw std::exception("sorted_queue_t::pop_top(): Attempted to pop_top() empty queue");

		ret = m_container.front();
		pop();

		return ret;
	}

	inode_t*
	top(void) 	
	{
		if ( 0 == m_container.size() )
			throw std::exception("sorted_queue_t::top(): Attempted to retrieve top() of empty queue");

		return m_container.front();
	}

	const inode_t*
	operator[](std::size_t n) const
	{
		return const_cast< const inode_t* >( m_container[ n ] );
	}

	const inode_t*
	at(std::size_t n) const
	{
		return const_cast< const inode_t* >( m_container.at(n) );
	}

	std::vector< inode_t* >::iterator
	begin(void)
	{
		return m_container.begin();
	}

	std::vector< inode_t* >::iterator
	end(void)
	{
		return m_container.end();
	}

	std::vector< inode_t* >::const_reverse_iterator
	crbegin(void)
	{
		return m_container.crbegin();
	}

	std::vector< inode_t* >::const_reverse_iterator
	crend(void)
	{
		return m_container.crend();
	}

	std::vector< inode_t* >::reverse_iterator
	rbegin(void)
	{
		return m_container.rbegin();
	}

	std::vector< inode_t* >::reverse_iterator
	rend(void)
	{
		return m_container.rend();
	}

	std::vector< inode_t* >::const_iterator
	cbegin(void) const
	{
		return m_container.cbegin();
	}

	std::vector< inode_t* >::const_iterator
	cend(void) const
	{
		return m_container.cend();
	}

	std::size_t
	size(void) const
	{
		return m_container.size();
	}

	std::vector< inode_t* >::iterator
	erase(std::vector< inode_t* >::iterator pos)
	{
		auto& itr = m_container.erase(pos);
		return itr;
	}
};

#endif
