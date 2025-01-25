struct buffer
{
    u8 *data;
    size_t count;
};

#define CONSTANT_STRING(String)            \
    {                                      \
        sizeof(String) - 1, (u8 *)(String) \
    }

void exit(const char *msg)
{
    printf("Exit reason %s", msg);
    exit(0);
}

buffer create_buffer(u64 bytes)
{
    buffer result;
    result.data = (u8 *)malloc(bytes);
    if (result.data == nullptr)
    {
        exit("out of memory");
    }
    result.count = bytes;
    return result;
}

void free_buffer(buffer *b)
{
    free(b->data);
    b = {};
}

bool eqaulBuffer(buffer b1, buffer b2)
{
    if (b1.count != b2.count)
    {
        return false;
    }
    if (b1.data == b2.data)
    {
        return true;
    }
    for (u64 idx = 0; idx < b1.count; ++idx)
    {
        if (b1.data[idx] != b2.data[idx])
        {
            return false;
        }
    }
    return true;
}
