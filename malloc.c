#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 기본 상수 정의 */
#define WSIZE       8           // Word size (Bytes) - 헤더/푸터 크기
#define DSIZE       16          // Double word size (Bytes) - 정렬 기준
#define MEM_SIZE    800         // 문제에서 지정한 메모리 풀 크기

/* 비트 조작을 위한 마스크 */
#define ALLOC_MASK      0x1     // 현재 블록 할당 여부 (0:Free, 1:Alloc)
#define PREV_ALLOC_MASK 0x2     // 이전 블록 할당 여부 (0:Free, 1:Alloc)
#define SIZE_MASK       (~0xF)  // 하위 4비트를 지우고 사이즈만 추출

/* 매크로 함수 (안전한 포인터 연산을 위해 괄호 필수) */
// 크기, 현재 할당, 이전 할당 정보를 합쳐서 헤더 값을 만듦
#define PACK(size, alloc, prev_alloc) ((size) | (alloc) | (prev_alloc))

// 주소 p에서 값 읽기/쓰기 (64비트 기준 unsigned long 사용)
#define GET(p)          (*(unsigned long *)(p))
#define PUT(p, val)     (*(unsigned long *)(p) = (val))

// 주소 p에서 사이즈, 할당여부, 이전할당여부 읽기
#define GET_SIZE(p)         (GET(p) & SIZE_MASK)
#define GET_ALLOC(p)        (GET(p) & ALLOC_MASK)
#define GET_PREV_ALLOC(p)   (GET(p) & PREV_ALLOC_MASK)

// 블록 포인터(bp)를 주면 헤더와 푸터의 주소를 반환
#define HDRP(bp)    ((char *)(bp) - WSIZE)
// 주의: FTRP는 현재 블록이 Free 상태일 때만 유효함 (Alloc은 푸터 없음)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 다음 블록과 이전 블록의 포인터(bp) 반환
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(HDRP(bp)))
// 주의: PREV_BLKP는 이전 블록이 Free 상태일 때만 유효함 (Footer를 통해 이동)
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - WSIZE)))

/* 전약 변수 설정*/
static unsigned char mem_pool[MEM_SIZE]; // 800 바이트 크기의 정적 메모리 배열
static unsigned char *heap_listp; // 힙의 시작점을 가리킬 포인터

typedef int data_t;

/* 함수 구현 */
void init_mem(){
    // 1. 초기 힙 포인터 위치 잡기 
    // (배열의 시작주소는 정렬이 안 되어 있을 수 있으므로 정렬을 맞춰주는 작업이 필요할 수 있음)
    // 하지만 보통 배열 선언 시 aligned 속성을 주거나, 여기서 오프셋을 조정함.
    
    // 2. 프롤로그(Prologue) 블록 생성
    // - 힙의 시작을 알리는 16바이트짜리 가짜 블록 (Header + Footer)
    // - 할당된 상태(Alloc=1)로 표시해서 앞으로 넘어가지 못하게 막음
    
    // 3. 거대한 Free 블록 생성 (초기 상태)
    // - 남은 메모리 전체를 하나의 큰 Free 블록으로 만듦
    // - Header 작성 (Size = 남은크기, Alloc = 0, Prev_Alloc = 1)
    // - Footer 작성 (Header와 동일)
    
    // 4. 에필로그(Epilogue) 블록 생성
    // - 힙의 끝을 알리는 헤더 (Size = 0, Alloc = 1)
    
    // 5. heap_listp를 프롤로그 블록 다음(Payload 시작)으로 설정
}

unsigned char *mm_alloc(size_t size){//구현완
    size_t asize; // 조정된 블록 사이즈(헤더 + 정렬 패딩)
    char *bp; // 현재 블록의 payload 시작점

    // 1. size == 0 인 경우 NULL 처리
    if (size == 0) return NULL;

    // 2. 사이즈 조정 (Align)
    asize = round_up(size + WSIZE, DSIZE);
    
    // 3. 가용 리스트 탐색 (Find Fit)
    bp = find_fit(asize);

    if(bp == NULL){
        return NULL
    }

    // 4. 찾았을 경우 배치(Place)
    place(bp, asize);

    return bp;
}

void mm_free(unsigned char *p){

}

void show_mm(){//출력하는것


}

static void *find_fit(size_t asize){//구현완
    void* bp;//block pointer 선언

    for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(GET_ALLOC(HDRP(bp))==0 && GET_SIZE(HDRP(bp))>=asize){
            return bp;
        }
    }
    return NULL;
    //헤더정보를가지고 할당한 사이즈정보, 하위 첫번째 비트(할당여부)를 확인하여 진행
}

static void place(void *bp, size_t asize){
    size_t csize = GET_SIZE(HDRP(bp)); // current block size를 담을 변수
    void* next_bp = NEXT_BLKP(HDRP(bp));
    int isPrevAlloc = GET_PREV_ALLOC(HDRP(bp));
    
    if(csize - asize<16){
        PUT(HDRP(bp), PACK(csize, 1, isPrevAlloc)); //할당되었다고 업데이트하기
        PUT(HDRP(next_bp), PACK(GET_SIZE(HDRP(next_bp)), GET_ALLOC(HDRP(next_bp)), 2))//다음 블록의 헤더도 이전 블록이 할당되었다고 업데이트하기
    }
    else{
        PUT(HDRP(bp),)
    }
    /* 
    Todo
        1. 현재 블록의 전체 크기 가져오기
        2. (전체 크기 - 필요 크기)가 최소 블록 크기(16) 보다 크다면?
            -> 앞부분은 할당(Header만 씀, Next Block의 Prev_Alloc 1로 설정)
            -> 뒷부분은 Free(Header + Footer 씀, NextBlock의 Prev_Alloc 0으로 설정)
        3. 아니면?
            -> 그냥 통째로 할당 상태로 변경
    */
    // - 만약 남는 공간이 충분히 크면 쪼갠다 (Split)
    // - 아니면 통째로 준다
    // - 리턴: 블록의 포인터(bp)

}

int round_up(int n, int m){//구현완
    return ((n+m-1)/m)*m;
}

#define DO_SHOW(cmd) printf("<%s>\n", #cmd); (cmd); show_mm()
int main(){
    unsigned char *p[10];
    DO_SHOW(mm_init());
    DO_SHOW(p[0] = mm_alloc(100));
    DO_SHOW(p[1] = mm_alloc(300));
    DO_SHOW(p[2] = mm_alloc(70));
    DO_SHOW(p[3] = mm_alloc(180));
    DO_SHOW(mm_free(p[2]));
    DO_SHOW(mm_free(p[3]));
    DO_SHOW(p[4]= mm_alloc(50));
    DO_SHOW(mm_free(p[0]));
    DO_SHOW(mm_free(p[1]));
    DO_SHOW(p[5] = mm_alloc(120));
    DO_SHOW(mm_free(p[4]));
    DO_SHOW(mm_free(p[5]));


    return 0;
}