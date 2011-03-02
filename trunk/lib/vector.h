/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition of a vector class and its standard operations. The vector is
 * similar to a stack, but its internal elements can also be accessed. The
 * vector also uses dynamic memory.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#ifndef _KECOSATS_VECTOR_
#define _KECOSATS_VECTOR_

//These are the default values for a vector's settings
#define DEFAULT_BLOCK_SIZE 1000
#define DEFAULT_SEGMENT_SIZE 100
#define DEFAULT_SEGMENT_EXT 10
#define MAX_FREE_SEGMENT_PERCENTAGE 0.3

/**
 * The type T defines the internal type of each vector
 */
typedef int T;
   
/**
 * A vector is comprised of several elements that defines its
 * behaviour.
 * Basically a vector has several blocks of elements, the size
 * of each block is always fixed. When more memory is needed,
 * a block of elements is allocated. All the blocks pointer's
 * are stored in a segment: an array of pointers to blocks. A vector
 * has an initial segment size, nevertheless, if more blocks are needed
 * than the capacity of a segment, the segment is reallocated in a bigger
 * one, the size is augmented by a fixed size.
 *
 * @param block_size The size of each of the blocks
 * @param segment_size The size of the initial segment
 * @param segment_ext When the segment size is not enough for
 * holding all the needed blocks, then the segment is reallocated
 * with 'segment_ext' additional cells
 * @param array_of_T The segment
 * @param size The number of elements in the vector
 * @param top_seg The block's index of the vector's next free cell
 * @param top_bi The element's index of the next free cell (inside its
 * respective block)
 * @param alloc_seg The number of allocated blocks in the vector
 */
typedef struct vector_{
    int block_size;
    int segment_size;
    int segment_ext;

    T **array_of_T;
    int size;
    int top_seg;
    int top_bi;
    int alloc_seg;
} vector;

/**
 * Returns a new vector, and sets its attributes as the default values.
 * @return A pointer to a vector with default values
 */
inline vector* new_vector();

/**
 * Creates and returns a new vector with the specified
 * values for its attributes.
 * @param block_size The size of each block of elements
 * @param segment_size The initial size for the segment that
 * will hold blocks
 * @param segment_ext The number of additional segments that are
 * added when new segments are needed
 * @return A pointer to a vector with 
 */
inline vector* new_vector_spec(int block_size, int segment_size,
        int segment_ext);

/**
 * Expands the size of the vector by one unit and places the 
 * given element in the last position of the vector
 * @param v The vector where to perform the operation
 * @param item The item to push back in the vector
 */
inline void push(vector* v, T item);

/**
 * Returns a non-zero integer if the vector is empty,
 * otherwise, it returns zero
 * @param v The vector where to perform the operation
 * @return Zero if the vector is not empty, a non-zero integer
 * otherwise
 */
inline int empty(vector* v);

/**
 * Returns the last element in the vector
 * @param v The vector where to perform the operation
 */
inline T top(vector* v);

/**
 * Decreases the size of the vector by one unit
 * @param v The vector where to perform the operation
 */
inline void pop(vector* v);

/**
 * Decreases the vector's size by the given number of units
 * @param v The vector where to perform the operation
 * @param n The number of units to decrease the vector's size
 */
inline void multi_pop(vector* v, int n);

/**
 * Returns the 'i'th element of the vector
 * @param v The vector where to perform the operation
 * @param i The index of the element to return
 * @return The 'i'th element of the vector
 */
inline T get(vector *v, int i);

/**
 * Sets the vector's 'i'th element to the value of x
 * @param v The vector where to perform the operation
 * @param i The index of the element to set
 * @param x The value to set to the element
 */
inline void set(vector *v, int i, T x);

/**
 * Frees the vector and all the space it uses, however,
 * if an element of the vector includes pointers to other
 * structures, those structures are not freed
 * @param v The vector where to perform the operation
 */
inline void free_vector(vector *v);

/**
 * Frees a vector and the elements inside it, however, a
 * function that frees the space of each element must be
 * provided
 * @param v The vector where to perform the operation
 * @param del_T A function that receives a pointer to one
 * of the vector's elements and frees it space
 */
inline void delete_vector(vector *v, void (*del_T)(T*));

#endif
