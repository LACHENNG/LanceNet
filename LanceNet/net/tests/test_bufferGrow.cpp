
#include <LanceNet/net/Buffer.h>
#include <string>
#include <assert.h>
using namespace LanceNet::net;
using namespace std;

int main()
{

  Buffer buf;
  buf.append(string(400, 'y'));
  assert(buf.readableBytes() == 400);
  assert(buf.writeableBytes() == Buffer::kDefaultInitSize-400);

  buf.retrieve(50);
  assert(buf.readableBytes() == 350);
  assert(buf.writeableBytes() == Buffer::kDefaultInitSize-400);
  assert(buf.prependableBytes() == Buffer::kDefaultPrependSize+50);

  buf.append(string(1000, 'z'));
  assert(buf.readableBytes() == 1350);
  assert(buf.writeableBytes() == 0);
  assert(buf.prependableBytes() == Buffer::kDefaultPrependSize+50); // FIXME


  buf.retrieveAll();
  assert(buf.readableBytes() ==  0);
  assert(buf.writeableBytes() == 1400); // FIXME
  assert(buf.prependableBytes() == Buffer::kDefaultPrependSize);
}
