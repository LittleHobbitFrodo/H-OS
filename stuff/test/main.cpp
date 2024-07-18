#include <iostream>

/*typedef struct segment {
    void* next;
    unsigned int size;
    unsigned char opt;
} segment;*/

void* align(void *ptr, size_t align) {
    unsigned long addr = reinterpret_cast<unsigned long>(ptr);
    return reinterpret_cast<void*>((addr + align - 1) & ~(align - 1));
}

int main() {

    std::cout << "align address:" << std::endl;

    void* ptr = (void*)0x0ffdaa216;

    std::cout << "ptr: " << ptr << " : " << align(ptr, 4096) << " : " << (size_t)ptr << std::endl;

    /*std::cout << "is space bigger than 127?" << std::endl;

    size_t start = 1;
    size_t space;
    for (unsigned i = 1; i < 127; i++) { // Start from 1 to avoid division by zero
        // Calculate the starting address plus the size of the segment struct
        space = start + sizeof(segment) + (rand() % 100);

        // Calculate the alignment offset. The alignment value `i` must be a power of two
        size_t offset = (i - (space % i)) % i;
        space += offset;

        std::cout << "true: " << i << " (space: " << space << ")" << std::endl;
    }*/
}