

#include <LanceNet/net/Buffer.h>
#include <string>
#include <assert.h>
using namespace LanceNet::net;
using namespace std;

int main()
{
    Buffer buf(8, 16);
    //assert(buf.size(), 8 + 16);
    int msg_len = 16;
    buf.preappend(msg_len);
    buf.append("0123456789123456");

    assert(buf.size() == 8 + 16);
    assert(buf.prependableBytes() == 8 - sizeof(int));
    assert(buf.writeableBytes() ==  0);
    
    assert(buf.peekInt32() ==  msg_len);

    return 0;
}
