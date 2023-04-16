// Note: stolen from muduo
// Modified by Lance @ nwpu

#include <LanceNet/net/Buffer.h>
#include <boost/test/tools/old/interface.hpp>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

// Automatically generates a main function to initialize and run the test module
#define BOOST_TEST_MODULE buffer_test
// #define BOOST_TEST_MAIN
// link the dynamic library of the boot.test library instead of static lib(default)
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace LanceNet::net;

using std::string;

BOOST_AUTO_TEST_CASE(testBufferAppendRetrieve)
{
  Buffer buf;
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);

  const string str(200, 'x');
  buf.append(str);
  BOOST_CHECK_EQUAL(buf.readableBytes(), str.size());
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize - str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);

  const string str2 =  buf.retrieveAsString(50);
  BOOST_CHECK_EQUAL(str2.size(), 50);
  BOOST_CHECK_EQUAL(buf.readableBytes(), str.size() - str2.size());
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize - str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize + str2.size());
  BOOST_CHECK_EQUAL(str2, string(50, 'x'));

  buf.append(str);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 2*str.size() - str2.size());
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize - 2*str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize + str2.size());

  const string str3 =  buf.retrieveAllAsString();
  BOOST_CHECK_EQUAL(str3.size(), 350);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);
  BOOST_CHECK_EQUAL(str3, string(350, 'x'));
}

BOOST_AUTO_TEST_CASE(testBufferGrow)
{
  Buffer buf;
  buf.append(string(400, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 400);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-400);

  buf.retrieve(50);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 350);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-400);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize+50);

  buf.append(string(1000, 'z'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 1350);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize+50); // FIXME


  buf.retrieveAll();
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), 1400); // FIXME
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);
}

BOOST_AUTO_TEST_CASE(testBufferInsideGrow)
{
  Buffer buf;
  buf.append(string(800, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 800);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-800);

  buf.retrieve(500);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 300);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-800);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize+500);

  buf.append(string(300, 'z'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 600);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-600);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);
}

BOOST_AUTO_TEST_CASE(testBufferShrink)
{
  Buffer buf;
  buf.append(string(2000, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 2000);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);

  buf.retrieve(1500);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 500);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize+1500);

  // buf.shrink(0);
  // BOOST_CHECK_EQUAL(buf.readableBytes(), 500);
  // BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-500);
  // BOOST_CHECK_EQUAL(buf.retrieveAllAsString(), string(500, 'y'));
  // BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);
}

BOOST_AUTO_TEST_CASE(testBufferPrepend)
{
  Buffer buf;
  buf.append(string(200, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 200);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-200);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);

  int x = 0;
  
  buf.preappend(&x, sizeof x);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 204);
  BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize-200);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize - 4);
}

BOOST_AUTO_TEST_CASE(testBufferReadInt)
{
  Buffer buf;
  // binary repr : 0x48545450 (in big edian order)
  buf.append("HTTP");

  // printf("hex: %#x ", buf.peek()[0]) ;
  // printf("%#x ", buf.peek()[1]) ;
  // printf("%#x ", buf.peek()[2]) ;
  // printf("%#x \n", buf.peek()[3]) ;

  BOOST_CHECK_EQUAL(buf.readableBytes(), 4);
  BOOST_CHECK_EQUAL(buf.peekInt8(), 'H');

  int16_t top16 = ntohs(buf.peekInt16());

  // printf("top16 %#x \n", top16) ;
  // 0x5448 ?
  // printf("top16 hex: %#x\n", top16) ;

  // printf("ntohs(top16) hex: %#x\n", top16) ;
  BOOST_CHECK_EQUAL(top16, ((int)'H' << 8) | (int)'T');
  //FIXME: fixed
  // BOOST_CHECK_EQUAL(buf.peekInt32(), (top16 << 16) | ('T' << 8) | ('P') );
  BOOST_CHECK_EQUAL(ntohl(buf.peekInt32()), (top16 << 16) | ('T' << 8) | ('P') );

  BOOST_CHECK_EQUAL(buf.readInt8(), 'H');
  BOOST_CHECK_EQUAL(buf.readInt16(), 'T'*256 + 'T');
  BOOST_CHECK_EQUAL(buf.readInt8(), 'P');
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  // unreasonable test
  // BOOST_CHECK_EQUAL(buf.writeableBytes(), Buffer::kDefaultInitSize);

  for(int i = 0; i < Buffer::kDefaultInitSize - 4 - 1; i++)
  {
    buf.append("L", 1);
  }

  BOOST_CHECK_EQUAL(buf.readableBytes(), Buffer::kDefaultInitSize - 5);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), 4 + Buffer::kDefaultPrependSize);

  // this will cause to internal buffer to the front, to make space internally
  buf.append("12345", 5);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kDefaultPrependSize);
  BOOST_CHECK_EQUAL(buf.readableBytes(), Buffer::kDefaultInitSize);
  BOOST_CHECK_EQUAL(buf.readableBytes(), Buffer::kDefaultInitSize);

  buf.retrieveAll();

  buf.appendInt32(23);
  buf.appendInt8(-1);
  buf.appendInt16(-2);
  buf.appendInt32(-3);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 11);
  BOOST_CHECK_EQUAL(buf.readInt32(), 23);
  BOOST_CHECK_EQUAL(buf.readInt8(), -1);
  BOOST_CHECK_EQUAL(buf.readInt16(), -2);
  BOOST_CHECK_EQUAL(buf.readInt32(), -3);
}

BOOST_AUTO_TEST_CASE(testBufferFindEOL)
{
  Buffer buf;
  buf.append(string(100000, 'x'));
  const char* null = NULL;
  // BOOST_CHECK_EQUAL(buf.readableBytes(), 100000);
  // BOOST_CHECK_EQUAL(buf.retrieveAllAsString(), string(100000, 'x'));

  // buf.retrieve(90000);
  // BOOST_CHECK_EQUAL(buf.readableBytes(), 10000);

  BOOST_CHECK_EQUAL(buf.findEOL(), null);
  // printf("begin %p\n", buf.peek());
  // printf("end %p\n", buf.peek() + buf.readableBytes());
  // printf("end char : %c ", *(buf.peek() + buf.readableBytes() - 1));
  // printf("begin + 90000: %p\n", buf.peek() + 90000);

  // out of range
  // BOOST_CHECK_EQUAL(buf.findEOL(buf.peek()+90000), null);
  // BOOST_CHECK_EQUAL(buf.findEOL(buf.peek() + 100000), null);
}

void output(Buffer&& buf, const void* inner)
{
  Buffer newbuf(std::move(buf));
  // printf("New Buffer at %p, inner %p\n", &newbuf, newbuf.peek());
  BOOST_CHECK_EQUAL(inner, newbuf.peek());
}

// NOTE: This test fails in g++ 4.4, passes in g++ 4.6.
BOOST_AUTO_TEST_CASE(testMove)
{
  Buffer buf;
  buf.append("lance", 5);
  const void* inner = buf.peek();
  // printf("Buffer at %p, inner %p\n", &buf, inner);
  output(std::move(buf), inner);
}
