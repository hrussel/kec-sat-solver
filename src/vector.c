/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for a vector and its methods
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#include <stdlib.h>
#include "vector.h"

//private functions' definition

/**
 * This function checks if a relevant percentage (30% by default)
 * of the allocated blocks are unused, in which case they are freed.
 * This percentage can be chaged by changing the MAX_FREE_SEGMENT_PERCENTAGE
 * value
 * @param v The vector where to perform the operation
 */
void check_and_free_blocks(vector* v);


inline vector* new_vector(){
    return new_vector_spec(
        DEFAULT_BLOCK_SIZE, DEFAULT_SEGMENT_SIZE,
        DEFAULT_SEGMENT_EXT
    );
}

inline vector* new_vector_spec(int block_size, int segment_size,
        int segment_ext){

    vector *nv = (vector*)malloc(sizeof(vector));

    nv->size = 0;
    nv->block_size = block_size;
    nv->segment_size = segment_size;
    nv->segment_ext = segment_ext;

    nv->array_of_T = (T**)malloc(sizeof(T*)*segment_size);
    nv->array_of_T[0] = (T*)malloc(sizeof(T)*block_size);

    nv->top_seg = 0;
    nv->top_bi = 0;
    nv->alloc_seg = 1;

    return nv;
}

inline void push(vector* v, T item){
    v->array_of_T[v->top_seg][v->top_bi] = item;
    v->size++;

    v->top_bi = (v->top_bi+1)%(v->block_size);

    if(v->top_bi == 0){
        v->top_seg++;
    }

    if( v->top_seg >= v->alloc_seg ){

        if( v->top_seg >= v->segment_size ){
            v->segment_size += v->segment_ext;
            v->array_of_T = (T**)realloc(v->array_of_T,
                    sizeof(T*)*(v->segment_size));
        }

        v->array_of_T[v->top_seg] = (T*)malloc(sizeof(T)*(v->block_size));

        v->alloc_seg++;
    }
}

inline int empty(vector* v){
    return v->size == 0;
}

inline T top(vector *v){
    if(v->top_bi == 0){
        return v->array_of_T[(v->top_seg)-1][(v->block_size)-1];
    }
    return v->array_of_T[v->top_seg][v->top_bi-1];
}

inline void pop(vector *v){
    if(v->top_bi == 0){
        v->top_seg--;
        v->top_bi = v->block_size-1;
    }
    else {
        v->top_bi--;
    }
    v->size--;
    check_and_free_blocks(v);
}

inline void multi_pop(vector *v, int n){
    v->size -= n;

    v->top_seg = v->size/v->block_size;
    v->top_bi = v->size%v->block_size;
    check_and_free_blocks(v);
}

inline T get(vector *v, int i){
    int seg = i/(v->block_size);
    int bi = i%(v->block_size);

    return v->array_of_T[seg][bi];
}

inline void set(vector *v, int i, T x){
    int seg = i/(v->block_size);
    int bi = i%(v->block_size);

    v->array_of_T[seg][bi] = x;
}

inline void free_vector(vector *v){
    int i;
    for(i=0;i<=v->alloc_seg;i++){
        free(v->array_of_T[i]);
    }
    free(v->array_of_T);
    free(v);
}

inline void delete_vector(vector *v, void (*del_T)(T*)){
    int i, n=v->size;
    for(i=0;i<=v->alloc_seg;i++){
        int j;
        for(j=0; n>0 && j<v->block_size;j++){
            del_T(&v->array_of_T[i][j]);
            n--;
        }
        free(v->array_of_T[i]);
    }
    free(v->array_of_T);
    free(v);
}

void check_and_free_blocks(vector* v){
    int to_free = (int)(v->segment_size*MAX_FREE_SEGMENT_PERCENTAGE);

    if(v->alloc_seg < v->segment_size - to_free){
        //A 20% of the exess blocks are not freed
        int to_leave = (int)(to_free*0.2);
        int i;
        for(i=v->alloc_seg+to_leave;i<v->alloc_seg;i++){
            free(v->array_of_T[i]);
        }
    }
}
