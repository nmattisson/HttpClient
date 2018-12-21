#ifndef __I_CHUNK_STREAM_H_
#define __I_CHUNK_STREAM_H_

class IChunkStream {
    public:
        virtual ~IChunkStream() {}
        virtual int getSize() = 0;
        virtual String getNextChunk() = 0;
        virtual int Eof() = 0;
};

#endif /* __I_CHUNK_STREAM_H_ */