#ifndef BUFFER_H
#define BUFFER_H
#include <mutex>

class Buffer{
public:
    void LoadBlockBuffer(size_t offset);
    void removeDataBuffer();
    // void updateBufferBlock(size_t offset);
    static Buffer* getBuffer();
    virtual ~Buffer(){};

protected:
    Buffer() = default;

    std::mutex buffer_mutex;
    size_t max_size;
    size_t min_size;
    static Buffer* buffer;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
}

#endif // BUFFER_H