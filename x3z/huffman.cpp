#include "stdafx.h"
#include "huffman.hpp"

htree_t::htree_t(inode_t* t) 
	: m_tree(t), m_bits(0), m_byte(0), m_sfl(0), m_fsz(0), m_sn(0) 
{
	return;
}

htree_t::~htree_t(void)
{
	reset();
	return;
}

void
htree_t::reset(void)
{
	if ( nullptr != m_tree )
		delete m_tree;

	m_bits	= 0; 
	m_byte	= 0; 
	m_sfl	= 0; 
	m_fsz	= 0; 
	m_sn	= 0;
	m_tree	= nullptr;

	m_codes.clear();
	return;
}

bool
htree_t::encode(std::vector< uint8_t >& dst, std::vector< uint8_t >& src)
{
	huffman_header_t* hdr(nullptr);

	if ( 0 == src.size() )
		return false;

	dst.clear();
	reset();

	m_fsz = src.size();

	m_tree = build_tree(dst, src);
	generate_codes(m_tree, m_codes, hct_t());

	for ( auto& byte : src )
		write_symbol(dst, byte);

	if ( 8 != m_bits && 0 != m_bits ) {
		while ( false == write_bit(0) )
			;

		dst.push_back(m_byte);
		m_byte = 0;
	}

	// unpossible / pedantry
	if ( sizeof(huffman_header_t) > dst.size() )
		return false;

	hdr						= reinterpret_cast< huffman_header_t* >( &dst[ 0 ] );
	hdr->magic				= byte_swap(static_cast< uint32_t >(HUFFMAN_HEADER_MAGIC));
	hdr->compressed_size	= uint64_t(dst.size());
	hdr->uncompressed_size	= m_fsz;
	hdr->sfreq_size			= static_cast< uint8_t >( m_sfl );
	hdr->symbol_number		= m_sn;

	return true;
}

bool
htree_t::decode(std::vector< uint8_t >& dst, std::vector< uint8_t >& src)
{
	huffman_header_t*					hdr(nullptr);
	std::vector< uint8_t >::iterator	sitr(src.begin());
	std::vector< uint8_t >::iterator	eitr(src.end());
	uint64_t							length(0);

	if ( 0 == src.size() || sizeof(huffman_header_t) > src.size() )
		return false;

	dst.clear();

	reset();

	hdr = reinterpret_cast< huffman_header_t* >( &src[ 0 ] );
	m_fsz = hdr->uncompressed_size;
	m_sn = hdr->symbol_number;
	m_sfl = hdr->sfreq_size;
	m_bits = 8;

	if ( HUFFMAN_HEADER_MAGIC != byte_swap(hdr->magic) && HUFFMAN_HEADER_MAGIC != hdr->magic)
		return false;
	else if ( hdr->compressed_size != src.size() )
		return false;
	else if ( hdr->symbol_number > SIZE_MAX / symbol_struct_size(m_sfl) )
		return false;
	else if ( ( hdr->symbol_number * symbol_struct_size(m_sfl) ) > SIZE_MAX - sizeof(huffman_header_t) )
		return false;
	else if ( ( hdr->symbol_number * symbol_struct_size(m_sfl) ) + sizeof(huffman_header_t) > src.size() )
		return false;

	sitr += sizeof(huffman_header_t) + ( std::size_t(hdr->symbol_number * symbol_struct_size(m_sfl)) );

	if ( ( hdr->symbol_number * symbol_struct_size(m_sfl) ) + sizeof(huffman_header_t) != ( src.size() - ( eitr - sitr ) ) )
		return false;

	m_tree = read_tree(src);

	if ( nullptr == m_tree )
		return false;

	do {
		dst.push_back(read_symbol(sitr, eitr));
	} while ( --m_fsz );

	return true;
}

inline uint8_t
htree_t::read_bit(std::vector< uint8_t >::iterator& sitr, std::vector< uint8_t >::iterator& eitr)
{
	if ( sitr == eitr )
		throw std::exception("htree_t::read_bit(): Invalid iterator state (entry)");

	if ( 0 != m_bits )
		return 0x01 & ( *sitr >> ( --m_bits ) );
	else {
		sitr++;

		if ( sitr == eitr )
			throw std::exception("htree_t::read_bit(): Invalid iterator state (increment)");

		m_bits = 8;
		return 0x01 & ( *sitr >> ( --m_bits ) );
	}

	// pedantry
	throw std::exception("htree_t::read_bit(): Invalid object state (impossible code path)");
	return 0;
}

inline uint8_t
htree_t::read_symbol(std::vector< uint8_t >::iterator& sitr, std::vector< uint8_t >::iterator& eitr)
{
	inode_t*			node(m_tree);
	internal_node_t*	inode(nullptr);
	leaf_t< uint8_t >*	leaf(nullptr);

	if ( nullptr == m_tree )
		throw std::exception("htree_t::read_symbol(): Invalid object state (nullptr)");

	while ( 1 ) {
		leaf = dynamic_cast< leaf_t< uint8_t >* >( node );

		if ( nullptr != leaf )
			return leaf->data();

		if ( 0 != read_bit(sitr, eitr) ) {
			inode = dynamic_cast< internal_node_t* >( node );

			if ( nullptr != inode && nullptr != inode->right() )
				node = const_cast< inode_t* >( inode->right() );
			else
				throw std::exception("htree_t::read_symbol(): Invalid tree state (right)");

		} else {
			inode = dynamic_cast< internal_node_t* >( node );

			if ( nullptr != inode && nullptr != inode->left() )
				node = const_cast< inode_t* >( inode->left() );
			else
				throw std::exception("htree_t::read_symbol(): Invalid tree state (left)");
		}
	}

	throw std::exception("htree_t::read_symbol(): Invalid object state (impossible code path)");
}

inode_t*
htree_t::build_tree(std::vector< uint8_t >& dst, std::vector< uint8_t >& src)
{
	std::array< uint64_t, 256 >		map;
	huffman_header_t*				hdr(nullptr);
	uint64_t						sfl(0);
	sorted_queue_t					sq;


	std::memset(&map[ 0 ], 0, 256 * sizeof(uint64_t));

	for ( auto& itr : src )
		map[ itr ]++;


	for ( std::size_t idx = 0; idx < 256; idx++ ) {
		if ( 0 != map[ idx ] ) {
			leaf_t< uint8_t >* leaf(new leaf_t< uint8_t >(map[ idx ], ( idx & UINT8_MAX )));

			if ( map[ idx ] > sfl )
				sfl = map[ idx ];

			sq.push(leaf);
		}
	}

	if ( static_cast<std::size_t>( UINT8_MAX + 1 ) < sq.size() )
		throw std::exception("htree_t::build_tree(): Impossible error occurred; (over max symbols)");

	m_sfl	= byte_count(sfl);
	m_sn	= static_cast< uint16_t >( sq.size() );
	sq.sort();

	dst.resize(sizeof(huffman_header_t));

	hdr						= reinterpret_cast< huffman_header_t* >( &dst[ 0 ] );
	hdr->symbol_number		= static_cast< uint16_t >( m_sn ); //sq.size() );
	hdr->uncompressed_size	= 0;
	hdr->compressed_size	= 0;

	for ( std::size_t idx = 0; idx < sq.size(); idx++) { 
		std::vector< uint8_t > tmp;

		switch ( m_sfl ) {
			case sizeof(uint8_t) :
				tmp = byte_to_vector(static_cast< uint8_t >( sq.at(idx)->weight() ));
				break;
			case sizeof(uint16_t) :
				tmp = word_to_vector(static_cast< uint16_t >( sq.at(idx)->weight() ));
				break;

			case sizeof(uint32_t) :
				tmp = dword_to_vector(static_cast< uint32_t >( sq.at(idx)->weight() ));
				break;

			case sizeof(uint64_t) :
				tmp = qword_to_vector(sq.at(idx)->weight());
				break;

			default:
				throw std::exception("htree_t::build_tree(): Invalid data state (byte count)");
				break;
		}

		if ( nullptr == dynamic_cast< const leaf_t< uint8_t >* >( sq.at(idx) ) )
			throw std::exception("htree_t::build_t(): Dynamic cast to leaf_t failed, shouldnt be possible");

		dst.push_back(dynamic_cast< const leaf_t< uint8_t >* >( sq.at(idx) )->data());
		dst.insert(dst.end(), tmp.begin(), tmp.end());
	}

	sq.rsort();

	while ( 1 < sq.size() ) {
		inode_t* p(new internal_node_t(sq.pop_top(), sq.pop_top()));

		sq.push(p, false);
		sq.rsort();
	}

	return sq.top();
}

void
htree_t::generate_codes(const inode_t* node, hmap_t& map, hct_t& prefix)
{
	if ( const leaf_t< uint8_t >* lf = dynamic_cast<const leaf_t< uint8_t >*>( node ) ) 
		map[ lf->data() ].vector = prefix;
	else if ( const internal_node_t* in = dynamic_cast<const internal_node_t*>( node ) ) {
		std::vector< bool > lpv(prefix), rpv(prefix);

		lpv.push_back(false);
		generate_codes(in->left(), map, lpv);

		rpv.push_back(true);
		generate_codes(in->right(), map, rpv); 
	}

	return;
}

inline uint8_t
htree_t::byte_count(uint64_t val)
{
	if ( std::numeric_limits< uint8_t >::max() >= val )
		return sizeof(uint8_t);
	else if ( std::numeric_limits< uint16_t >::max() >= val )
		return sizeof(uint16_t);
	else if ( std::numeric_limits< uint32_t >::max() >= val )
		return sizeof(uint32_t);
	else
		return sizeof(uint64_t);
}

inline std::size_t
htree_t::symbol_struct_size(std::size_t bc)
{
	std::size_t retval(0);

	switch ( bc ) {
		case sizeof(uint8_t) :
			retval = sizeof(huffman_symbol_byte_t);
			break;

		case sizeof(uint16_t) :
			retval = sizeof(huffman_symbol_word_t);
			break;

		case sizeof(uint32_t) :
			retval = sizeof(huffman_symbol_dword_t);
			break;

		case sizeof(uint64_t) :
			retval = sizeof(huffman_symbol_qword_t);
			break;

		default:
			throw std::exception("htree_t::symbol_struct_size(): Invalid data state (byte count)");
			break;
	}

	return retval;
}

inline bool
htree_t::write_bit(uint8_t bit)
{
	m_byte |= ( bit ? 1 : 0 );
	m_bits++;

	if ( 8 == m_bits ) {
		m_bits = 0;
		return true;
	} else
		m_byte <<= 1;

	return false;
}

inline void
htree_t::write_symbol(std::vector< uint8_t >& dst, std::size_t c)
{
	const uint8_t	bidx(c & UINT8_MAX);
	std::vector< bool >& bits(m_codes[ bidx ].vector);

	for ( auto& bit = bits.begin(); bit != bits.end(); bit++ ) {
		if ( true == write_bit(( true == *bit ? 1 : 0 )) ) {
			dst.push_back(m_byte);
			m_byte = 0;
		}
	}

	return;
}

inode_t*
htree_t::read_tree(std::vector< uint8_t >& src)
{
	huffman_symbol_t*	sym(nullptr);
	sorted_queue_t		sq;
	uint64_t			sqsc(0);

	sym = reinterpret_cast< huffman_symbol_t* >( &src[ sizeof(huffman_header_t) ] );

	for ( std::size_t idx = 0; idx < m_sn; idx++ ) {
		switch ( m_sfl ) {
			case sizeof(uint8_t) :
			{
				huffman_symbol_byte_t*	hb(nullptr);

				hb = reinterpret_cast< huffman_symbol_byte_t* >( sym );
				sq.push(new leaf_t< uint8_t >(hb->frequency, hb->symbol));
				hb++;
				sym = reinterpret_cast< huffman_symbol_t* >( hb );
				break;
			}
			case sizeof(uint16_t) :
			{
				huffman_symbol_word_t*	hw(nullptr);

				hw = reinterpret_cast< huffman_symbol_word_t* >( sym );
				sq.push(new leaf_t< uint8_t >(big_to_little(static_cast< uint16_t >( hw->frequency )), hw->symbol));
				hw++;
				sym = reinterpret_cast< huffman_symbol_t* >( hw );
				break;
			}
			case sizeof(uint32_t) :
			{
				huffman_symbol_dword_t*	hd(nullptr);

				hd = reinterpret_cast< huffman_symbol_dword_t* >( sym );
				sq.push(new leaf_t< uint8_t >(big_to_little(static_cast< uint32_t >( hd->frequency )), hd->symbol ));
				hd++;
				sym = reinterpret_cast< huffman_symbol_t* >( hd );
				break;
			}
			case sizeof(uint64_t) :
				sq.push(new leaf_t< uint8_t >(big_to_little(static_cast< uint64_t >( sym->frequency )), sym->symbol));
				sym++;
				break;

			default:
				throw std::exception("htree_t::read_tree(): Invalid data state (byte count)");
				break;
		}
	}

	sq.rsort();
	sqsc = sq.size();
	while (1 < sqsc) { //sq.size()) {
		inode_t* p(new internal_node_t(sq.pop_top(), sq.pop_top()));
		sq.push(p, false);
		sq.rsort();
		sqsc = sq.size();
	}

	return sq.top();
}
